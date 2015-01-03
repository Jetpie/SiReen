#include "similarHeader.hpp"
#include "image_feature_extract.hpp"
#include <unistd.h>
#include <ctime>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>

int main(int argc, char * argv[])
{
    string imgPath = "/home/bingqingqu/TAOCP/test_images/test1.jpg";
    Mat src_new = imread(imgPath,0);
    IplImage *src_old = cvLoadImage(imgPath.c_str(), 0);

    similarKeyFunction kf;
    ImageCoder ic;
    // reset size and binSize
    int size = 0;
    int binSize = 16;

    // initialize
    VlDsiftFilter* ft_old;
    VlDsiftFilter* ft_new;
    ft_old = kf.GetDsift(src_old, size, binSize);
    ft_new = ic.dsiftDescripter(src_new,size,binSize);

    float* f_old = ft_old->descrs;
    float* f_new = ft_new->descrs;
    for(int i=0 ;i < 100;i++)
    {
        for(int j=0;j<128;j++)
        {
            cout << f_old[i*128+j]<< ",";
        }
        cout << endl;
    }

}
