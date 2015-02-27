// File: file_utility.hpp
// Description: utilities for file and string processing
// Author: Bingqing Qu
// License: GPLv3
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

    std::string FileToStr(const char *filename);
    void FileToPtr(const char * filename, float* output, char * delim);
    void FileToPtr(const char * filename, int* output, char * delim);
    void StrToFile(const char * filename, string input, const char * mode);
    void Spliter(const char*str, const char delim, vector<string>& result);
    void Split2(const std::string &s, const char delim, std::vector<std::string> &elems);
}
#endif //SIREEN_FILE_UTILITY_H_
