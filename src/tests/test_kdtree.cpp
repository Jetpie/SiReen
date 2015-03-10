#include "sireen/nearest_neighbour.hpp"

using namespace std;

int main()
{
    nnse::KDTree kdtree;
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
