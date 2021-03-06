#include <unistd.h>
#include <ctime>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include "sireen/file_utility.hpp"
#include "sireen/image_feature_extract.hpp"
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
float* eigen_mat_test()
{
    vector<float> v;
    v.push_back(1.0);
    v.push_back(2.0);
    return &v[0];
}
void print_vector(float *x)
{
    for(size_t i = 0; i < 2; ++i)
    {
        cout << x[i] << endl;
    }
}
int main(int argc, char * argv[])
{
    string images[] = {"test1.jpg","test2.jpg","test3.jpg","test4.jpg","test5.jpg","test6.jpg"};
    string prefix = "/home/bingqingqu/TAOCP/Datasets/test/";
    string codebookPath =  "/home/bingqingqu/TAOCP/Datasets/test/cb1008001019.txt";
    ImageCoder ic;
    float *codebook = new float[128 * 500];

    // clock_t s;
    // s = clock();
    char delim[2] = ",";
    // load codebook
    futil::file_to_pointer(codebookPath.c_str(), codebook,delim);
    // cout << "codebook:" << float(clock() -s) << endl;

    // s = clock();
    // string result_str = futil::file2str(codebookPath.c_str());
    // cout << "file2str:" << float(clock() -s) << endl;


    // const std::string words= "Hello,world,I,love,u,a ,ha,g,a,ha,s,csdsasd,d@@,s!";
    // // const char words1[]= "Hello,world,I,love,u,a ,ha,g,a,ha,s,csdsasd,d@@,s!";
    // std::vector<std::string> result;
    // s = clock();
    // futil::spliter(result_str.c_str(),',',result);
    // cout << "split1:" << float(clock() -s) << endl;
    // cout << result[5]<<endl;

    // std::vector<std::string> result2;
    // s = clock();
    // futil::split2(result_str,',',result2);
    // cout << "split2:" << float(clock() -s) << endl;
    // cout << result2[5]<<endl;
    clock_t start;
    FILE * f = fopen("/home/bingqingqu/TAOCP/Datasets/test/result_new.txt","w+");
    fclose(f);
    for(int i=0;i<6;i++)
    {
        Mat src_new = imread(prefix+images[i],0);

        // test
        string llc_test;
        start= clock();
        try{
            llc_test = ic.llc_sift(src_new, codebook, 500, 5);

            // cout << llc_test<<endl;
            futil::string_to_file("/home/bingqingqu/TAOCP/Datasets/test/result_new.txt",
                            llc_test, "a+");
        }
        catch(...){
            cout << "fail to llc" <<endl;
        }
        cout << "time test:" << float(clock() -start)/CLOCKS_PER_SEC << endl;
        // cout << llc_test<<endl;
    }
    delete [] codebook;
    string directory = "/home/bingqingqu/TAOCP/Datasets/test/";
    vector<string> files_in_dir;
    futil::get_files_in_dir(files_in_dir,directory);
    cout << files_in_dir[0]<<","<<files_in_dir[1] << endl;

    // vector<float> test_descr;
    // float * descr_buf = new float[128];
    // for(int i=0;i<128;++i)
    // {
    //     descr_buf[i] = 1.5;
    // }
    // std::copy(descr_buf, descr_buf+128, back_inserter(test_descr));
    // std::copy(descr_buf, descr_buf+128, back_inserter(test_descr));
    // delete [] descr_buf;
    // for(vector<float>::iterator it(test_descr.begin()); it!=test_descr.end();++it)
    //     cout << " " << *it;
    // cout << endl;
    // float *descr_ptr = &test_descr[0];
    // for(int i=0;i<256;++i)
    //     cout << " " << descr_ptr[i] ;
    // cout << endl;


    // Vector2f v(5.0,6.0);
    // float* vvv = eigen_mat_test();
    // for(size_t i = 0; i< 2; ++i)
    //     cout << vvv[i] << "," << endl;
    vector<float> xxx;
    xxx.reserve(100);
    xxx.push_back(0.5);
    cout << xxx.capacity() << endl;
    cout << xxx.size() << endl;
    xxx.clear();
    cout << xxx.capacity() << endl;
    cout << xxx.size() << endl;
}
