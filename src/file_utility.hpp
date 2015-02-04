#pragma once

#include <vector>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <time.h>
#include <string.h>
#include <cerrno>
using namespace std;

class FileUtil
{


public:

    FileUtil(void);
    ~FileUtil(void);

    void write_TXT_file(const char * pFileName, string input);
    void file2ptr(const char * filename, float* output, char * delim);
    void file2ptr(const char * filename, int* output, char * delim);
    string file2str(const char *filename);
    void split_string(const char*str, char c, vector<string>& split_vect);

};
