#include "sireen/nearest_neighbour.hpp"
#include "sireen/metrics.hpp"
#include "sireen/file_utility.hpp"
#include <ctime>
using namespace std;
using namespace nnse;


int main()
{

    Feature* features = new Feature[6];
    double p[2] = {2.0,3.0};
    features[0] = {p,2,0};
    double p1[2] = {5.0,4.0};
    features[1] = {p1,2,1};
    double p2[2] = {9.0,6.0};
    features[2] = {p2,2,2};
    double p3[2] = {4.0,7.0};
    features[3] = {p3,2,3};
    double p4[2] = {8.0,1.0};
    features[4] = {p4,2,4};
    double p5[2] = {7.0,2.0};
    features[5] = {p5, 2,5};


    for(size_t i = 0; i < 6 ;++i)
    {
        cout << "[" << features[i].data[0] <<"," <<features[i].data[1] << "]";
    }
    cout << endl;

    const size_t dim = 2;
    nnse::KDTree kdtree(dim,1);
    kdtree.build(features,6);
    for(size_t i = 0; i < 6 ;++i)
    {
        cout << "[" <<features[i].data[0] <<"," <<features[i].data[1] << "]";
    }
    cout << endl;
    kdtree.print_tree();

    cout << "euclidean test: " <<  spat::euclidean(p2,p1,2) << endl;
    cout << "cosine test: " <<  spat::cosine(p2,p1,2) << endl;
    double q[2] = {6.9,4.0};
    std::vector<Feature> nbrs = kdtree.knn_basic(q,6);
    size_t nnb = nbrs.size();
    cout <<"nnb" << nnb<<endl;
    for(size_t i = 0;i < nnb ; ++i)
    {
        cout << "[" <<nbrs[nnb-i-1].data[0] <<"," <<nbrs[nnb-i-1].data[1] << "]" << endl;
    }


    delete [] features;

    std::priority_queue<int,std::vector<int> ,greater<int> > pq;
    pq.push(1);
    pq.push(5);
    pq.push(3);
    pq.push(2);
    cout << pq.top() << endl;
    pq.push(2.5);
    cout << pq.top() << endl;
    for(size_t i = 0;i < 4 ; ++i)
    {
        cout << pq.top() << endl;
        pq.pop();
    }
    vector<int> list;
    list.reserve(3);
    list[0] = 1;
    list[1] = 2;
    list[4] = 5;
    cout << list[3] << list[4] << endl;

    string file_name = "/home/bingqingqu/TAOCP/Subversion/search-by-image-svn/res/data/test40w.txt";
    string line;
    string data_buf;
    vector<string> result;
    ifstream myfile(file_name);
    int cnt = 0;
    if(!myfile.is_open())
        cerr << "break file" << endl;
    size_t n_data = 400000;
    Feature* feats = new Feature[n_data];
    double* qu = NULL;
    while(getline(myfile,line))
    {

        futil::spliter_c(line.c_str(),'\t',result);
        data_buf = result[6];
        result.clear();

        futil::spliter_c(data_buf.c_str(),',',result);
        double* temp = new double[500];
        for(size_t i=0; i<500; ++i)
        {

            temp[i] = atof(result[i].c_str());
        }
        Feature f(temp,500,cnt);
        if( cnt == 0)
        {
            qu = temp;
        }
        feats[cnt] = f;
        result.clear();
        // cout << "temp" << feats[0].data[150]  << endl;

        cnt++;
    }
    myfile.close();
    for(size_t i = 0 ; i < 500 ; ++i)
    {
        cout << feats[0].data[i] <<",";
    }
    cout << endl;
    for(size_t i = 0 ; i < 500 ; ++i)
    {
        cout << feats[100].data[i] <<",";
    }
    cout << endl;
    cout << feats[5].index<<endl;
    cout << spat::cosine(feats[0].data,feats[116073].data,500,true)<<endl;
    cout << spat::cosine(feats[0].data,feats[140922].data,500,true)<<endl;
    cout << spat::cosine(feats[0].data,feats[128786].data,500,true)<<endl;
    cout << spat::cosine(feats[0].data,feats[82475].data,500,true)<<endl;
    KDTree t(500);
    clock_t start = clock();
    t.build(feats,n_data);
    cout << "time for build:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    // t.print_trene();
    start = clock();
    vector<Feature> search_result = t.knn_basic(qu,10);
    cout << "time for knn_basic:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }

    start = clock();
    search_result = t.knn_basic_opt(qu,10);
    cout << "time for knn_basic_opt:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }

    start = clock();
    search_result = t.knn_bbf(qu,10,5000);
    cout << "time for knn_bbf:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }

    start = clock();
    search_result = t.knn_bbf_opt(qu,10,5000);
    cout << "time for knn_bbf_opt:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }


    for(size_t i = 0; i < n_data; ++i)
    {
        delete [] feats[i].data;
    }
    delete [] feats;

    // // array test
    // Feature* arr_test  = new Feature[10];
    // for(int i = 0; i < 10 ; ++i)
    // {
    //     double *tp = new double[2];
    //     tp[0] = double(i);
    //     tp[1] = double(i)*2;
    //     arr_test[i] = {tp, 2, i};
    //     cout << tp << endl;
    //     cout << arr_test[0].data << endl;
    //     cout << arr_test[0].data[0] << "," << arr_test[0].data[1] << endl;
    // }
    // for(size_t i = 0; i < 10; ++i)
    // {
    //     delete [] arr_test[i].data;
    // }
    // delete [] arr_test;

}
