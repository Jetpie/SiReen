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
// License: GPLv3

#ifndef SIREEN_IMAGE_FEATURE_EXTRACT_H_
#define SIREEN_IMAGE_FEATURE_EXTRACT_H_
#include <fstream>
#include <string.h>
#include <queue>
#include <stdexcept>

// opencv header
#include <opencv2/opencv.hpp>

// Eigen Linear Algebra
#include <Eigen/Dense>

using namespace cv;
using namespace std;
using namespace Eigen;

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
#include <vl/dsift.h>
};

// Image Coder Class
// Sample Usage:
//    ImageCoder icoder;
//    Mat src_image = cv::imread(path_to_image,0);
//    std::string result = icoder.LLCDescriptor(src_image);
class ImageCoder
{

private:
    // standard resize width
    int std_width_;
    // standard resize height
    int std_height_;
    // sift sampleing step size
    unsigned int step_;
    // sift bin size
    unsigned int bin_size_;
    // dsift filter
    VlDsiftFilter* dsift_filter_;
    // image data buffer
    // the buffer always contains current image pixel values
    vl_sift_pix* image_data_;
    void SetParams(int std_width, int std_height, int step, int bin_size);


public:
    ImageCoder(void);
    ImageCoder(int,int,int,int);
    ImageCoder(VlDsiftFilter*);
    ~ImageCoder(void);

    float* DsiftDescriptor(Mat);
    string LLCDescriptor(Mat, float*, const int, const int);
    Eigen::MatrixXf NormSift(float *, int, int, const bool);

};
#endif //SIREEN_IMAGE_FEATURE_EXTRACT_H_
