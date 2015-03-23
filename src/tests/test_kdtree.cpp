#include "sireen/nearest_neighbour.hpp"
#include "sireen/metrics.hpp"
#include "sireen/file_utility.hpp"
#include <ctime>
using namespace std;
using namespace nnse;


int main()
{
    clock_t start;
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

    cout << "Reading File... " << endl;
    start = clock();
    // begin read Feature from file
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
        if(cnt%10000 == 0)
        {
            cout << "\t" << cnt << " lines" << endl;
        }
    }
    myfile.close();
    cout << "File Readed (Elasped Time:" << double(clock() -start)/CLOCKS_PER_SEC
         << "s)"<< endl;


    // initialize tree with 500 feature dimension
    KDTree t(500);

    // 1. Build Tree
    cout << "Building KD-Tree... " << endl;
    start = clock();
    t.build(feats,n_data);
    cout << "Tree Built (Elasped Time:" << double(clock() -start)/CLOCKS_PER_SEC
         << "s)"<< endl;

    // 2.1 basic search
    start = clock();
    vector<Feature> search_result = t.knn_basic(qu,10);
    cout << "time for knn_basic:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    // print result
    cout << "--------------------\n" << "Results:"<< endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }
    cout << "--------------------" << endl;

    // 2.2 optimized basic search
    start = clock();
    search_result = t.knn_basic_opt(qu,10);
    cout << "time for knn_basic_opt:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    // print result
    cout << "--------------------\n" << "Results:"<< endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }
    cout << "--------------------" << endl;

    // 2.3 bbf search
    start = clock();
    search_result = t.knn_bbf(qu,10,5000);
    cout << "time for knn_bbf:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    // print result
    cout << "--------------------\n" << "Results:"<< endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }
    cout << "--------------------" << endl;

    // 2.4 optimized bbf search
    start = clock();
    search_result = t.knn_bbf_opt(qu,10,5000);
    cout << "time for knn_bbf_opt:" << double(clock() -start)/CLOCKS_PER_SEC << endl;
    // print result
    cout << "--------------------\n" << "Results:"<< endl;
    for(size_t i = 0; i < search_result.size(); ++i)
    {
        cout << search_result[i].index << endl;
    }
    cout << "--------------------" << endl;

    // 3 Rebuild the tree
    start = clock();
    cout << "Rebuilding KD-Tree... " << endl;
    t.build(feats,n_data);
    cout << "Tree Built (Elasped Time:" << double(clock() -start)/CLOCKS_PER_SEC
         << "s)"<< endl;

    // Finally, delete resources
    for(size_t i = 0; i < n_data; ++i)
    {
        delete [] feats[i].data;
    }
    delete [] feats;



}
