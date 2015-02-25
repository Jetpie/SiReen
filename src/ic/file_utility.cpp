#include "sireen/file_utility.hpp"

namespace futil
{

    /**
     * read file to float pointer separated by delim
     *
     *
     */
    void file2ptr(const char * filename, float* output, char * delim)
    {
        FILE * f = fopen(filename,"rt");
        if (f == NULL)
        {
            perror("fil2float error!\n");
            throw(errno);
        }

        char format[8] = "%f";
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
     * read file to int pointer separated by delim
     *
     *
     */
    void file2ptr(const char * filename, int* output, char * delim)
    {
        FILE * f = fopen(filename,"rt");
        if (f == NULL)
        {
            perror("fil2int error!\n");
            throw(errno);
        }

        char format[8] = "%d";
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
     * read file to std::string
     * probably this is the most elegent way to read file into string
     *
     */
    std::string file2str(const char * filename)
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

    void str2file(const char * filename, string input, const char * mode)
    {
        FILE * f = fopen(filename,mode);

        if ( f==NULL )
        {
            perror("n\n");
            return;
        }
        /*write in*/
        fprintf(f,"%s\n",input.c_str());
        if ( ferror(f) )
        {
            perror("str2file error\n");
            fclose(f);
            return;
        }
        fclose(f);
        return;
    }


    void spliter(const char*str, char c, vector<string>& result)
    {
        int begin = 0;
        int end   = 0;

        if(!strchr(str, c))
        {
            cout<<"error"<<endl;
            result.push_back(str);
            return;
        }

        for(int i = 0; str[i] != '\0'; i++)
        {
            if(str[i] == c)
            {
                end = i;
                string tmp(&str[begin], end-begin);
                result.push_back(tmp);
                begin = i+1;
            }

        }

        if(end != 0)
        {
            string tmp(&str[begin]);
            result.push_back(tmp);
        }
    }
    void split2(const std::string &s, char delim, std::vector<std::string> &elems)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return;
    }

}
