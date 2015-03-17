// Optimized spatial distance metrics
//
// @author: Bingqing Qu
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory
#ifndef SIREEN_METRICS_H_
#define SIREEN_METRICS_H_

#include <string.h>
#include <math.h>
//#define NDEBUG
using namespace std;

// spatial
namespace spat
{

    // should be very clear
    double cosine(const double* x,const double* y, const size_t dim,
                 const bool normalized=false);
    // use a trick for euclidean(x,y) = 2 - cosine_similarity if
    // input vectors are normalized
    double euclidean(const double* x,const double* y, const size_t dim,
                    const bool normalized = false);
    bool optimize_compare(const double* x,const double* y, const double target,
                          const size_t dim, double &dist);
}
#endif
