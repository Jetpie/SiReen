// Functions for reading, encoding and normalizeing image features
//
// @author: Bingqing Qu
//
// For implentation details, refer to:
//
// Jinjun Wang; Jianchao Yang; Kai Yu; Fengjun Lv; Huang, T.;
// Yihong Gong, "Locality-constrained Linear Coding for image
// classification, " Computer Vision and Pattern Recognition (CVPR),
// 2010 IEEE Conference on , vol., no., pp.3360,3367, 13-18 June 2010
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory

#include "sireen/image_feature_extract.hpp"
/**
 * Default constuctor
 */
ImageCoder::ImageCoder(void)
{
    this->dsift_filter_ = NULL;
    /* default setting */
    this->SetParams(128,128,8,16);
}
/**
 * Constructer overloading
 */
ImageCoder::ImageCoder(int std_width, int std_height, int step, int bin_size)
{
    this->dsift_filter_ = NULL;
    this->SetParams(std_width,std_height,step,bin_size);
}
/**
 * Constructer overloading
 */
ImageCoder::ImageCoder(VlDsiftFilter* filter)
{
    this->dsift_filter_ = filter;
    // switch off gaussian windowing
    vl_dsift_set_flat_window(dsift_filter_,true);

    // assume that x part equals to y part
    this->std_width_ = filter->imWidth;
    this->std_height_ = filter->imHeight;
    this->step_ = filter->stepX;
    this->bin_size_ = filter->geom.binSizeX;
    this->image_data_ = new vl_sift_pix[this->std_width_*this->std_height_];
}
/**
 * Destructor
 */
ImageCoder::~ImageCoder(void){
    vl_dsift_delete(this->dsift_filter_);
    delete this->image_data_;
}

/**
 * set dsiftfilter parameters of vlfeat library
 */
void
ImageCoder::SetParams(int std_width, int std_height, int step, int bin_size)
{
    this->std_width_ = std_width;
    this->std_height_ = std_height;
    this->step_ = step;
    this->bin_size_ = bin_size;
    this->image_data_ = new vl_sift_pix[this->std_width_*this->std_height_];
    // if dsift filter was initialized
    if(this->dsift_filter_)
    {
        cout<< "set filter from EXIST"<<endl;
        this->dsift_filter_->imWidth = std_width_;
        this->dsift_filter_->imHeight = std_height_;
        VlDsiftDescriptorGeometry geom =
            *vl_dsift_get_geometry(this->dsift_filter_);
        geom.binSizeX = bin_size ;
        geom.binSizeY = bin_size ;
        vl_dsift_set_geometry(this->dsift_filter_, &geom);
        vl_dsift_set_steps(this->dsift_filter_, step, step);
    }
    else
    {
        cout<< "set filter from NULL"<<endl;

        this->dsift_filter_ =
            vl_dsift_new_basic(std_width, std_height, step, bin_size);

        // switch off gaussian windowing
        vl_dsift_set_flat_window(this->dsift_filter_,true);
    }
}


/**
 * encode dense-sift descriptors
 *
 * @param src_image opencv Mat image
 * @return the dense sift float-point descriptors
 */
float*
ImageCoder::DsiftDescriptor(Mat src_image)
{
    // check if source image is graylevel
    if (src_image.channels() != 1)
        cvtColor(src_image,src_image,CV_BGR2GRAY);

    // resize image
    if(src_image.rows != 0 || !(src_image.cols==this->std_width_
                               && src_image.rows==this->std_height_))
        resize(src_image, src_image, Size(this->std_width_,this->std_height_),
               0, 0, INTER_LINEAR);

    // validate
    if(!src_image.data)
        return NULL;

    // get valid input for dsift process
    // memset here to prevent memory leak
    memset(this->image_data_, 0, sizeof(vl_sift_pix)
           * this->std_width_*this->std_height_);
    uchar * row_ptr;
    for (int i=0; i<src_image.rows; ++i)
    {
        row_ptr = src_image.ptr<uchar>(i);
        for (int j=0; j<src_image.cols; ++j)
        {
            this->image_data_[i*src_image.cols+j] = row_ptr[j];
        }
    }

    // process an image data
    vl_dsift_process(this->dsift_filter_,this->image_data_);

    // return the (unnormalized) sift descriptors
    // by default, the vlfeat library has normalized the descriptors
    // our following  normalization eliminates those peaks (big value gradients)
    // and re-normalize them
    return this->dsift_filter_->descrs;
}

/**
 * compute linear local constraint coding descriptor
 *
 * @param src_image source image in opencv mat format
 * @param codebook  codebook from sift-kmeans
 * @param ncb       dimension of codebook
 * @param k         get top k nearest codes
 */
string
ImageCoder::LLCDescriptor(Mat src_image, float *codebook,
                          const int ncb, const int k)
{

    float* dsift_descr = this->DsiftDescriptor(src_image);
    if(!dsift_descr)
        throw runtime_error("image not loaded or resized properly");
    // get sift descriptor size and number of keypoints
    int descr_size = vl_dsift_get_descriptor_size(dsift_filter_);
    int n_keypoints = vl_dsift_get_keypoint_num(dsift_filter_);

    // eliminate peak gradients and normalize
    // initialize dsift descriptors and codebook Eigen matrix
    MatrixXf mat_dsift= this->NormSift(dsift_descr,descr_size,n_keypoints,true);
    Map<MatrixXf> mat_cb(codebook,descr_size,ncb);

    // Step 1 - compute eucliean distance and sort
    // only in the case if all the sift features are not sure to
    // be nomalized to sum square 1, we arrange the distance as following
    MatrixXi knn_idx(n_keypoints, k);
    MatrixXf cdist(n_keypoints,ncb);
    // get euclidean distance of pairwise column features
    // use the trick of (u-v)^2 = u^2 + v^2 - 2uv
    // with assumption of Eigen column-wise manipulcation
    // is quite fast
    cdist = ( (mat_dsift.transpose() * mat_cb * -2).colwise()
              + mat_dsift.colwise().squaredNorm().transpose()).rowwise()
              + mat_cb.colwise().squaredNorm();

    // The idea behand this is according to Jinjun Wang et al.(2010)
    // section 3, an approximate fast encoding llc can be achieved by
    // keeping only the significant top k values and set others to 0.
    typedef std::pair<double,int> ValueAndIndex;
    for (int i = 0; i< n_keypoints; ++i)
    {
        std::priority_queue<ValueAndIndex,
                            std::vector<ValueAndIndex>,
                            std::greater<ValueAndIndex>
                            > q;
        // use a priority queue to implement the pop top k
        for (int j = 0; j < ncb; ++j)
            q.push(std::pair<double, int>(cdist(i,j),j));

        for (int n = 0; n < k; ++n )
        {
            knn_idx(i,n) = q.top().second;
            q.pop();
        }

    }

    // Step 2 - compute the covariance and solve the analytic solution
    // put the results into llc cache

    // declare temp variables
    // identity matrix
    MatrixXf I = MatrixXf::Identity(k,k) * (1e-4);
    // llc caches
    MatrixXf caches = MatrixXf::Zero(n_keypoints,ncb);
    // subtraction between vectors
    MatrixXf U(descr_size,k);
    // covariance matrix
    MatrixXf covariance(k,k);

    // c^hat in the formular:
    // c^hat_i = (C_i + lambda * diag(d)) \ 1
    // c_i = c^hat_i /1^T *c^hat_i
    // where C_i is the covariance matrix
    VectorXf c_hat(k);

    for(int i=0;i<n_keypoints;++i)
    {
        for(int j=0;j<k;j++)
            U.col(j) = (mat_cb.col(knn_idx(i,j)) - mat_dsift.col(i))
                .cwiseAbs();
        // compute covariance
        covariance = U.transpose()*U;
        c_hat = (covariance + I*covariance.trace())
            .fullPivLu().solve(VectorXf::Ones(k));

        c_hat = c_hat / c_hat.sum();
        for(int j = 0 ; j < k ; ++j)
            caches(i,knn_idx(i,j)) = c_hat(j);
    }

    // Step 3 - get the llc descriptor and normalize
    // get max coofficient for each column
    VectorXf llc = caches.colwise().maxCoeff();

    // normalization
    llc.normalize();

    // output the result in squeezed form
    // (i.e. bis after floating points are omitted)
    ostringstream s;
    s << llc(0);
    for(int i=1; i<ncb; ++i)
    {
        s << ",";
        s << llc(i);
    }

    return s.str();
}

/**
 * Optimized sift feature improvement and normalization
 * @param descriptors sift descriptors
 * @param row         number of rows
 * @param col         number of column
 * @param normalized  flag for normalized input
 */
Eigen::MatrixXf
ImageCoder::NormSift(float *descriptors, int row, int col,
                     const bool normalized=false)
{
    // use Eigen Map to pass float* to MatrixXf
    Map<MatrixXf> mat_dsift(descriptors,row,col);
    // clock_t s = clock();
    // check flag if the input is already normalized
    if(normalized)
    {
        for(int i=0;i<col;++i)
        {
            // safely check all values not equals to 0
            // to prevent float division exception
            if((mat_dsift.col(i).array()>0).any())
            {
                // suppress the sharp (>0.2) features
                mat_dsift.col(i) = (mat_dsift.col(i).array() > 0.2)
                    .select(0.2,mat_dsift.col(i));
                // final normalization
                mat_dsift.col(i).normalize();
            }

        }
    }
    else
    {
        for(int i=0;i<col;++i)
        {   // compute root l2 norm
            float norm = mat_dsift.col(i).norm();
            if(norm > 0)
            {   // normalization and suppression
                mat_dsift.col(i) = ((mat_dsift.col(i).array() / norm)
                                        > 0.2).select(0.2,mat_dsift.col(i));
                // normalization
                mat_dsift.col(i).normalize();
            }

        }
    }
    return mat_dsift;
}
