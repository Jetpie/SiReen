#include "similarHeader.hpp"


similarBasicFunction::similarBasicFunction(void)
{
}


similarBasicFunction::~similarBasicFunction(void)
{
}
/**************************************************/
/***********    数据存入llc文档     *****************/
/**************************************************/
void similarBasicFunction::write_TXT_file(const char * pFileName, float * input, int inputSize)
{
    FILE * pFile;
    /*open file*/
    pFile = fopen(pFileName,"wt");

    if ( NULL == pFile )
        //if no file, error report
    {
        perror("no file!\n");
        return;
    }
    /*write in*/
    for (int i = 0; i < inputSize-1; i++)
    {
        fprintf(pFile,"%f,",input[i]);
        if ( ferror(pFile) )
        {
            perror("error\n");
            fclose(pFile);
            return;
        }
    }
    // 最后以为不加逗号
    fprintf(pFile,"%f",input[inputSize-1]);
    if ( ferror(pFile) )
    {
        perror("error\n");
        fclose(pFile);
        return;
    }
    fclose(pFile);
    return;
}

/**************************************************/
/***********    数据存入llc文档     *****************/
/**************************************************/
void similarBasicFunction::write_TXT_file(const char * pFileName, string input)
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

/**************************************************/
/***********    txt文档数据读取     *****************/
/**************************************************/
void similarBasicFunction::read_TXT_file(const char * pFileName, float *output)
{
    FILE * pFile;
    int count = 0;
//   float *output1 = new float[500*128];
    /*open file*/
    pFile = fopen(pFileName,"rt");

    if ( NULL == pFile )
        //if no file, error report
    {
        perror("no file!\n");
        return;
    }
    /*write in*/
    while(!feof(pFile))
    {

        fscanf(pFile,"%f,",&output[count++]);
        //cout<<output[count-1]<<endl;
        if(count > 150000)
        {
            fclose(pFile);
            return;
        }
    }
    //printf("%f\n",output[127999]);
    fclose(pFile);
    return;
}

/**************************************************/
/***********    读取.之前的字符     *****************/
/**************************************************/
char *similarBasicFunction::getUrl(char *imgName)
{
    int i = 0;

    while(imgName[++i] != '.')
    {
    }
    char *imgurl = new char[i-1];
    for(int j=0; j<i; j++)
    {
        imgurl[j] = imgName[j];
    }
    return imgurl;
    delete(imgurl);
}

/**************************************************/
/***********    归一化特征矩阵     ******************/
/**************************************************/
float *similarBasicFunction::normalizedSift(float *Descriptors, int size)
{
    float *normDesc = new float[size];
    float temp,temp1;                                           //temp1和temp为存储每一个patch对应的128维向量的均方根
    int i,j;

    for(i=0; i<size/128; i++)
        //每128个元素对应一个patch的特征向量
        //分别对每个特征向量进行归一化
    {
        temp = 0;
        temp1 = 0;
        for(j=0; j<128; j++)
            //求均方差
        {
            //printf("%f\n",Descriptors[i*128 + j]);
            temp  = temp  + Descriptors[i*128 + j] * Descriptors[i*128 + j];
        }
        temp = sqrt(temp);

        //对于均方差大于1的向量，做归一化处理
        if(temp > 1)
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = Descriptors[i*128 + j]/temp;
                if(normDesc[i*128 + j] > 0.2)
                {
                    normDesc[i*128 + j] = 0.2;    //消除梯度太大的量，即归一化后大于0.2的量，强制等于0.2
                }
                temp1 = temp1 + normDesc[i*128 + j]*normDesc[i*128 + j];
            }
            temp1 = sqrt(temp1);
            for(j=0; j<128; j++)
                //第二次归一化
            {
                normDesc[i*128 + j] = normDesc[i*128 + j]/temp1;
            }

        }
        else
            //对于均方差小于1的向量，不做归一化处理
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = Descriptors[i*128 + j];
            }
        }
    }

    return normDesc;
}
/**************************************************/
/***********    字符串关键词分词     ****************/
/**************************************************/
void similarBasicFunction::split_string(const char*str, char c, vector<string>& split_vect)
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
