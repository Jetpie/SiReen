// Optimized c++ general construction and searching functions for
// KD-Tree. This implementation has following features:
//
// 1. Fixed memory usage for constucting kd-tree with features pointers.
// 2. Optimized distance comparison for ordinary kd-tree search.
//    The speed improved from ~0.13s to ~0.08s for 500,000 data (500D)
// 3. Best-Bin-Fisrt search method provide an approximate nearest
//    neighbour search. The max-epoch parameter can control the precision
//    as well as the time performance.
//
// @author: Bingqing Qu
// @version 0.1.0
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory
#ifndef SIREEN_NEAREST_NEIGHBOUR_H_
#define SIREEN_NEAREST_NEIGHBOUR_H_

// STL
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <stack>
#include <queue>
#include <limits>
#include <memory>

#include <string.h>
#include <assert.h>
#include <math.h>

#include "sireen/metrics.hpp"
#define NDEBUG
using namespace std;

// nnse is short for "nearest neighbour search"
namespace nnse
{
    /// key value pair. Not use the std::pair because there is default
    /// "<" operator overloading for it and it is too heavy for operation.
    template <class T, class V = double>
    struct KeyValue
    {
        T key;
        V value;
        KeyValue(const T k, const V v) : key(k), value(v){}
    };
    // Operator overloading
    template <class T>
    inline bool operator<(const KeyValue<T>& lhs, const KeyValue<T>& rhs)
    {return lhs.value < rhs.value; }
    template <class T>
    inline bool operator>(const KeyValue<T>& lhs, const KeyValue<T>& rhs)
    {return lhs.value > rhs.value; }

    // compute median position of a group of index
    inline size_t get_median_index(const size_t x)
    {return ( (x - 1) / 2); }

    // epic safe absolute operation on all types
    template<class T>
    inline T abs(const T x){return (x < 0 ? -x : x);}

    /// Define Feature structure
    struct Feature
    {
        /** feature data */
        double* data;
        /** feature dimension */
        size_t dimension;
        /** feature index */
        size_t index;
        Feature(double* data, const size_t dim, const size_t i) :
            data(data) , dimension(dim), index(i) {}
        Feature() : data(),dimension(){}
        Feature& operator=(Feature other)
        {
            std::swap(data,other.data);
            std::swap(dimension,other.dimension);
            std::swap(index,other.index);
            return *this;
        }
    };

    /// Node definitions for kd-tree
    struct KDTreeNode
    {
        /** feature dimension for partition */
        int pivot_dim;
        /** key value for partition */
        double pivot_val;
        /** leaf flag */
        bool leaf;
        /** all features at current node */
        Feature* features;
        /** number of features */
        size_t n;
        /** left child */
        shared_ptr<KDTreeNode> left;
        /** right child */
        shared_ptr<KDTreeNode> right;

    };

    ///
    /// KDTree data structure for fast nearest feature searching.
    /// Parameters can be adjust are feature dimension and number of
    /// features in leaf node.
    ///
    /// Usage:
    ///     KDTree kdtree(500,30); // 30 is default value for leaf size
    ///     // features is pointer to Feature array
    ///     // 300 is number of features to build the kd-tree
    ///     kdtree.build(features, 300);
    ///     // 5 is to get top 5 closest features
    ///     kdtree.knn_basic(feature, 5);
    class KDTree
    {
    private:
        // smart pointer for safty. I am still exploring the security of
        // such using.
        typedef shared_ptr<KDTreeNode> NodePtr;
        // typedef to avoid ugly long declaration
        typedef stack<NodePtr> NodeStack;
        typedef KeyValue<NodePtr> NodeBind;
        typedef priority_queue<NodeBind, vector<NodeBind>, greater<NodeBind> > NodeMinPQ;
        typedef KeyValue<Feature> FeatureBind;
        typedef priority_queue<FeatureBind, vector<FeatureBind> > FeatureMaxPQ;
        /** kd-tree root node */
        NodePtr root_;
        /** kd-tree feature dimension */
        size_t dimension_;
        /**
         * kd-tree leaf node number of points
         * Default leaf size is 30. Larger leaf size can achieve a
         * better search result but more closer to brute-force search,
         * i.e. bad searching time.
         */
        size_t leaf_size_;

        /**
         * Initialization of a kd-tree node, this will set initial position
         * of feature pointer, the number of feature should be taken and
         * a default value for patition dimension
         *
         * @param features an array of features
         * @param n        number of features
         *
         * @return initialized kd-tree node, furthor expand should be
         *         followed
         */
        NodePtr init_node(Feature *, const size_t);
        /**
         * Expand the subtree. This should be called after a kd-tree
         * node is initialized. If the current node is not a leaf, a
         * partition is applied on features. Then, it expand the two
         * children recursively.
         *
         * @param node current kd-tree node
         */
        void expand_subtree(NodePtr);
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
        void partition(NodePtr);
        /**
         * Traverse a kd-tree to a leaf node. Path decision are made
         * by comparision of values between the input feature and node
         * on the node's partition dimension. The backtrack path is
         * recorded by a std::stack
         *
         * @param feature a input feature
         * @param node a start node
         *
         * @return a leaf node with node.leaf=true
         */
        NodePtr traverse_to_leaf(double*, NodePtr, NodeStack&);
        /**
         * Traverse a kd-tree to a leaf node. Path decision are made
         * by comparision of values between the input feature and node
         * on the node's partition dimension. The backtrack path is
         * recorded by a std::priority_queue
         *
         * @param feature a input feature
         * @param node a start node
         *
         * @return a leaf node with node.leaf=true
         */
        NodePtr traverse_to_leaf(double*, NodePtr, NodeMinPQ&);

    public:
        /** Constructor */
        KDTree(const size_t, const size_t leaf_size = 30);
        /** Destructor */
        ~KDTree();
        /**
         * build the kd-tree structure from input features. The order is
         * building the node for indexing first and point the root node
         * to that built node. This is best design for multi-threading
         * building
         *
         * @param features an array of features
         * @param n        number of features
         *
         */
        void build(Feature*, const size_t);
        /**
         * Basic k-nearest-neighbour search method use for kd-tree.
         * First, traverse from root node to a leaf node and. Second,
         * backtrack to search for better node
         *
         * @param feature query feature data in array form
         * @param k       number of nearest neighbour returned
         *
         * @return
         */
        std::vector<Feature> knn_basic(double*, size_t);
        /**
         * Basic kd-tree search with optmization on comparison method.
         * The comparison of distance use an early-stop strategy if current
         * cumulative squared integral is already over the greatest-smallest
         * value in the max-priority queue.
         *
         * @param feature query feature data in array form
         * @param k       number of nearest neighbour returned
         *
         * @return
         */
        std::vector<Feature> knn_basic_opt(double*, size_t);
        /**
         * Search for approximate k nearest neighbours using the
         * Best Bin First approach.
         *
         * @param feature    query feture data in array form
         * @param k          number of nearest neighbour returned
         * @param max_epoch  maximum of epoch of search
         *
         * @return
         */
        std::vector<Feature> knn_bbf(double*, size_t, size_t);
        /**
         * Search for approximate k nearest neighbours using the
         * Best Bin First approach. Distance comparison applied an
         * early-stop strategy.
         *
         * @param feature    query feture data in array form
         * @param k          number of nearest neighbour returned
         * @param max_epoch  maximum of epoch of search
         *
         * @return
         */
        std::vector<Feature> knn_bbf_opt(double*, size_t, size_t);

        // DEBUG
        // pre-order to print the tree node
        void print_tree()
        {
            this->print_node(this->root_);
        };
        void print_node(NodePtr node,int indent=0)
        {
            if(node != NULL)
            {
                if(node->left)
                    print_node(node->left,indent+8);

                if(indent)
                {
                    cout << setw(indent) << ' ';
                }
                cout <<"(" << node->pivot_dim << ","
                     << node->pivot_val <<","
                     << node->n << ")\n";

                if(node->right)
                    print_node(node->right,indent+8);


            }
        }

    };

}
#endif //SIREEN_NEAREST_NEIGHBOUR_H_
