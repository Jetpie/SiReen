#include "image_feature_extract.hpp"

/*
 * Default Constructer
 */
ImageCoder::ImageCoder(void)
{
    this->stdWidth = 128;
    this->stdHeight = 128;
    this->step = 8;
}
ImageCoder::~ImageCoder(void){}


/*
 * Constructer overloading
 */
ImageCoder::ImageCoder(int stdWidth, int stdHeight, int step)
{
    this->stdWidth = stdWidth;
    this->stdHeight = stdHeight;
    this->step = step;
}


/*
 * decode dense-sift descripter
 * @param srcImg opencv IplImage
 * @param size
 * @param binSize
 * @return VlDsiftFilter
 */
VlDsiftFilter* ImageCoder::dsiftDescripter(Mat srcImage, int& size, int binSize)
{
    // check if source image is graylevel
    Mat grayImage;
    if (srcImage.channels() != 1)
        cvtColor(srcImage,grayImage,CV_BGR2GRAY);
    else
        grayImage = srcImage;

    // resize image
    Mat stdImage;
    resize(srcImage, stdImage, Size(this->stdWidth,this->stdHeight),
           0, 0, INTER_LINEAR);

    // validate
    if( ! stdImage.data)
        return 0;

    // init descriptors
    VlDsiftFilter* dsiftFilter =
        vl_dsift_new_basic(stdImage.cols, stdImage.rows,
                           this->step, binSize);
    // switch off gaussian windowing
    vl_dsift_set_flat_window(dsiftFilter,'true');

    // convert mat to float vector
    vector<float> imData;
    for (int i = 0; i < stdImage.rows; ++i)
        for (int j = 0; j < stdImage.cols; ++j)
            imData.push_back(stdImage.at<unsigned char>(i, j));
    // process an image data
    vl_dsift_process(dsiftFilter,&imData[0]);

    int descrSize = vl_dsift_get_descriptor_size(dsiftFilter);
    int nKeypoints = vl_dsift_get_keypoint_num(dsiftFilter);
    size = size + descrSize*nKeypoints;
    return dsiftFilter;
}



string ImageCoder::llcDescripter(float *Descriptors, float *B, int ncb, int size)
{
    int binsize = 128;                                              // 每个单词的尺寸
    int cc, nfm = size/binsize, knn = 5;                     // ncb:字典大小，nfm：一幅图patch数量，knn：local的限制
    float *LLC = new float[ncb];                                   // 为LLC分配内存
    Rect rect = Rect(0,0,knn,nfm);
    Mat Desc = cv::Mat(nfm,binsize,CV_32FC1,Descriptors);
    Mat codebook = cv::Mat(ncb,binsize,CV_32FC1,B);
    //for(int i=0; i<10; i++)
    //cout << "Desc = " << endl << " " << Desc.at<float>(0,i) << endl;

    /************* 1.计算公式：每个patch到每个单词的距离，存入D中***********/
    Mat DD = Desc.mul(Desc);
    Mat BB = codebook.mul(codebook);
    Mat Dd = Mat::zeros(nfm,1,CV_32FC1);
    Mat Bb = Mat::zeros(ncb,1,CV_32FC1);
    for(int i=0; i<binsize; i++)
    {
        cv::add(Dd,DD.col(i),Dd);
        cv::add(Bb,BB.col(i),Bb);
    }
    DD = repeat(Dd,1,ncb);
    BB = repeat(Bb.t(),nfm,1);

    /************** 2.然后对D进行排序，取前knn个最小值, 计算编码************/
    Mat idx, IDX, II, Ctmp;
    cv::sortIdx(DD - 2*Desc*codebook.t() + BB,idx,0);
    IDX = idx(rect);
    DD.release(), Dd.release(), BB.release(), Bb.release();

    II = Mat::eye(knn,knn,CV_32FC1);
    II = II.mul(1e-4);
    Mat LLCall = Mat::zeros(nfm,ncb,CV_32FC1);
    Mat Z = Mat::zeros(knn,binsize,CV_32FC1);
    for(int i=0; i<nfm; i++)
    {
        for(int ii=0; ii<knn; ii++)
        {
            Z.row(ii) = abs(codebook.row(IDX.at<int>(i,ii) ) - Desc.row(i));
        }

        mulTransposed(Z, Ctmp, 0);
        Mat C = Ctmp + II.mul(trace(Ctmp));
        Mat LLCtmp = C.inv()*Mat::ones(knn,1,CV_32FC1);
        divide(LLCtmp,sum(LLCtmp),LLCtmp);                                  //归一化
        for(int j=0; j<knn; j++)
            LLCall.at<float>(i,IDX.at<int>(i,j)) = LLCtmp.at<float>(j,0);
    }
    Ctmp.release(), Z.release(), II.release(), idx.release();
    transpose(LLCall,LLCall);                                               // 算得全局LLC编码
    //cout << "LLCall = " << endl << " " << LLCall << endl << endl;

    /************** 3.通过for循环从矩阵LLCall中读取最大值作为LLC编码，存入LLC数组 ************/
    float maxLocTmp;                                                        // 每个patch的LLC系数存储
    float sumSqrtMax = 0;
    float *pData;
    for(int i=0; i<ncb; i++)
        //
    {
        pData = LLCall.ptr<float>(i);
        maxLocTmp = pData[0];
        for(int j=1; j<nfm; j++)
        {
            if(pData[j] > maxLocTmp)
                maxLocTmp = pData[j];
        }
        LLC[i] = maxLocTmp;
        sumSqrtMax = sumSqrtMax + maxLocTmp*maxLocTmp;
        //cout << "LLC = " << LLC[i] << endl;
    }

    ostringstream s;
    /****  归一化  *****/
    sumSqrtMax = sqrt(sumSqrtMax);
    for(int i=0; i<ncb; i++)
    {
        LLC[i] = LLC[i] / sumSqrtMax;
        s<<LLC[i];
        //s<<setprecision(4)<<LLC[i];
        if(i<ncb-1)
        {
            s<<",";
        }
        //string llcstr(s.c_str());
    }
    return s.str();
}

/*
 * Normalization of dense sift desripters
 *
 */
float* ImageCoder::normalizeSift(float *Descriptors, int descrSize, int nKeypoints)
{
    float *normDesc = new float[size];
    float temp,temp1;                                           //temp1和temp为存储每一个patch对应的128维向量的均方根
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
            temp  = temp  + Descriptors[i*128 + j] * Descriptors[i*128 + j];
        }
        temp = sqrt(temp);

        //对于均方差大于1的向量，做归一化处理
        if(temp > 1)
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = Descriptors[i*128 + j]/temp;
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
                normDesc[i*128 + j] = Descriptors[i*128 + j];
            }
        }
    }

    return normDesc;
}
