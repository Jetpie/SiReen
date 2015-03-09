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

using namespace std;

namespace nnse
{
    /// Define Feature structure
    struct Feature
    {
        /** feature dimension */
        int dimension;
        /** feature data */
        float* data;

    };
    /// Node definitions for kd-tree
    struct KDTreeNode
    {
        /** key index for partition */
        int index;
        /** key value for partition */
        float pivot;
        /** leaf flag */
        bool leaf;
        /** all features at current node */
        Feature* feature;
        /** number of features */
        int n;
        /** left child */
        KDTreeNode* left;
        /** right child */
        KDTreeNode* right;
    };
    class KDTree
    {
    private:
        /** kd-tree root */
        KDTreeNode root;
        void ExpandNodeSubtree(KDTreeNode*);

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
        void Build(Feature*, int);
        /**
         * build the kd-tree structure from input features
         *
         * @param features an array of features
         * @param n        number of features
         */
        KDTree* NodeInit(Feature *, int);
        void KnnBBF(void);

    };

}
#endif //SIREEN_NEAREST_NEIGHBOUR_H_
