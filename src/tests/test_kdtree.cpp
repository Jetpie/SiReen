#include "sireen/nearest_neighbour.hpp"

using namespace std;
using namespace nnse;
int main()
{
    Feature* features = new Feature[6];
    float p[2] = {2.0,3.0};
    features[0] = Feature(p,2);
    float p1[2] = {5.0,4.0};
    features[1] = Feature(p1,2);
    float p2[2] = {9.0,6.0};
    features[2] = Feature(p2,2);
    float p3[2] = {4.0,7.0};
    features[3] = Feature(p3,2);
    float p4[2] = {8.0,1.0};
    features[4] = Feature(p4,2);
    float p5[2] = {7.0,2.0};
    features[5] = Feature(p5,2);

    for(size_t i = 0; i < 6 ;++i)
    {
        cout << "[" << features[i].data[0] <<"," <<features[i].data[1] << "]";
    }
    cout << endl;
    const size_t dim = 2;
    nnse::KDTree* kdtree = new nnse::KDTree(dim);
    kdtree->build(features,6);
    for(size_t i = 0; i < 6 ;++i)
    {
        cout << "[" <<features[i].data[0] <<"," <<features[i].data[1] << "]";
    }
    cout << endl;
    kdtree->print_tree();


    delete [] features;
    cout << "***std::nth_element Test***" << endl;
    float a[] = {7.0,3.0,5.0,4.0,2.0,6.0,1,0};
    vector<nnse::KeyValue> v;
    for(size_t i = 0; i< 7 ;++i)
    {
        v.push_back(nnse::KeyValue(i,a[i]));
    }
    std::nth_element(v.begin(), v.begin() + v.size()/2,v.end());
    for(size_t i = 0; i< 7 ;++i)
    {
        cout << v[i].value << " ";
    }
    cout << endl;
    for(size_t i = 0; i< 7 ;++i)
    {
        cout << v[i].key << " ";
    }
    cout << endl;
    cout << "median: " << v[v.size()/2].value << endl;
    cout << "***std::nth_element Test***" << endl;

    float tmp;
    size_t d,i_src,cnt=0;
    for(size_t df=0; df<7;++df)
    {
        i_src = v[df].key;
        if(i_src == df)
            continue;
        d = df;
        tmp = a[d];
        do
        {
            a[d]  =a[i_src];
            v[d].key = d;

            d = i_src;
            i_src = v[i_src].key;
            cnt++;
            for(size_t i=0; i< 7; ++i)
            {
                cout << a[i] << " ";
            }
            cout << endl;

        }
        while(i_src != df);
        a[d]  = tmp;
        v[d].key  = d;

    }
    for(size_t i=0; i< 7; ++i)
    {
        cout << a[i] << " ";
    }
    cout << endl;
    cout << "number of swap:"  << cnt << endl;

}
