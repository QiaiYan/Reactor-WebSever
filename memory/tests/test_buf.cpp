#include <stdio.h>
#include <assert.h>

#include "data_buf.h"
#include "log.h"

int main()
{
    Logger::get_instance()->init(NULL);

    FILE *fp = fopen("./test_buf.txt", "w+t");
    if(fp == NULL)
    {
        LOG_ERROR("open file failed\n");
        return -1;
    }
    int fd = fileno(fp);

    //文件中添加数据
    const char *content = "hello";
    fputs(content, fp);
    rewind(fp);//文件指针重设到起始位置
    LOG_INFO("content of file: %s", content);

    InputBuffer ib;
    OutputBuffer ob;

    //数据：文件->data_buf
    int r_cnt = ib.read_from_fd(fd);   
    LOG_INFO("read %d bytes from file by InputBuffer\n", r_cnt);
    const char *r_data = ib.get_from_buf();  
    LOG_INFO("data get from InputBuffer: %s", r_data);
    ib.clear();   //清空缓冲区

    //数据:data_buf->文件
    const char * w_data = "world";
    ob.write2buf(w_data, strlen(w_data));  //先写入数据到data_buf
    int w_cnt= ob.write2fd(fd);        //再将data_buf中的数据写到fd文件中
    LOG_INFO("write %d bytes to file by OutputBuffer\n", w_cnt);
    LOG_INFO("data written to file by OutputBuffer: %s", w_data);
    //此时data_buf中的数据为world ,fd中也为world
    

    //数据：文件->data_buf
    rewind(fp);
    r_cnt = ib.read_from_fd(fd);   //读取文件中的数据到缓冲区
    LOG_INFO("read %d bytes from file by InputBuffer\n", r_cnt);
    r_data = ib.get_from_buf();    //读取缓冲区的数据
    LOG_INFO("data get from buf: %s", r_data);    

    fclose(fp);

    return 0;
}

//该测试说明data_buf支持 data_buf与文件之间的数据相互流通