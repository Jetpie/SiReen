// File: image_feature_extract.hpp
// Description: functions for image feature extractions
// Author: Bingqing Qu
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

    float* DsiftDescriptor(Mat src_image);
    string LLCDescriptor(Mat src_image, float* codebook,
                         const int ncb, const int k);
    Eigen::MatrixXf NormSift(float * descriptors, int row, int col,
                             const bool normalized);

};
#endif //SIREEN_IMAGE_FEATURE_EXTRACT_H_
