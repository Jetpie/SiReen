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

class ImageCoder
{

private:
    /* stand image frame size */
    int stdWidth;
    int stdHeight;
    /* sampling step  */
    unsigned int step;
    /* bin size */
    unsigned int binSize;
    /* dsift filter */
    VlDsiftFilter* dsiftFilter;// = NULL;
    vl_sift_pix* imData;
    void setParams(int stdWidth, int stdHeight, int step, int binSize);
    float* normalizeSift(float * descriptors, int size);


public:
    /* Constructer and Destructer */
    ImageCoder(void);
    ImageCoder(int,int,int,int);
    ImageCoder(VlDsiftFilter*);
    ~ImageCoder(void);

    float* dsiftDescripter(Mat srcImage);
    string llcDescripter(Mat srcImage, float* codebook, int ncb, int k);
    Eigen::MatrixXf normSift(float * descriptors, int row, int col, bool normalized);

};
#endif //SIREEN_IMAGE_FEATURE_EXTRACT_H_
