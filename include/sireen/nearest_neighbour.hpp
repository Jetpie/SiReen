// Optimized functions for kd-tree construction and searching.
//
// @author: Bingqing Qu
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory
#ifndef SIREEN_NEAREST_NEIGHBOUR_H_
#define SIREEN_NEAREST_NEIGHBOUR_H_

#include <vector>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <assert.h>
//#define NDEBUG
using namespace std;

// nnse is short for "nearest neighbour search"
namespace nnse
{
    /// key value pair. Not use the std::pair because there is default
    /// "<" operator overloading for it and it is too heavy for operation.
    struct KeyValue
    {
        size_t key;
        float value;
        KeyValue(const size_t k, const float v) : key(k), value(v){}
    };
    // Operator overloading
    inline bool operator<(const KeyValue& lhs, const KeyValue& rhs)
    {return lhs.value < rhs.value; }

    /// Define Feature structure
    struct Feature
    {
        /** feature dimension */
        size_t dimension;
        /** feature data */
        float* data;

    };
    /// Node definitions for kd-tree
    struct KDTreeNode
    {
        /** feature dimension for partition */
        int pivot_dim;
        /** key value for partition */
        float pivot_val;
        /** leaf flag */
        bool leaf;
        /** all features at current node */
        Feature* features;
        /** number of features */
        size_t n;
        /** left child */
        KDTreeNode* left;
        /** right child */
        KDTreeNode* right;
    };

    ///
    /// Todo: add descriptions for KDTree
    ///
    ///
    ///
    class KDTree
    {
    private:
        /** kd-tree root */
        KDTreeNode* root_;

    public:
        /** Constructor */
        KDTree();
        /** Destructor */
        ~KDTree();
        /**
         * build the kd-tree structure from input features
         *
         * @param features an array of features
         * @param n        number of features
         *
         */
        void Build(Feature*, const size_t);
        /**
         * initialization of a subtree
         *
         * @param features an array of features
         * @param n        number of features
         */
        KDTreeNode* InitNode(Feature *, const size_t);
        /**
         * expand kd-tree after root node is initialized
         *
         * @param node current kd-tree node
         */
        void Expand(KDTreeNode*);
        /**
         * Partition features on the current node. Two parts:
         *
         * 1.Determine pivot feature to split to patition the currrent
         * node's features. First, find the dimension with grestest
         * variance. Second, find the feature with the median of value on
         * that dimension.
         *
         * 2.Partition the features by the pivot. This is done by firstly,
         * get the order vector and secondly, re-order the features by that
         * order vector.
         *
         * @param node the current node
         *
         */
        void Partition(KDTreeNode*);


    };

}
#endif //SIREEN_NEAREST_NEIGHBOUR_H_
