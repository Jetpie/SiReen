#include <unistd.h>
#include <ctime>
#include <vector>
#include "sireen/file_utility.hpp"
#include "sireen/image_feature_extract.hpp"

/*
 * Main
 */
int main(int argc, char * argv[]) {

    /*********************************************
     *  Step 0 - optget to receive input option
     *********************************************/
    char result_buf[256]= "res/llc/msrc_ocid_v2.txt";
    char codebook_buf[256]= "res/codebooks/msrc_ocid_v2/cbmsrc_ocid_v2.txt";
    char image_dir_buf[256]= "res/images/msrc_ocid_v2";
    /*	CHECK THE INPUT OPTIONS	*/
    //initialize the arg options
    int opt;
    while ((opt = getopt(argc, argv, "r:c:i:")) != -1) {
        switch (opt) {
        case 'r':
            sprintf(result_buf, "%s", optarg);
            break;
        case 'c':
            sprintf(codebook_buf, "%s", optarg);
            break;
        case 'i':
            sprintf(image_dir_buf, "%s", optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [options]\n", argv[0]);
            fprintf(stderr, "	-i :PATH to image directory\n");
            fprintf(stderr, "	-r :PATH to result\n");
            fprintf(stderr, "	-c :PATH to codebook\n");

            return -1;
        }
    }
    /*	CHECK END	*/

    /*--------------------------------------------
     *	Path
     --------------------------------------------*/
    string result_path = string(result_buf);
    string image_dir = string(image_dir_buf);
    string codebook_path = string(codebook_buf);

    /*--------------------------------------------
     *	PARAMETERS
     --------------------------------------------*/
    const int CB_SIZE = 500;

    /*--------------------------------------------
     *	VARIABLE READ & WRITE CACHE
     --------------------------------------------*/
    FILE * outfile;
    float *codebook = new float[128 * CB_SIZE];

    //counter for reading lines;
    unsigned int done=0;

    // Initiation
    ImageCoder icoder;


    /*********************************************
     *  Step 1 - Loading & Check everything
     *********************************************/
    // 1. codebook validation
    if (access(codebook_path.c_str(), 0)){
        cerr << "codebook not found!" << endl;
        return -1;
    }
    char delim[2] = ",";
    futil::FileToPtr(codebook_path.c_str(), codebook,delim);
    if (codebook == NULL) {
        cerr << "codebook error!" << endl;
        return -1;
    }
    // 1. write file validation
    outfile = fopen(result_path.c_str(), "wt+");
    //if no file, error report
    if ( NULL == outfile) {
        cerr << "result file initialize problem!" << endl;
        return -1;
    }

    vector<string> all_images;
    string imgfile;
    string llcstr;
    futil::GetFilesInDirectory(all_images,image_dir);
    /*********************************************
     *  Step 2 - Traverse the image directory
     *********************************************/
    clock_t start = clock();
    for(unsigned int n = 0; n < all_images.size(); ++n)
    {
        // load image source to Mat format(opencv2.4.9)
        // using the simple llcDescripter interface from
        // ImageCoder
        imgfile = all_images[n];
        try
        {
            Mat src_image = imread(imgfile,0);
            if(!src_image.data)
            {
                cout << "\tinvalid source image! --> " << imgfile << endl;
                continue;
            }
            llcstr = icoder.LLCDescriptor(src_image, codebook, CB_SIZE, 5);
        }
        catch(...)
        {
            cout << "\tbroken image! --> " << imgfile << endl;
            continue;
        }


        /*********************************************
         *  Step 4 -  write result to file
         *********************************************/

        // correct file
        fprintf(outfile, "%s\t", imgfile.c_str());
        fprintf(outfile, "%s\n", llcstr.c_str());
        // succeed count
        done++;
        // print info
        if(done % 10== 0){
            cout << "\t" << done << " Processed..." << endl;
        }

    }
    cout << "\t" << done << " Processed...(done)"
         << " <Elasped Time: " << float(clock() -start)/CLOCKS_PER_SEC
         << "s>"<< endl;
    delete codebook;
    fclose(outfile);

}
