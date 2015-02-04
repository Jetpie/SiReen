#include "file_utility.hpp"


FileUtil::FileUtil(void)
{
}


FileUtil::~FileUtil(void)
{
}
/**************************************************/
/***********    数据存入llc文档     *****************/
/**************************************************/
void FileUtil::write_TXT_file(const char * pFileName, string input)
{
    FILE * pFile;
    /*open file*/
    pFile = fopen(pFileName,"at");

    if ( NULL == pFile )
        //if no file, error report
    {
        perror("no coresponding llc file!\n");
        return;
    }
    /*write in*/
    fprintf(pFile,"%s\n",input.c_str());
    if ( ferror(pFile) )
    {
        perror("error\n");
        fclose(pFile);
        return;
    }
    fclose(pFile);
    return;
}


/*
 * read codebook from file
 *
 */
void FileUtil::file2ptr(const char * filename, float* output, char * delim)
{
    FILE * f = fopen(filename,"rt");
    if (f == NULL)
    {
        perror("fil2float error!\n");
        throw(errno);
    }
    char format[8] = "%f";
        //FormatSpecifier<T>::format;
    // concat final format for fscanf
    strcat(format,delim);
    while(!feof(f))
    {
        fscanf(f,format,output++);
    }
    fclose(f);
    return;
}
/*
 * read codebook from file
 *
 */
void FileUtil::file2ptr(const char * filename, int* output, char * delim)
{
    FILE * f = fopen(filename,"rt");
    if (f == NULL)
    {
        perror("fil2float error!\n");
        throw(errno);
    }
    char format[8] = "%d";
        //FormatSpecifier<T>::format;
    // concat final format for fscanf
    strcat(format,delim);
    while(!feof(f))
    {
        fscanf(f,format,output++);
    }
    fclose(f);
    return;
}

/**************************************************/
/***********    字符串关键词分词     ****************/
/**************************************************/
void FileUtil::split_string(const char*str, char c, vector<string>& split_vect)
{
    int begin = 0;
    int end   = 0;

    if(!strchr(str, c))
    {
        cout<<"error"<<endl;
        split_vect.push_back(str);
        return;
    }

    for(int i = 0; str[i] != '\0'; i++)
    {
        if(str[i] == c)
        {
            end = i;
            string tmp(&str[begin], end-begin);
            split_vect.push_back(tmp);
            begin = i+1;
        }

    }

    if(end != 0)
    {
        string tmp(&str[begin]);
        split_vect.push_back(tmp);
    }
}

/*
 * read file to std::string
 * probably this is the most elegent way to read file into string
 *
 */
std::string
FileUtil::file2str(const char * filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}
