#ifndef SIREEN_FILE_UTILITY_H_
#define SIREEN_FILE_UTILITY_H_

#include <vector>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <string.h>
#include <cerrno>
#include <sstream>

using namespace std;

namespace futil{

    std::string file2str(const char *filename);
    void file2ptr(const char * filename, float* output, char * delim);
    void file2ptr(const char * filename, int* output, char * delim);
    void str2file(const char * filename, string input, const char * mode);
    void spliter(const char*str, char c, vector<string>& split_vect);
    void split2(const std::string &s, char delim, std::vector<std::string> &elems);
}
#endif //SIREEN_FILE_UTILITY_H_
