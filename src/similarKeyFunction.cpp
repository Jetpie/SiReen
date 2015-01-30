#include "similarHeader.hpp"


similarKeyFunction::similarKeyFunction(void)
{
}


similarKeyFunction::~similarKeyFunction(void)
{
}



VlDsiftFilter* similarKeyFunction::GetDsift(IplImage* SrcImage, int& size, int binSize)
/*********************************************************/
/******************** 计算DSIFT特征 ***********************/
/*********************************************************/
{
    // resize images
    CvSize dst_cvsize;
    dst_cvsize.width = 128;                                                     // resize width
    dst_cvsize.height = 128;                                                    // resize height
    IplImage *Image = cvCreateImage(dst_cvsize, SrcImage->depth,SrcImage->nChannels);
    cvResize(SrcImage, Image, CV_INTER_LINEAR);                                 // resize
    if (Image == NULL)
    {
        return 0;
    }
    // load image dataVlDsiftFilter
    int step = 8;
    vl_sift_pix *ImageData = new vl_sift_pix[Image->height*Image->width];
    unsigned char *Pixel;
    for (int i=0; i<Image->height; i++)
    {
        for (int j=0; j<Image->width; j++)
        {
            Pixel = (unsigned char*)(Image->imageData+i * Image->width+j);
            ImageData[i*Image->width+j] = *(Pixel);
        }
    }
    // init descriptors
    VlDsiftFilter *DsiftFilt = NULL;
    DsiftFilt = vl_dsift_new_basic(Image->width,Image->height,step,binSize);
    int descr_size, n_points;

    // get descriptors
    vl_dsift_set_flat_window(DsiftFilt,'true');
    vl_dsift_process (DsiftFilt, ImageData);
    descr_size = vl_dsift_get_descriptor_size(DsiftFilt);
    n_points = vl_dsift_get_keypoint_num(DsiftFilt);
    size = size + descr_size*n_points;
    //normalizedSift(DsiftFilt->descrs, size);
    delete []ImageData;
    ImageData = NULL;
    cvReleaseImage(&Image);
    return DsiftFilt;
}

/**************************************************/
/***********     Generate LLC     *****************/
/**************************************************/
string similarKeyFunction::llcCompute(float *Descriptors, float *B, int ncb, int size)
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
    // cout << IDX << endl;
    // cout << "IDX:" << IDX.rows << "," << IDX.cols << endl;

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
