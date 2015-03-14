// Optimized spatial distance metrics
//
// @author: Bingqing Qu
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory
#include "sireen/metrics.hpp"
namespace spat
{
    // should be very clear
    float cosine(const float* x,const float* y, const size_t dim)
    {
        float similarity = 0;
        for(size_t i = 0; i < dim ; ++i)
        {
            similarity = x[i] * y[i];
        }
        return (1-similarity);
    }
    // use a trick for euclidean(x,y) = 2 - cosine_similarity if
    // input vectors are normalized
    float euclidean(const float* x,const float* y, const size_t dim,
                    const bool normalized)
    {
        if(normalized)
        {
            return (1 + cosine(x,y,dim));
        }
        else
        {
            float dist = 0 ;
            float tmp = 0;
            for(size_t i = 0; i < dim ; ++i)
            {
                tmp = x[i] - y[i];
                dist += tmp * tmp;
            }
            return sqrt(dist);

        }
    }

}
