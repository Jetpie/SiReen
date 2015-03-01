// File: nearest_neighbor.hpp
// Description: data structures and algorithms for nearest neighbor
//              search
// Author: Bingqing Qu
// License: GPLv3
#ifndef SIREEN_NEAREST_NEIGHBOR_H_
#define SIREEN_NEAREST_NEIGHBOR_H_

#include <vector>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <string.h>
#include <cerrno>
#include <sstream>

using namespace std;

namespace nnse
{
    /// Define point structure
    struct Point
    {
        float x,y;
        Point() : x(0.),y(0.)
        {}
        Point(float u, float v) : x(u),y(v)
        {}
    };

    class KDTree
    {
    public:
        KDTree();
        ~KDTree();
        void ConstructTree(void);
        void Search(void);
    };
    class Root
    {

    };
}
#endif //SIREEN_NEAREST_NEIGHBOR_H_
