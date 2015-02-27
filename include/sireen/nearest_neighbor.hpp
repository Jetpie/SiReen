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
    /*
     * Define point structure
     */
    struct point
    {
        double x,y;
        point() : x(0.),y(0.)
        {}
        point(double u, double v) : x(u),y(v)
        {}
    };

    class kdtree
    {
    public:
        kdtree();
        ~kdtree();
        void constructTree(void);
        void search(void);
    };
    class root
    {

    };
}
#endif //SIREEN_NEAREST_NEIGHBOR_H_
