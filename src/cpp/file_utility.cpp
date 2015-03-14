#include "sireen/file_utility.hpp"

namespace futil
{

    /**
     * read file to float array separated by delim
     * @param filename the input file name
     * @param output   the output float pointer
     * @param delim    delimiter
     */
    void
    file_to_pointer(const char * filename, float* output, char * delim)
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

    /**
     * read file to int array separated by delim
     * @param filename the input file name
     * @param output   the output int pointer
     * @param delim    delimiter
     */
    void
    file_to_pointer(const char * filename, int* output,char * delim)
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

    /**
     * read file to std::string
     * probably this is the most elegent way to read file into string
     * @param filename input file name
     * @return the string read from file
     */
    std::string
    file_to_string(const char * filename)
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

    /**
     * write string to file
     * @param filename output file name
     * @param input    input string
     * @param mode     write mode(w/r/a)
     */
    void
    string_to_file(const char * filename, const string input, const char * mode)
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

    /**
     * string spliter by delimiter (use a C-way for implementation)
     * @param str    input string for spliting
     * @param delim  delimiter
     * @param result result vector
     */
    void
    spliter_c(const char* str, const char delim, vector<string> &result)
    {
        int begin = 0;
        int end   = 0;

        if(!strchr(str, delim))
        {
            cout<<"error"<<endl;
            result.push_back(str);
            return;
        }

        for(int i = 0; str[i] != '\0'; i++)
        {
            if(str[i] == delim)
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

    /**
     * string spliter by delimiter (this may be slower than Spliter)
     * @param str    input string for spliting
     * @param delim  delimiter
     * @param result result vector
     */
    void
    spliter_std(const string &s, const char delim, vector<string> &result)
    {
        stringstream ss(s);
        string item;
        while (std::getline(ss, item, delim)) {
            result.push_back(item);
        }
        return;
    }


    /**
     * Retreive a list of files in a directory (except the ones that
     * begin with a dot)
     * @param out       output vector
     * @param directory directory path
     *
     */
    int
    get_files_in_dir(std::vector<string> &out, const string &directory)
    {
    #ifdef WINDOWS
        HANDLE dir;
        WIN32_FIND_DATA file_data;

        if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data))
            == INVALID_HANDLE_VALUE)
            return; /* No files found */

        do {
            const string file_name = file_data.cFileName;
            const string full_file_name = directory + "/" + file_name;
            const bool is_directory = (file_data.dwFileAttributes
                                       & FILE_ATTRIBUTE_DIRECTORY) != 0;

            if (file_name[0] == '.')
                continue;

            if (is_directory)
                continue;

            out.push_back(full_file_name);
        } while (FindNextFile(dir, &file_data));

        FindClose(dir);
        return 0;
    #else
        DIR *dir;
        struct dirent *ent;
        struct stat st;

        dir = opendir(directory.c_str());
        if(dir == NULL) {
            cout << "Error(" << errno << ") opening " << dir << endl;
            return errno;
        }
        while ((ent = readdir(dir)) != NULL) {
            const string file_name = ent->d_name;
            const string full_file_name = directory + "/" + file_name;

            if (file_name[0] == '.')
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            out.push_back(full_file_name);
        }
        closedir(dir);
        return 0;
    #endif
    }
}
