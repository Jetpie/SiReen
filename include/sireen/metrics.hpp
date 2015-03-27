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
    /**
     * Compute cosine similarity between features. If the input vector
     * is marked as normalized, the simplified computation will be
     * applied.
     *
     * @param x         vector x
     * @param y         vector y
     * @param dim       feature dimension
     * @param noralized flag to determine the nomalization state of
     *                  feature vecter.
     *
     * @return cosine distance of features
     */
    template <class T> T
    cosine(const T* x, const T* y, const size_t dim,
                 const bool normalized)
    {
        T similarity = 0;
        if(normalized)
        {
            for(size_t i = 0; i < dim ; ++i)
                similarity += x[i] * y[i];
        }
        else
        {
            double x_base = 0, y_base = 0;
            for(size_t i = 0; i < dim ; ++i)
            {
                x_base += x[i] * x[i];
                y_base += y[i] * y[i];
                similarity += x[i] * y[i];
            }
            if(similarity)
                similarity /= sqrt(x_base) * sqrt(y_base);
        }

        return (1-similarity);
    }
    // use a trick for euclidean(x,y) = 2 - cosine_similarity if
    // input vectors are normalized
    /**
     * Compute euclidean similarity between features. If the input vector
     * is marked as normalized, use a trick for:
     * euclidean(x,y) = 2 - cosine_similarity
     *
     * @param x         vector x
     * @param y         vector y
     * @param dim       feature dimension
     * @param noralized flag to determine the nomalization state of
     *                  feature vecter.
     *
     * @return euclidean distance of features
     */
    template <class T> T
    euclidean(const T* x,const T* y, const size_t dim,
                    const bool normalized)
    {
        if(normalized)
        {
            return (2* cosine(x,y,dim,true));
        }
        else
        {
            T dist = 0 ;
            T tmp = 0;
            for(size_t i = 0; i < dim ; ++i)
            {
                tmp = x[i] - y[i];
                dist += tmp * tmp;
            }
            return sqrt(dist);

        }
    }

    /**
     * compute euclidean distance in terms of using optimized comparison
     * startegy. If the cumulative squared distance has beyond the
     * current best, it will stop and return an incomplete result.
     * Else if the final result can beat current best, return true
     * with the complete distance value.
     *
     * @param x      vector x
     * @param y      vector y
     * @param dim    feature dimension
     * @param target current best value
     * @param dist   distance reference
     *
     */
    template <class T> bool
    optimize_compare(const T* x,const T* y, const T target,
                       const size_t dim, T &dist)
    {
        // flush
        dist = 0.0;
        T tmp = 0;
        for(size_t i = 0; i < dim ; ++i)
        {
            tmp = x[i] - y[i];
            dist += tmp * tmp;
            if(dist >= target)
                return false;
        }
        return true;
    }
}
#endif
