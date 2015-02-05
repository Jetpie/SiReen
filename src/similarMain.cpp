#include <unistd.h>
#include <ctime>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include "similarHeader.hpp"

/**************************************************/
#define NUM_SECTOR 200000
#define CONNECTOR  "_"
#define CB_PREFIX  "cb"
/**************************************************/


/*
 * split llc record
 */
vector<string> split(const string& str, string pattern) {
	string::size_type pos;
	vector<string> result;
	string ostr = str + pattern;
	unsigned int size = ostr.size();

	for (unsigned int i = 0; i < size; i++) {
		pos = ostr.find(pattern, i);
		if (pos < size) {
			string s = ostr.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
			break;
		}
	}
	return result;
}

/*
 * Load LLC directory and put into map
 */
map<string, string> loadLLC(string fpath, string cat) {
	// initialize llc map structure
	map<string, string> llcmap;

	// count number of lines in llc for loading bar
	int nLine = 0;
	ifstream in(fpath);
	std::string unused;
	while ( std::getline(in, unused) )
	   ++nLine;

	cout << cat <<  " num of lines: "<<nLine<<endl;

	// LOADING BAR SETTINGS
	// float nCurrent = 0.0;
	// float progress = 0.0;

	ifstream catfile(fpath.c_str());
	if (!catfile.good()) {
		cout << "broken file:" << cat.c_str() << endl;
		// validation
		return llcmap;
	}
	string line;
	getline(catfile, line);
	int num = 0;

	while (line.size() > 0) {
		// // loading bar
		// progress = (++nCurrent)/nLine;
	    // int barWidth = 70;
	    // std::cout << "[";
	    // int pos = barWidth * progress;
	    // for (int i = 0; i < barWidth; ++i) {
	    //     if (i < pos) std::cout << "=";
	    //     else if (i == pos) std::cout << ">";
	    //     else std::cout << " ";
	    // }
	    // std::cout << "] " << int(progress * 100.0) << " % [Loading LLC]\r";
	    // std::cout.flush();
	    // // end

		vector<string> items = split(line, ",");
		if (items.size() < 1) {
			getline(catfile, line);
			continue;
		}
		string key = cat + CONNECTOR + items[0];
		map<string, string>::iterator iter = llcmap.find(key);
		if (iter == llcmap.end()) {
			int begin = items[0].size() + 1;
			string val = line.substr(begin, line.size() - begin);
			llcmap.insert(map<string, string>::value_type(key, val));
			num++;
		}
		line = "";
		getline(catfile, line);

	}
	// std::cout << std::endl;
	return llcmap;
}

/*
 * Main
 */
int main(int argc, char * argv[]) {

	/*********************************************
	 * 	Step 0 - optget to receive input option
	 *********************************************/
	// category: category id for handling
	// sqlDir：sql directory
	// resultDir: result directory
	// codebookDir: codebook directory
	// logDir: log file directory
	// llcDir: llc directory

	// expand this if category mechanism vary
	char categoryBuffer[16];
	categoryBuffer[0] = '0';
	char sqlDirBuffer[256] = "/data/rsworkspace/imagesearch/res/sqldata/";
	char resultDirBuffer[256]= "/data/rsworkspace/imagesearch/res/result/";
	char codebookDirBuffer[256]= "/data/rsworkspace/imagesearch/res/codebook/";
	char logDirBuffer[256]= "/data/rsworkspace/imagesearch/log/log_f/";
	char llcDirBuffer[256]= "/data/rsworkspace/imagesearch/res/llc/";
	/*	CHECK THE INPUT OPTIONS	*/
	//initialize the arg options
	int opt;
	while ((opt = getopt(argc, argv, "c:s:r:d:g:l:")) != -1) {
		switch (opt) {
		case 'c':
			sprintf(categoryBuffer, "%s", optarg);
			break;
		case 's':
			sprintf(sqlDirBuffer, "%s", optarg);
			break;
		case 'r':
			sprintf(resultDirBuffer, "%s", optarg);
			break;
		case 'd':
			sprintf(codebookDirBuffer, "%s", optarg);
			break;
		case 'g':
			sprintf(logDirBuffer, "%s", optarg);
			break;
		case 'l':
			sprintf(llcDirBuffer, "%s", optarg);
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [options]\n", argv[0]);
			fprintf(stderr, "	-c :Specify category\n");
			fprintf(stderr, "	-s :PATH to sql directory\n");
			fprintf(stderr, "	-r :PATH to result directory\n");
			fprintf(stderr, "	-d :PATH to codebook directory\n");
			fprintf(stderr, "	-g :PATH to log directory\n");
			fprintf(stderr, "	-l :PATH to llc directory\n");
			return -1;
		}
	}
	// at least input
	if (categoryBuffer[0] == '0') {
		fprintf(stderr, "Please specify a category for handling!");
		fprintf(stderr, "Usage: %s [options]\n", argv[0]);
		fprintf(stderr, "	-c[*] :Specify category\n");
		fprintf(stderr, "	-s    :PATH to sql file\n");
		fprintf(stderr, "	-r    :PATH to result directory\n");
		fprintf(stderr, "	-d    :PATH to codebook directory\n");
		fprintf(stderr, "	-g    :PATH to log path\n");
		fprintf(stderr, "	-l    :PATH to llc directory\n");
		return -1;
	}
	/*	CHECK END	*/

	/*--------------------------------------------
	 *	Path
	 --------------------------------------------*/
	unsigned int resultInd = 0;
	stringstream sector;
	sector << resultInd;
	std::string category(categoryBuffer);

	std::string sqlPath(string(sqlDirBuffer) + category + ".txt");
	std::string codebookPath(string(codebookDirBuffer) + CB_PREFIX + category + ".txt");
	std::string llcPath(string(llcDirBuffer) + category + ".llc");
	std::string resultPath(string(resultDirBuffer) + category +"_"
						   + sector.str()  + ".txt");
	std::string logPath(string(logDirBuffer) + category + ".txt");
	// //print the path
	// cout << "######################################################" << endl;
	// cout << "category:"                     << category << endl;
	// cout << "path to sql file:" 		    << sqlPath  << endl;
	// cout << "path to codebook directory:" 	<< codebookPath << endl;
	// cout << "path to llc directory:" 	    << llcPath << endl;
	// cout << "path to result directory:" 	<< resultPath << endl;
	// cout << "path to log directory:" 	    << logPath << endl;
	// cout << "######################################################" << endl;


	/*--------------------------------------------
	 *	PARAMETERS
	 --------------------------------------------*/
	// determine 131/137/138
	string server138 = "gwk138";
	string server131 = "gwk131";
	string server59 = "gwk59";
	// getline最大值
	unsigned int MAX_LINE_LEN = 512;
    // 取数据库的字段个数
	unsigned int FIELD_LEN = 7;
    // 最小的图片路径层级数codebookDir
	unsigned int MIN_IMGPATH = 3;
	// code book size
	int CB_SIZE = 500;

	// size：dsift特征维度，binSize：dsift每个patch尺寸
	int size, binSize;
	// 特殊分词字符
	char deliminamer = 0x01;

	/*--------------------------------------------
	 *	VARIABLE READ & WRITE CACHE
	 --------------------------------------------*/
	// pFile：读取的文件。
	// rFile：结果存放的文件。
	FILE * pFile, *rFile;
	// 每一行数据存储的变量
	char prodinfo[512];
	// 字典变量
	float *codebook = new float[128 * CB_SIZE];

	//counter for reading lines;
	unsigned int lineCount=0;
	//counters for processing statues
	unsigned int successCount = 0;
	unsigned int brokenCount = 0;
	unsigned int existCount = 0;


	// Initiation
	similarBasicFunction BF;
	similarKeyFunction KF;

	// Load log file
	ofstream log(logPath);

	/*********************************************
	 * 	Step 1 - Loading & Check everything
	 *********************************************/
	// HARD VALIDATIONS - return 1 if not exist

	// 1. sql file validation
	pFile = fopen(sqlPath.c_str(), "rt");
	//if no file, error report
	if ( NULL == pFile)
	{
		cerr << "No sql file found!" << endl;
		log << "No sql file found!" << endl;
		return -1;
	}

	// 2. codebook validation
	if (access(codebookPath.c_str(), 0)){
		log << "No codebook for category " << category
			<< "!" << endl
			<< "----------------------------------" << endl;
		cerr << "No codebook for category!" << endl;
		return -1;
	}
	// allocate to codebook
	memset(codebook, 0, sizeof(float) * CB_SIZE * 128);
	BF.read_TXT_file(codebookPath.c_str(), codebook);
	if (codebook == NULL) {
		log << "No codebook for category " << category
			<< "!" << endl
			<< "----------------------------------" << endl;
		cerr << "No codebook for category!" << endl;
		return -1;
	}
	// 3. write file validation
	rFile = fopen(resultPath.c_str(), "wt+");
	//if no file, error report
	if ( NULL == rFile) {
		cerr << "result file initialize problem!" << endl;
		log << "result file initialize problem!" << endl;
		return -1;
	}

	// SOFT VALIDATIONS - no action taken
	// 1.llc map
	map<string, string> llcmap = loadLLC(llcPath,category);
	log << "LLC map size: " << llcmap.size() << endl;
	cout<< "LLC map size: " << llcmap.size() << endl;

	//initialize the category map
	map<string, int> catMap;

	/*********************************************
	 * 	Step 2 - Traverse the SQL file
	 *********************************************/
	while (fgets(prodinfo, MAX_LINE_LEN, pFile) != NULL) {
		// line count for each
		lineCount++;
		// print info
		if(lineCount % 10000 == 0){
			cout	<< "Total: " << setw(10) << left << lineCount 	<< "\t|"
					<< "Existed: "<< setw(10) << left << existCount	<< "\t|"
					<< "Broken: " << setw(10) << left << brokenCount	<< "\t|"
					<< "Succeed: " << setw(10) << left << successCount
					<< endl;
			log		<< "Total: " << setw(10) << left << lineCount 	<< "\t|"
					<< "Existed: "<< setw(10) << left << existCount	<< "\t|"
					<< "Broken: " << setw(10) << left << brokenCount	<< "\t|"
					<< "Succeed: " << setw(10) << left << successCount
					<< endl;
		}

		// read product info
		prodinfo[strlen(prodinfo) - 1] = '\0';
		// item length = 6
		// 0 category id
		// 1 urlmd5
		// 2 imgurl
		// 3 merchant id
		// 4 product weight
		// 5 Commission state
		// 6 product state
		vector<string> items;
		BF.split_string(prodinfo, deliminamer, items);
		// reset prodinfo
		memset(prodinfo, 0, sizeof(char) * 512);

		// LINE INFO VALIDATION
		if (items.size() != FIELD_LEN && items[0] != category)
			continue;

		// string to store llc
		string llcstr;
		//check map
		map<string, string>::iterator iter;
		string key = items[0] + CONNECTOR + items[1];
		iter = llcmap.find(key);
		if (iter != llcmap.end()) {
			//exist
			existCount++;
			//cout<<items[1]<<"already exist!"<<endl;
			llcstr = iter->second;

		}
		else{


			// init imgloc
			vector<string> imgloc;
			// split the image location
			BF.split_string(items[2].c_str(), '/', imgloc);

			/*********************************************
			 * 	Step 3 - handle a single file
			 *********************************************/
			// path length check > 3
			if (imgloc.size() > MIN_IMGPATH){

				// image url
				string imgfile;
				// temp variable to store final url path
				string serverPath = "";
				if (imgloc[1] == server131) {
					for (std::size_t path_i = 2, max = imgloc.size();
						 path_i != max; path_i++) {
						serverPath += "/" + imgloc[path_i];
					}
					imgfile = "/data/131_data/images/gwknew/product_images"
						+ serverPath;
				} else if (imgloc[1] == server138) {
					for (std::size_t path_i = 2, max = imgloc.size();
						 path_i != max; path_i++) {
						serverPath += "/" + imgloc[path_i];
					}
					imgfile = "/data/138_data/images/gwknew/product_images"
						+ serverPath;
				} else if (imgloc[1] == server59){
					for (std::size_t path_i = 2, max = imgloc.size();
						 path_i != max; path_i++) {
						serverPath += "/" + imgloc[path_i];
					}
					imgfile = "/data/59_data/product_images"
						+ serverPath;

				}else {
					if (imgloc[1] == "gwk137") {
						for (std::size_t path_i = 2, max = imgloc.size();
							 path_i != max; path_i++) {
							serverPath += "/" + imgloc[path_i];
						}
					} else {
						for (std::size_t path_i = 1, max = imgloc.size();
							 path_i != max; path_i++) {
							serverPath += "/" + imgloc[path_i];
						}
					}
					imgfile = "/data/137_data/images/gwknew/product_images"
						+ serverPath;
				}

				// reset size and binSize
				size = 0;
				binSize = 16;

				// initialize
				VlDsiftFilter* ft;
				/******** 读取图片并判定图片是否为空 ********/
				IplImage *SrcImage = cvLoadImage(imgfile.c_str(), 0);
				if (SrcImage == NULL) {
					// debug
					// cout<<imgfile<<" image broken!"<<endl;
					brokenCount++;
					log << "image broken: " << imgfile.c_str() << endl;
					cvReleaseImage(&SrcImage);
					continue;
				} else {
					//invalid image,such as:while,black
					CvScalar mean = cvAvg(SrcImage);
					CvScalar std;
					cvAvgSdv(SrcImage, &mean, &std);
					if ((mean.val[0] <= 255 or mean.val[0] >= 240)
						&& (mean.val[0] <= 5 or mean.val[0] >= 0)
						&& (std.val[0] <= 5 && std.val[0] >= 0)) {
						// debug
						// cout << imgfile << " image invalid!" << endl;
						brokenCount++;
						log << "image invalid: " << imgfile.c_str() << endl;
						cvReleaseImage(&SrcImage);
						continue;
					}

					try {
						ft = KF.GetDsift(SrcImage, size, binSize);
					} catch (exception e) {
						// debug
						// cout << imgfile << " getDsift fail!" << endl;
						vl_dsift_delete(ft);
						cout << items[1] << " error!" << endl;
						cvReleaseImage(&SrcImage);
						continue;
					}
				}
				cvReleaseImage(&SrcImage);
				float * Descr = new float[size];
				Descr = BF.normalizedSift(ft->descrs, size);

				// compute the final feature
				llcstr = KF.llcCompute(const_cast<float*>(Descr), codebook,
									   CB_SIZE, size);
				// write to llc
				BF.write_TXT_file(const_cast<char*>(llcPath.c_str()),
								  items[1] + "," + llcstr);
				// llcmap.insert(map<string, string>::value_type(key, llcstr));

				vl_dsift_delete(ft);
				delete[] Descr;
				Descr = NULL;

			}
			else{
				log <<"image path format error!" <<endl;
				continue;
			}

		}

		/*********************************************
		 * 	Step 4 -  write result to file
		 *********************************************/

		// correct file
		if(successCount!=0 && successCount % NUM_SECTOR == 0){
			// flush stringstream sector
			sector.str(std::string());
			sector.clear();
			fclose(rFile);
			sector << ++resultInd;
			resultPath = string(resultDirBuffer) + category +"_"
				+ sector.str() + ".txt";
			rFile = fopen(resultPath.c_str(), "wt+");
			//if no file, error report
			if ( NULL == rFile) {
				cerr << "result file initialize problem!" << endl;
				log << "result file initialize problem!" << endl;
				return -1;
			}

		}
		fprintf(rFile, "%s\t%s\t%s\t%s\t%s\t%s\t", items[0].c_str(),
				items[3].c_str(), items[1].c_str(), items[4].c_str(),
				items[5].c_str(),items[6].c_str());
		fprintf(rFile, "%s\n", llcstr.c_str());

		if (ferror(rFile)) {
			perror("result file error\n");
			log << "result file error\n" << endl;
			fclose(rFile);
		}else {
			// finally, success count
			successCount++;
		}

	}

	delete[] codebook;
	fclose(pFile);
	log		<< "Total: " << setw(10) << left << lineCount 	    << "\t|"
			<< "Existed: "<< setw(10) << left << existCount	    << "\t|"
			<< "Broken: " << setw(10) << left << brokenCount	<< "\t|"
			<< "Succeed: " << setw(10) << left << successCount
			<< endl;
	log << "**********END**********" << endl;
	log.close();
}
