// Optimized c++ general construction and searching functions for
// KD-Tree.
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
#include <iomanip>
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

    // compute median position of a group of index
    inline size_t get_median_index(const size_t x)
    {return ( x / 2); }

    /// Define Feature structure
    struct Feature
    {
        /** feature dimension */
        size_t dimension;
        /** feature data */
        float* data;
        Feature(){}
        Feature(float* data, const size_t dim) : dimension(dim), data(data){}

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
        // DEBUG
        void print()
        {
            cout << "**********" << endl;
            cout << "pivot_dim:" << pivot_dim << endl;
            cout << "pivot_val:" << pivot_val << endl;
            cout << "leaf:" << leaf << endl;
            cout << "n:" << n << endl;
            size_t root_idx = get_median_index(n);
            cout << "root index:" << root_idx <<endl;
            cout << "feature:" <<"(" <<features[root_idx].data[0] <<","
                 << features[root_idx].data[1] << ")"<<endl;
        };
    };

    ///
    /// Todo: add descriptions for KDTree
    ///
    ///
    ///
    class KDTree
    {
    private:
        /** kd-tree root node */
        KDTreeNode* root_;
        /** kd-tree feature dimension */
        size_t dimension_;

        /**
         * initialization of a subtree
         *
         * @param features an array of features
         * @param n        number of features
         */
        KDTreeNode* init_node(Feature *, const size_t);
        /**
         * expand kd-tree after root node is initialized
         *
         * @param node current kd-tree node
         */
        void expand_subtree(KDTreeNode*);
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
         * The result of the partition is the Feature array is re-ordered
         * and a new node contains left child of  features[0:k] and the
         * right child contains the right child of features[k+1:k+1+n],
         * where n is the length of right child. The current root node is
         * features[k]
         *
         * @param node the current node
         *
         */
        void partition(KDTreeNode*);
        void traverse_to_leaf(KDTreeNode*);

    public:
        /** Constructor */
        KDTree(const size_t);
        /** Destructor */
        ~KDTree();
        /**
         * Release all the allocated memories
         *
         * @param a tree node
         */
        void release(KDTreeNode* );
        /**
         * build the kd-tree structure from input features
         *
         * @param features an array of features
         * @param n        number of features
         *
         */
        void build(Feature*, const size_t);
        /**
         *
         *
         */
        void knn_search_bbf(float* );
        void knn_search_brute_force(float*);
        void knn_search_kdtree(float* );




        // DEBUG
        void print_tree()
        {

            this->print_node(this->root_);

        };
        void print_node(KDTreeNode* node,int indent=0)
        {
            if(node != NULL)
            {
                if(node->left)
                    print_node(node->left,indent+8);

                if(indent)
                {
                    cout << setw(indent) << ' ';
                }
                size_t k = get_median_index(node->n);
                cout <<"(" << node->features[k].data[0] << ","
                     << node->features[k].data[1] << ")\n";

                if(node->right)
                    print_node(node->right,indent+8);


            }
        }

    };

}
#endif //SIREEN_NEAREST_NEIGHBOUR_H_
