#pragma once

#include <stdio.h>
#include <dirent.h>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <string.h>
#include <queue>
// opencv header
#include <opencv2/opencv.hpp>
#include <ctype.h>

// Eigen Linear Algebra
//#include <Eigen/Core>
#include <Eigen/Dense>

using namespace cv;
using namespace std;
using namespace Eigen;

extern "C" {
#include <vl/generic.h>
#include <vl/stringop.h>
#include <vl/pgm.h>
#include <vl/sift.h>
#include <vl/getopt_long.h>
#include <vl/dsift.h>
#include <vl/ikmeans.h>
};

class ImageCoder
{

private:
    /* stand image frame size */
    unsigned int stdWidth;
    unsigned int stdHeight;
    /* sampling step  */
    unsigned int step;
    /* bin size */
    unsigned int binSize;
    /* dsift filter */
    VlDsiftFilter* dsiftFilter = NULL;

    void setParams(int stdWidth, int stdHeight, int step, int binSize);
    float* normalizeSift(float * descripters, int size);


public:
    /* Constructer and Destructer */
    ImageCoder(void);
    ImageCoder(int,int,int,int);
    ImageCoder(VlDsiftFilter*);
    ~ImageCoder(void);

    float* dsiftDescripter(Mat srcImage);
    string llcDescripter(Mat srcImage, float* codebook, int ncb, int k);
    string llcTest(Mat srcImage, float* codebook, int ncv, int k);


};
