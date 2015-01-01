#pragma once

#include <stdio.h>
#include <dirent.h>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>

#include <opencv2/opencv.hpp>
#include <string.h>
#include <ctype.h>


using namespace cv;
using namespace std;

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
    unsigned int stdWidth;
    unsigned int stdHeight;
    unsigned int step;

public:
    /* Constructer and Deconstructer */
    ImageCoder(void);
    ImageCoder(int,int,int);
    ~ImageCoder(void);
    VlDsiftFilter* dsiftDescripter(Mat srcImage, int& size, intt binSize);
    string llcDescripter(float* dsiftDescripter, float* B int ncb, int size);
    float* normalizedSift(float * descripters, int size);

};
