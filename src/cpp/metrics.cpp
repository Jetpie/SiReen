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
    double cosine(const double* x,const double* y, const size_t dim,
                 const bool normalized)
    {
        double similarity = 0;
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
    double euclidean(const double* x,const double* y, const size_t dim,
                    const bool normalized)
    {
        if(normalized)
        {
            return (2* cosine(x,y,dim,true));
        }
        else
        {
            double dist = 0 ;
            double tmp = 0;
            for(size_t i = 0; i < dim ; ++i)
            {
                tmp = x[i] - y[i];
                dist += tmp * tmp;
            }
            return sqrt(dist);

        }
    }

}
