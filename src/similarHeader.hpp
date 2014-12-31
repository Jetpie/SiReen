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

/*typedef struct productstruct
{
    int cat;
    char imgurl[128];
    int merchant;
}prod;*/

class similarKeyFunction
{
public:
    similarKeyFunction(void);
    ~similarKeyFunction(void);

    VlDsiftFilter* GetDsift(IplImage* SrcImage, int& size, int binSize);
    string llcCompute(float *Descriptors, float *B, int ncb, int size);

};

class similarBasicFunction
{
public:
    similarBasicFunction(void);
    ~similarBasicFunction(void);

    void write_TXT_file(const char * pFileName, float * input, int inputSize);
    void write_TXT_file(const char * pFileName, string input);
    //void write_result_file(const char * pFileName, vector<string> items, float * input, int inputSize);
    void read_TXT_file(const char * pFileName, float *output);
    char *getUrl(char *imgName);
    float *normalizedSift(float *Descriptors, int size);
    void split_string(const char*str, char c, vector<string>& split_vect);

};


