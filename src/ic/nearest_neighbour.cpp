// Optimized c++ general construction and searching functions for
// KD-Tree.
//
// @author: Bingqing Qu
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory
#include "sireen/nearest_neighbour.hpp"

namespace nnse
{
    KDTree::KDTree(const size_t dimension): dimension_(dimension){}
    KDTree::~KDTree()
    {
        this->release(this->root_);
    }
    /**
     * Release all the allocated memories
     */
    void
    KDTree::release(KDTreeNode* node)
    {
        if(!node)
            return;
        this->release(node->left);
        this->release(node->right);
        delete node;
    }
    /**
     * build the kd-tree structure from input features
     *
     * @param features an array of features
     * @param n        number of features
     *
     */
    void
    KDTree::build(Feature* features, const size_t n)
    {
        // check inputs
        if(!features || n <= 0)
        {
            cerr << " KDTree::build : Error input, no features or n <= 0"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return;
        }
        // init
        this->root_ = this->init_node(features, n);
        // sanity check for initialized root
        assert(this->root_);
        // expand
        this->expand_subtree(this->root_);

    }

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
    KDTreeNode*
    KDTree::init_node(Feature* features, const size_t n)
    {
        // dynamic allocate root node
        KDTreeNode* node = new KDTreeNode;
        if(!node)
        {
            cerr << " KDTree::init_node : memory allocation to KDTreeNode fail!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return NULL;
        }
        // initialize index, features and n params for root
        node->pivot_dim = -1;
        // non-negative feature assumption
        node->pivot_val = -1.0;
        node->features = features;
        node->leaf = false;
        node->n = n;
        // initialize to NULL
        node->left = NULL;
        node->right = NULL;

        return node;

    }

    /**
     * Expand the subtree. This should be called after a kd-tree
     * node is initialized. If the current node is not a leaf, a
     * partition is applied on features. Then, it expand the two
     * children recursively.
     *
     * @param node current kd-tree node
     */
    void
    KDTree::expand_subtree(KDTreeNode* node)
    {
        // check leaf condition for stoping
        if( node->n == 1 || node->n == 0)
        {
            node->leaf = true;
            return;
        }
        // the following parts should be very clear
        this->partition(node);

        if(node->left)
            this->expand_subtree(node->left);
        if(node->right)
            this->expand_subtree(node->right);
    }

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
    void
    KDTree::partition(KDTreeNode* node)
    {
        // ***1 DETERMINE THE PIVOT DIMENSION AND FEATURE***

        // variable initialization
        Feature* features = node->features;
        // sanity check for features
        assert(features);
        size_t pivot_dim = 0;
        float pivot_val, mean, var, x_diff = 0;
        float var_max = -1.0;
        size_t n = node->n;

        // search for the feature dimension with greatest variance
        for(size_t i = 0; i < this->dimension_; ++i)
        {
            // flush mean and varaiance value
            mean = var = 0;

            // integral and divide to get mean
            for(size_t j = 0; j < n; ++j)
                mean += features[j].data[i];
            mean /= n;

            // integral and divide to get variance
            for(size_t j = 0; j < n; ++j)
            {
                x_diff = features[j].data[i] - mean;
                var += x_diff * x_diff;
            }
            // use trick here for comparison
            // var /= n;

            // update current best dimension
            if( var > var_max)
            {
                pivot_dim = i;
                var_max = var;
            }

        }

        // search for the median value for partition
        // use the user-defined structure KeyValue and std:nth_element
        // with overloaded operator "<" to fast find median
        vector<KeyValue> order;
        for( size_t i=0; i < n; ++i)
            order.push_back(KeyValue(i,features[i].data[pivot_dim]));

        // get the median index number
        const size_t k = get_median_index(n);

        // the nth_element can do what we want for partition. For given
        // begin and end iterators, and a k iterator, there is no
        // element greater than *k at left and likewise on the right.
        std::nth_element(order.begin(), order.begin()+k, order.end());

        // assign pivot dimension and value for current ndoe
        pivot_val = order[k].value;
        node->pivot_dim = pivot_dim;
        node->pivot_val = pivot_val;

        // ***2 PARTIOTION THE NODE BY PIVOT***

        // apply a re-order iterable algorithm(in-place) here.
        // the algorithm use a "permute-from" mind to do a permute-cycle
        // at each loop.
        Feature tmp;
        size_t from, to;
        for(size_t cur=0; cur < n; ++cur)
        {
            // check if the current element needs to be permuted
            from = order[cur].key;
            assert(from < n);
            // the element is already in the correct place
            if(from == cur)
                continue;
            // initialize to position
            to = cur;
            tmp = features[to];
            do
            {
                // assign the value on order
                features[to] = features[from];
                // assign the order to show it is done
                order[to].key = to;

                to = from;
                from = order[from].key;
                assert(from!=to);
            }
            while(from != cur);
            // finally, close the permutation cycle
            features[to] = tmp;
            order[to].key = to;
        }

        // if all features fall on same side, make node as a leaf
        // generally, under this condition, k = 1 and n = 2?
        // TODO: prove the above assumption
        if(k + 1 == n)
        {
            node->leaf = true;
            return;
        }
        cout << "k:" << k << endl;
        //
        node->left = this->init_node(features, k + 1);
        node->right = this->init_node(features + (k + 1), (n - k - 1) );
    }
    /**
     * Traverse a kd-tree to a leaf node. Path decision are made
     * by comparision of values between the input feature and node
     * on the node's partition dimension.
     *
     * @param feature a input feature
     * @param node a start node
     *
     * @return a leaf node with node.leaf=true
     */
    KDTreeNode*
    KDTree::traverse_to_leaf(Feature feature, KDTreeNode* node,
                             stack<KDTreeNode*> &node_stack)
    {
        if(!node)
        {
            cerr << " KDTree::traverse_to_leaf : bad node!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return NULL;

        }
        KDTreeNode* cur_node = node;
        float value;
        size_t dim;


        while(cur_node && !cur_node->leaf)
        {
            // partition dimension and value
            dim = cur_node->pivot_dim;
            value = cur_node->pivot_val;

            // sanity check for dimension
            assert(dim < this->dimension_);

            // go left child
            if(feature.data[dim] <= value)
            {
                node_stack.push(cur_node->right);
                cur_node = cur_node->left;
            }
            else
            {
                node_stack.push(cur_node->left);
                cur_node = cur_node->right;
            }
        }

        return cur_node;
    }

    /**
     * Basic k-nearest-neighbour search method use for kd-tree.
     * First, traverse from root node to a leaf node and. Second,
     * backtrack to search for better node
     *
     * @param feature query Feature
     *
     * @return
     */
    Feature
    KDTree::knn_search_basic(Feature feature)
    {
        KDTreeNode* node;
        // checklist for backtrack use
        stack<KDTreeNode*> check_list;


        while(!check_list.empty())
        {
            node = check_list.top();
            check_list.pop();
            // find leaf and push unprocessed to stack
            node = this->traverse_to_leaf(feature,this->root_,check_list);

            // TODO: update best
            for(size_t i = 0; i < node->n; ++i)
            {

            }
        }

    }

}
