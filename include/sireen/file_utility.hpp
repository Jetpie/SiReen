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
#include <errno.h>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

namespace futil{

    std::string file_to_string(const char *filename);
    void file_to_pointer(const char * filename, float* output, char * delim);
    void file_to_pointer(const char * filename, int* output, char * delim);
    void string_to_file(const char * filename, string input, const char * mode);
    void spliter_c(const char*str, const char delim, vector<string>& result);
    void spliter_std(const std::string &s, const char delim, std::vector<std::string> &elems);
    int get_files_in_dir(std::vector<string> &out, const string &directory);
}
#endif //SIREEN_FILE_UTILITY_H_
