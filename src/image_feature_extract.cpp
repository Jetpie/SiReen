#include "sireen/image_feature_extract.hpp"

/*
 * Default Constructer
 */
ImageCoder::ImageCoder(void)
{
    this->dsiftFilter = NULL;
    /* default setting */
    this->setParams(128,128,8,16);

    int descrSize = vl_dsift_get_descriptor_size(this->dsiftFilter);
    int nKeypoints = vl_dsift_get_keypoint_num(this->dsiftFilter);
}

/*
 * Constructer overloading
 */
ImageCoder::ImageCoder(int stdWidth, int stdHeight, int step, int binSize)
{
    this->dsiftFilter = NULL;
    this->setParams(stdWidth,stdHeight,step,binSize);
}
/*
 * Constructer overloading
 */
ImageCoder::ImageCoder(VlDsiftFilter* filter)
{
    this->dsiftFilter = filter;
    // switch off gaussian windowing
    vl_dsift_set_flat_window(dsiftFilter,true);

    // assume that x part equals to y part
    this->stdWidth = filter->imWidth;
    this->stdHeight = filter->imHeight;
    this->step = filter->stepX;
    this->binSize = filter->geom.binSizeX;
    this->imData = new vl_sift_pix[this->stdWidth*this->stdHeight];
}

/*
 * Default Destructor
 */
ImageCoder::~ImageCoder(void){
    vl_dsift_delete(this->dsiftFilter);
    delete this->imData;
}

void
ImageCoder::setParams(int stdWidth, int stdHeight, int step, int binSize)
{
    this->stdWidth = stdWidth;
    this->stdHeight = stdHeight;
    this->step = step;
    this->binSize = binSize;
    this->imData = new vl_sift_pix[this->stdWidth*this->stdHeight];
    if(this->dsiftFilter)
    {

        cout<< "set filter from EXIST"<<endl;
        this->dsiftFilter->imWidth = stdWidth;
        this->dsiftFilter->imHeight = stdHeight;
        VlDsiftDescriptorGeometry geom =
            *vl_dsift_get_geometry(this->dsiftFilter);
        geom.binSizeX = binSize ;
        geom.binSizeY = binSize ;
        vl_dsift_set_geometry(this->dsiftFilter, &geom);
        vl_dsift_set_steps(this->dsiftFilter, step, step);
    }
    else
    {
        cout<< "set filter from NULL"<<endl;

        this->dsiftFilter =
            vl_dsift_new_basic(stdWidth, stdHeight, step, binSize);

        // switch off gaussian windowing
        vl_dsift_set_flat_window(this->dsiftFilter,true);
    }
}


/*
 * decode dense-sift descripters
 *
 * @param srcImage opencv Mat image
 * @return float*
 */
float*
ImageCoder::dsiftDescripter(Mat srcImage)
{
    // check if source image is graylevel
    if (srcImage.channels() != 1)
        cvtColor(srcImage,srcImage,CV_BGR2GRAY);

    // resize image
    if(!(srcImage.cols==this->stdWidth
         && srcImage.rows==this->stdHeight))
        resize(srcImage, srcImage, Size(this->stdWidth,this->stdHeight),
               0, 0, INTER_LINEAR);

    // validate
    if(!srcImage.data || srcImage.cols == 0)
        return NULL;

    // get valid input for dsift process
    // memset here to prevent memory leak
    memset(this->imData, 0, sizeof(vl_sift_pix) * this->stdWidth*this->stdHeight);
    uchar * rowPtr;
    for (int i=0; i<srcImage.rows; i++)
    {
        rowPtr = srcImage.ptr<uchar>(i);
        for (int j=0; j<srcImage.cols; j++)
        {
            imData[i*srcImage.cols+j] = rowPtr[j];
        }
    }

    // process an image data
    vl_dsift_process(this->dsiftFilter,imData);

    // return the (unnormalized) sift descripters
    // by default, the vlfeat library has normalized the descriptors
    // our following  normalization eliminates those peaks (big value gradients)
    // and re-normalize them
    return this->dsiftFilter->descrs;
}

/*
 * compute linear local constraint coding descripter
 * @param srcImage source image in opencv mat format
 * @param codebook codebook from sift-kmeans
 * @param ncb dimension of codebook
 * @param k get top k nearest codes
 */
string
ImageCoder::llcDescripter(Mat srcImage, float *codebook,const int ncb, int k)
{

    float* dsiftDescr = this->dsiftDescripter(srcImage);
    if(!dsiftDescr)
        throw runtime_error("image not loaded or resized properly");
    // get sift descripter size and number of keypoints
    int descrSize = vl_dsift_get_descriptor_size(dsiftFilter);
    int nKeypoints = vl_dsift_get_keypoint_num(dsiftFilter);

    // float* dsiftNorm = this->normalizeSift(dsiftDescr,descrSize*nKeypoints);
    // Map<MatrixXf> matdsift2(dsiftNorm,descrSize,nKeypoints);

    // eliminate peak gradients and normalize
    // initialize dsift descripters and codebook Eigen matrix
    MatrixXf matdsift= this->normSift(dsiftDescr,descrSize,nKeypoints,true);
    Map<MatrixXf> matcb(codebook,descrSize,ncb);

    // Step 1 - compute eucliean distance and sort
    // only in the case if all the sift features are not sure to
    // be nomalized to sum square 1, we arrange the distance as following
    MatrixXi knnIdx(nKeypoints, k);
    MatrixXf cdist(nKeypoints,ncb);
    // get euclidean distance of pairwise column features
    // use the trick of (u-v)^2 = u^2 + v^2 - 2uv
    // with assumption of Eigen column-wise manipulcation
    // is quite fast
    cdist = ( (matdsift.transpose() * matcb * -2).colwise()
              + matdsift.colwise().squaredNorm().transpose()).rowwise()
              + matcb.colwise().squaredNorm();

    typedef std::pair<double,int> value_and_index;
    for (int i = 0; i< nKeypoints; i++)
    {
        std::priority_queue<value_and_index,
                            std::vector<value_and_index>,
                            std::greater<value_and_index>
                            > q;
        // use a priority queue to implement the pop top k
        for (int j = 0; j < ncb; j++)
            q.push(std::pair<double, int>(cdist(i,j),j));

        for (int n = 0; n < k; n++ )
        {
            knnIdx(i,n) = q.top().second;
            q.pop();
        }

    }

    // Step 2 - compute the covariance and solve the analytic solution
    // put the results into llc cache

    // declare temp variables
    MatrixXf I = MatrixXf::Identity(k,k) * (1e-4);
    MatrixXf caches = MatrixXf::Zero(nKeypoints,ncb);
    MatrixXf U(descrSize,k);
    MatrixXf covariance(k,k);
    VectorXf cHat(k);
    for(int i=0;i<nKeypoints;i++)
    {
        for(int j=0;j<k;j++)
            U.col(j) = (matcb.col(knnIdx(i,j)) - matdsift.col(i))
                .cwiseAbs();
        // compute covariance
        covariance = U.transpose()*U;
        cHat = (covariance + I*covariance.trace())
            .fullPivLu().solve(VectorXf::Ones(k));

        cHat = cHat / cHat.sum();
        for(int j = 0 ; j < k ; j++)
            caches(i,knnIdx(i,j)) = cHat(j);
    }

    // Step 3 - get the llc descripter and normalize
    // get max coofficient for each column
    VectorXf llc = caches.colwise().maxCoeff();
    // normalization
    llc.normalize();

    // output the result in squeezed form
    // (i.e. bis after floating points are omitted)
    ostringstream s;
    s << llc(0);
    for(int i=1; i<ncb; i++)
    {
        s << ",";
        s << llc(i);
    }
    // release memory
    // delete dsiftNorm;
    return s.str();
}

/*
 * Normalization of dense sift desripters
 * @param descriptors DsiftFilter->Descr
 * @param size number of Keypoints * descriptor size
 */
float* ImageCoder::normalizeSift(float *descriptors, int size)
{
    float *normDesc = new float[size];
    //temp1和temp为存储每一个patch对应的128维向量的均方根
    float temp,temp1;
    int i,j;

    for(i=0; i<size/128; i++)
        //每128个元素对应一个patch的特征向量
        //分别对每个特征向量进行归一化
    {
        temp = 0;
        temp1 = 0;
        for(j=0; j<128; j++)
            //求均方差
        {
            //printf("%f\n",Descriptors[i*128 + j]);
            temp  = temp  + descriptors[i*128 + j] * descriptors[i*128 + j];
        }
        temp = sqrt(temp);

        //对于均方差大于1的向量，做归一化处理
        if(temp > 1)
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = descriptors[i*128 + j] / temp;
                if(normDesc[i*128 + j] > 0.2)
                {
                    normDesc[i*128 + j] = 0.2;    //消除梯度太大的量，即归一化后大于0.2的量，强制等于0.2
                }
                temp1 = temp1 + normDesc[i*128 + j]*normDesc[i*128 + j];
            }
            temp1 = sqrt(temp1);
            for(j=0; j<128; j++)
                //第二次归一化
            {
                normDesc[i*128 + j] = normDesc[i*128 + j]/temp1;
            }

        }
        else
            //对于均方差小于1的向量，不做归一化处理
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = descriptors[i*128 + j];
            }
        }
    }

    return normDesc;
}


/*
 * Optimized sift feature improvement and normalization
 * @param descriptors sift descriptors
 * @param row number of rows
 * @param col number of column
 * @param normalized flag for normalized input
 */
Eigen::MatrixXf
ImageCoder::normSift(float *descriptors, int row, int col, bool normalized=false)
{
    // use Eigen Map to pass float* to MatrixXf
    Map<MatrixXf> matdsift(descriptors,row,col);
    // clock_t s = clock();
    // check flag if the input is already normalized
    if(normalized)
    {
        cout << "begin" << endl;
        for(int i=0;i<col;i++)
        {
            // safely check all values not equals to 0
            // to prevent float division exception
            if((matdsift.col(i).array()>0).any())
            {
                // suppress the sharp (>0.2) features
                matdsift.col(i) = (matdsift.col(i).array() > 0.2)
                    .select(0.2,matdsift.col(i));
                // final normalization
                matdsift.col(i).normalize();
            }

        }
    }
    else
    {
        for(int i=0;i<col;i++)
        {   // compute root l2 norm
            float norm = matdsift.col(i).norm();
            if(norm > 0)
            {   // normalization and suppression
                matdsift.col(i) = ((matdsift.col(i).array() / norm)
                                        > 0.2).select(0.2,matdsift.col(i));
                // normalization
                matdsift.col(i).normalize();
            }

        }
    }
    return matdsift;
}
