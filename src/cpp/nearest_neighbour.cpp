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
#include "sireen/nearest_neighbour.hpp"

namespace nnse
{
    KDTree::KDTree(const size_t d, const size_t leaf_size):
        dimension_(d),leaf_size_(leaf_size){}
    KDTree::~KDTree()
    {
        cout << "report use_count:" << this->root_.use_count() << endl;
        // this->release(this->root_);
    }

    // /**
    //  * Release all the allocated memories
    //  *
    //  * @param a tree node
    //  * @deperated because smart pointer for node class applied
    //  */
    // void
    // KDTree::release(KDTreeNode* node)
    // {
    //     if(!node)
    //         return;
    //     this->release(node->left);
    //     this->release(node->right);
    //     delete node;
    // }

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
        NodePtr building= this->init_node(features, n);
        // sanity check for initialized root
        assert(building);
        // expand
        this->expand_subtree(building);

        this->root_ = building;
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
    KDTree::NodePtr
    KDTree::init_node(Feature* features, const size_t n)
    {
        // dynamic allocate root node
        NodePtr node = std::make_shared<KDTreeNode>();
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
    KDTree::expand_subtree(NodePtr node)
    {
        // check leaf condition for stoping
        if( node->n <= this->leaf_size_)
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
    KDTree::partition(NodePtr node)
    {
        // ***1 DETERMINE THE PIVOT DIMENSION AND FEATURE***

        // variable initialization
        Feature* features = node->features;
        // sanity check for features
        assert(features);
        size_t pivot_dim = 0;
        double pivot_val, mean, var, x_diff = 0;
        double var_max = -1.0;
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
        vector<KeyValue<size_t>> order;
        for( size_t i=0; i < n; ++i)
            order.push_back(KeyValue<size_t>(i,features[i].data[pivot_dim]));

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

        //
        node->left = this->init_node(features, k + 1);
        node->right = this->init_node(features + (k + 1), (n - k - 1) );
    }
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
    KDTree::NodePtr
    KDTree::traverse_to_leaf(double* feature,NodePtr node, NodeStack& container)
    {
        if(!node)
        {
            cerr << " KDTree::traverse_to_leaf : bad node!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return NULL;

        }
        NodePtr other;
        NodePtr cur_node = node;
        double value;
        size_t dim;

        while(cur_node && !cur_node->leaf)
        {
            // partition dimension and value
            dim = cur_node->pivot_dim;
            value = cur_node->pivot_val;

            // sanity check for dimension
            assert(dim < this->dimension_);

            // go to a child and preserve the other
            if(feature[dim] <= value)
            {
                other = cur_node->right;
                cur_node = cur_node->left;
            }
            else
            {
                other = cur_node->left;
                cur_node = cur_node->right;
            }
            if(other)
                container.push(other);
        }

        return cur_node;
    }
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
    KDTree::NodePtr
    KDTree::traverse_to_leaf(double* feature,NodePtr node, NodeMinPQ& container)
    {
        if(!node)
        {
            cerr << " KDTree::traverse_to_leaf : bad node!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return NULL;

        }
        NodePtr other;
        NodePtr cur_node = node;
        double value;
        size_t dim;

        while(cur_node && !cur_node->leaf)
        {
            // partition dimension and value
            dim = cur_node->pivot_dim;
            value = cur_node->pivot_val;

            // sanity check for dimension
            assert(dim < this->dimension_);

            // go to a child and preserve the other
            if(feature[dim] <= value)
            {
                other = cur_node->right;
                cur_node = cur_node->left;
            }
            else
            {
                other = cur_node->left;
                cur_node = cur_node->right;
            }
            if(other)
                container.push(NodeBind(other,
                  abs(other->pivot_val - feature[other->pivot_dim])
                               ));
        }

        return cur_node;
    }
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
    std::vector<Feature>
    KDTree::knn_basic(double* feature, size_t k)
    {
        // best result buffer
        vector<Feature> nbrs;
        nbrs.reserve(k);
        if(!this->root_ || !feature)
        {
            cerr << " KDTree::knn_basic_opt : tree not built or invalid input!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return nbrs;
        }
        NodePtr node;
        // checklist for backtrack use
        NodeStack check_list;
        // min-priority queue to keep top k lagrest(reversed order
        // of distances). The features with largest distances will be
        // passed to returnd vector.
        FeatureMaxPQ max_pq;
        double cur_best = numeric_limits<double>::max();

        // distance butter
        double dist = 0;

        // root for handle
        check_list.push(this->root_);
        while(!check_list.empty())
        {
            // pop the element
            node = check_list.top();
            check_list.pop();

            // check if pitvot dimension comparison can possibly
            // beat current best distance
            if(!(abs(node->pivot_val - feature[node->pivot_dim]) < cur_best))
                continue;

            // find leaf and push unprocessed to stack
            node = this->traverse_to_leaf(feature,node,check_list);
            for(size_t i = 0; i < node->n; ++i)
            {

                dist = spat::euclidean(node->features[i].data,feature,
                                       this->dimension_,false);
                if(dist < cur_best)
                {
                    // maintain the bounded min priority queue
                    if(max_pq.size() == k)
                    {
                        // update current best
                        // pop the old greatest-smallest
                        max_pq.pop();
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    // the special point here is that we need to set best
                    // distance to the distance value of largest smallest
                    // feature
                    else if(max_pq.size() == k-1)
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    else
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                    }
                }
            }
        }

        // finally pass results to returned result
        const size_t detected = max_pq.size();
        for(size_t i = 0; i < detected ; ++i)
        {
            nbrs.push_back(max_pq.top().key);
            max_pq.pop();
        }
        return nbrs;
    }
    /**
     * Basic kd-tree search with optmization on comparison method.
     * The comparison of distance use a early-stop startegy if current
     * cumulative squared integral is already over the greatest-smallest
     * value in the max-priority queue.
     *
     * @param feature query feature data in array form
     * @param k       number of nearest neighbour returned
     *
     * @return
     */
    std::vector<Feature>
    KDTree::knn_basic_opt(double* feature, size_t k)
    {
        // best result buffer
        vector<Feature> nbrs;
        nbrs.reserve(k);
        if(!this->root_ || !feature)
        {
            cerr << " KDTree::knn_basic_opt : tree not built or invalid input!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return nbrs;
        }
        NodePtr node;
        // checklist for backtrack use
        NodeStack check_list;
        // min-priority queue to keep top k lagrest(reversed order
        // of distances). The features with largest distances will be
        // passed to returnd vector.
        FeatureMaxPQ max_pq;

        double cur_best = numeric_limits<double>::max();

        // distance butter
        double dist = 0;

        // root for handle
        check_list.push(this->root_);
        while(!check_list.empty())
        {
            // pop the element
            node = check_list.top();
            check_list.pop();

            // check if pitvot dimension comparison can possibly
            // beat current best distance
            if(!(abs(node->pivot_val - feature[node->pivot_dim]) < cur_best))
                continue;

            // find leaf and push unprocessed to stack
            node = this->traverse_to_leaf(feature,node,check_list);
            for(size_t i = 0; i < node->n; ++i)
            {

                if(spat::optimize_compare(node->features[i].data,feature,
                                          cur_best,this->dimension_,dist))
                {
                    // maintain the bounded min priority queue
                    if(max_pq.size() == k)
                    {

                        // pop the old greatest-smallest
                        max_pq.pop();
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    // the special point here is that we need to set best
                    // distance to the distance value of largest smallest
                    // feature
                    else if(max_pq.size() == k-1)
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    else
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                    }
                }
            }
        }

        // finally pass results to returned result
        const size_t detected = max_pq.size();
        for(size_t i = 0; i < detected ; ++i)
        {
            nbrs.push_back(max_pq.top().key);
            max_pq.pop();
        }
        return nbrs;
    }
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
    std::vector<Feature>
    KDTree::knn_bbf(double* feature, size_t k, size_t max_epoch)
    {
        // best result buffer
        vector<Feature> nbrs;
        nbrs.reserve(k);
        if(!this->root_ || !feature)
        {
            cerr << " KDTree::knn_basic_opt : tree not built or invalid input!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return nbrs;
        }
        size_t epoch = 0;
        NodePtr node;
        // checklist for backtrack use
        NodeMinPQ check_list;
        // min-priority queue to keep top k lagrest(reversed order
        // of distances). The features with largest distances will be
        // passed to returnd vector.
        FeatureMaxPQ max_pq;
        double cur_best = numeric_limits<double>::max();

        // distance butter
        double dist = 0;

        // root for handle
        check_list.push(NodeBind(this->root_,0));
        while(!check_list.empty() && epoch < max_epoch)
        {
            // pop the element
            node = check_list.top().key;
            check_list.pop();

            // find leaf and push unprocessed to stack
            node = this->traverse_to_leaf(feature,node,check_list);
            for(size_t i = 0; i < node->n; ++i)
            {

                dist = spat::euclidean(node->features[i].data,feature,
                                       this->dimension_,false);
                if(dist < cur_best)
                {
                    // maintain the bounded min priority queue
                    if(max_pq.size() == k)
                    {
                        // update current best
                        // pop the old greatest-smallest
                        max_pq.pop();
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    // the special point here is that we need to set best
                    // distance to the distance value of largest smallest
                    // feature
                    else if(max_pq.size() == k-1)
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    else
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                    }
                }
            }
            ++epoch;
        }

        // finally pass results to returned result
        const size_t detected = max_pq.size();
        for(size_t i = 0; i < detected ; ++i)
        {
            nbrs.push_back(max_pq.top().key);
            max_pq.pop();
        }
        return nbrs;
    }
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
    std::vector<Feature>
    KDTree::knn_bbf_opt(double* feature, size_t k, size_t max_epoch)
    {
        // best result buffer
        vector<Feature> nbrs;
        nbrs.reserve(k);
        if(!this->root_ || !feature)
        {
            cerr << " KDTree::knn_basic_opt : tree not built or invalid input!"
                 <<__FILE__<<","<<__LINE__ <<endl;
            return nbrs;
        }
        size_t epoch = 0;
        NodePtr node;
        // checklist for backtrack use
        NodeMinPQ check_list;
        // min-priority queue to keep top k lagrest(reversed order
        // of distances). The features with largest distances will be
        // passed to returnd vector.
        FeatureMaxPQ max_pq;

        double cur_best = numeric_limits<double>::max();

        // distance butter
        double dist = 0;

        // root for handle
        check_list.push(NodeBind(this->root_,0));
        while(!check_list.empty() && epoch < max_epoch)
        {
            // pop the element
            node = check_list.top().key;
            check_list.pop();

            // find leaf and push unprocessed to stack
            node = this->traverse_to_leaf(feature,node,check_list);
            for(size_t i = 0; i < node->n; ++i)
            {

                if(spat::optimize_compare(node->features[i].data,feature,
                                          cur_best,this->dimension_,dist))
                {
                    // maintain the bounded min priority queue
                    if(max_pq.size() == k)
                    {

                        // pop the old greatest-smallest
                        max_pq.pop();
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    // the special point here is that we need to set best
                    // distance to the distance value of largest smallest
                    // feature
                    else if(max_pq.size() == k-1)
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                        cur_best = max_pq.top().value;
                    }
                    else
                    {
                        max_pq.push(KeyValue<Feature>(node->features[i], dist));
                    }
                }
            }
            ++epoch;
        }

        // finally pass results to returned result
        const size_t detected = max_pq.size();
        for(size_t i = 0; i < detected ; ++i)
        {
            nbrs.push_back(max_pq.top().key);
            max_pq.pop();
        }
        return nbrs;
    }



}
