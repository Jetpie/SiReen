#include "sireen/nearest_neighbour.hpp"

namespace nnse
{
    KDTree::KDTree(){}

    KDTree::~KDTree(){}

    /**
     * build the kd-tree structure from input features
     *
     * @param features an array of features
     * @param n        number of features
     *
     */
    void
    KDTree::Build(Feature* features, const size_t n)
    {
        // check input
        if( !features )
        {
            cerr << "Error: KDTree::Build(): input error!" << endl;
        }
        // init
        this->root_ = InitNode(features, n);

        // expand
        Expand(this->root_);
    }

    /**
     * initialization of kd-tree from the feature database
     *
     * @param features an array of features
     * @param n        number of features
     */
    KDTreeNode*
    KDTree::InitNode(Feature* features, const size_t n)
    {
        // dynamic allocate root node
        KDTreeNode* node = new KDTreeNode;
        // initialize index, features and n params for root
        node->pivot_dim = -1;
        node->features = features;
        node->n = n;

        return node;

    }

    /**
     * expand kd-tree after root node is initialized
     *
     * @param node current kd-tree node
     */
    void
    KDTree::Expand(KDTreeNode* node)
    {
        // check leaf condition for stoping
        if( node->n==1 || node->n ==0)
        {
            node->leaf = true;
            return;
        }
        // the following parts should be very clear
        Partition(node);
        if(node->left)
            Expand(node->left);
        if(node->right)
            Expand(node->right);
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
     * @param node the current node
     *
     */
    void
    KDTree::Partition(KDTreeNode* node)
    {
        // ***1 DETERMINE THE PIVOT DIMENSION AND FEATURE***

        // variable initialization
        Feature* features = node->features;
        size_t pivot_dim = 0;
        float pivot_val, mean, var, x_diff = 0;
        float var_max = -1.0;
        size_t n = node->n;
        size_t dim = features[0].dimension;

        // search for the feature dimension with greatest variance
        for(size_t i = 0; i < dim; ++i)
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
        const size_t k = (n-1) / 2;

        // the nth_element can do what we want for partition. For given
        // begin and end iterators, and a k iterator, there is no
        // element greater than *k at and likewise on the right.
        std::nth_element(order.begin(), order.begin()+k, order.end());

        // assign pivot dimension and value for current ndoe
        pivot_val = order[k].value;
        node->pivot_dim = pivot_dim;
        node->pivot_val = pivot_val;

        // ***2 PARTIOTION THE NODE***

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
        if(k + 1 == n)
        {
            node->leaf=true;
            return;
        }
        //
        node->left = InitNode(features, k+1);
        node->right = InitNode(features+k, n-k+1);

    }

}
