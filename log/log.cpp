#include <string.h>
#include <time.h>
#include <chrono>
#include <stdarg.h>
#include <stdexcept>
#include "log.h"

using namespace std;

Logger::log_level g_log_level = Logger::LOG_LEVEL_INFO;

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "[ERROR]",
  "[WARN ]",
  "[INFO ]",
  "[DEBUG]",
};

struct my_time
{
    int year;
    char month;
    char day;
    char hour;
    char minute;
    char second; 
};

//获取当前系统时间
static my_time get_current_sys_time()
{
    auto tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    my_time t = { ptm->tm_year + 1900, static_cast<char>(ptm->tm_mon + 1), static_cast<char>(ptm->tm_mday),
            static_cast<char>(ptm->tm_hour), static_cast<char>(ptm->tm_min), static_cast<char>(ptm->tm_sec)};
    return t;
}

//日志类无参构造
Logger::Logger()
{
    PR_DEBUG("log class constructed\n");
    l_count = 0;
}

//析构函数
Logger::~Logger()
{
    if(l_asyncw_thread)
    {
        is_thread_stop = true; //异步写入线程标志改变为停止状态
        if(l_asyncw_thread->joinable())  //和主线程之间仍然存在联系
        {
            l_buffer_queue->notify();  //唤醒所有读取数据的线程
            l_asyncw_thread->join();   //主线程阻塞等待该线程运行结束后，系统自动回收该线程资源
        }
        delete l_asyncw_thread;   //删除该指针
        l_asyncw_thread==nullptr;   
    }

    lock_guard<mutex> lck (l_mutex);
    if (l_fp != NULL)   //关闭文件
    {
        fclose(l_fp);
    }

    if(l_buf)    //清除日志缓冲区
    {
        delete [] l_buf;
    }

    if(l_buffer_queue)  //删除缓冲队列指针
    {
        delete l_buffer_queue;
    }
}

//初始化logger对象
 bool Logger::init(const char *file_name, int buffer_queue_size, 
                    Logger::log_level level, int buffer_size, int split_lines)
{
    if(l_inited)
    {
        PR_WARN("Logger has been initialized, do not try again!\n");
        return false;
    }

     // 如果file_name为空，将日志输出至stdout
    if(!file_name)
    {
        l_is_stdout = true;
    }

    //文件名长度大于128
    if( !l_is_stdout && strlen(file_name)>=128 )
    {
        PR_ERROR("file name must be less than 128 bytes!\n");
        // exit(-1);
        throw invalid_argument("file name must be less than 128 bytes!");;
    }

    //设置日志级别
    set_log_level(level);

    //如果设置缓冲队列的缓冲区数量大于1,则表明使用异步写入
    if (buffer_queue_size >= 1)
    {
        l_is_async = true;
        l_buffer_queue = new buffer_queue<string>(buffer_queue_size);  //初始化缓冲队列
        l_asyncw_thread = new thread(&Logger::async_flush);  //初始化异步写入线程，用于将缓冲队列中的数据写入日志文件中
    }
    
    l_buf_size = buffer_size;   // 设置缓冲区大小
    l_buf = new char[l_buf_size];   // 创建缓冲区
    memset(l_buf, '\0', l_buf_size); // 将缓冲区清零
    l_split_lines = split_lines;      // 设置分割行数

    my_time tm = get_current_sys_time();  // 获取当前系统时间
    l_today = tm.day;      // 设置当前日期
 
    //日志写到标出输出流的情况
    if(l_is_stdout)
    {
        l_inited = true;
        l_fp = stdout;   // 设置日志输出至stdout
        PR_DEBUG("succeed in using stdout as log output\n");
        PR_DEBUG("log init finished!\n");
        return true;
    }

    const char *p = strrchr(file_name, '/');  // 查找file_name中最后一个'/'的位置
    char log_file_fullname[268] = {0};       // 创建一个用于存储日志文件完整路径名称的数组

    //日志写在指定文件下

     // 如果file_name中没有'/'，返回失败
    if (p == NULL)
    {
        PR_ERROR("log file name should behind '/'\n");
        return false;
    }
    else
    {
        strcpy(l_file_name, p + 1);   // 复制文件名部分
        strncpy(l_dir_name, file_name, p - file_name + 1);   // 复制目录名部分
        snprintf(log_file_fullname, 267, "%s%04d_%02d_%02d_%s", l_dir_name, 
            tm.year, tm.month, tm.day, l_file_name);    // 构造完整的日志文件名
    
        l_fp = fopen(log_file_fullname, "a");    // 打开日志文件
    }

    // 如果打开失败，返回失败
    if (l_fp == NULL)
    {
        PR_ERROR("open %s failed!\n", log_file_fullname);
        return false;
    }

    //初始化成功
    l_inited = true;
    PR_DEBUG("succeed in using file %s as log output\n", log_file_fullname); //输出写入日志的名字
    PR_DEBUG("log init finished!\n");

    return true;
}

//写日志
void Logger::write_log(const char* file_name, const char* tn_callbackname, int line_no, log_level level, const char *format, ...)
{
    my_time my_tm = get_current_sys_time();

    {
        lock_guard<mutex> lck (l_mutex);
        l_count++;
        
        //如果是新的一天或日志数量达到分割行数
        if (l_today != my_tm.day || l_count % l_split_lines == 0)
        {
            PR_DEBUG("start to create a new log file\n");
            // 创建新的日志文件
            char new_file_name[301] = {0};
            fflush(l_fp);
            fclose(l_fp);
            char prefix[24] = {0};
            
            // 构造日志文件名的前缀
            snprintf(prefix, 23, "%04d_%02d_%02d_", my_tm.year, my_tm.month, my_tm.day);
        
            if (l_today != my_tm.day)
            {
                // 如果是新的一天
                snprintf(new_file_name, 300, "%s%s%s", l_dir_name, prefix, l_file_name);
                l_today = my_tm.day;
                l_count = 0;   // 重置日志计数
            }
            else
            {
                // 如果还是当前天，但达到了分割行数
                snprintf(new_file_name, 300, "%s%s%s.%lld", l_dir_name, prefix, l_file_name, l_count / l_split_lines); 
            }
            l_fp = fopen(new_file_name, "a");   // 打开新的日志文件
        }
    }

    va_list valst;  //可变参数列表
    va_start(valst, format);  //列表的开始位置设置为format

    string log_str;
    {
        lock_guard<mutex> lck (l_mutex);;

        //写入数据的前缀：包括写入日期，日志级别,写入文件名，调用该函数的函数名，以及函数所在的行数
        int n = snprintf(l_buf, 300, "%04d-%02d-%02d %02d:%02d:%02d %s [%s:%s:%d] ",
                        my_tm.year, my_tm.month, my_tm.day,
                            my_tm.hour, my_tm.minute, my_tm.second, LogLevelName[level],
                            file_name, tn_callbackname, line_no);
        
        //将所有可变参数添加到前缀后面
        int m = vsnprintf(l_buf + n, l_buf_size - 1, format, valst);
        l_buf[n + m] = '\n';
        l_buf[n + m + 1] = '\0';
        log_str = l_buf;
    }
    va_end(valst);
    
     //使用异步写入
    if (l_is_async)
    {
        //将要写入的数据放在缓冲队列之中，交给异步写入日志线程处理，将数据写入日志文件中
        while (!l_buffer_queue->push(log_str) && !is_thread_stop)   
        {    
                       
        }
        
    }
    else
    { //同步方式写入
        lock_guard<mutex> lck (l_mutex);
        fputs(log_str.c_str(), l_fp);   // 将日志内容写入文件
    }
}

// 刷新日志缓冲区
void Logger::flush(void)
{
    lock_guard<mutex> lck (l_mutex);
    fflush(l_fp);
}

//异步写日志，将缓冲队列中的数据读读出写入日志文件中
void* Logger::async_write()
{
    string single_line;
    while(l_buffer_queue->pop(single_line) && !is_thread_stop)  //l_buffer_queue 中取出日志信息，直到 is_thread_stop 变量被设置为 true 或者队列为空
   {
      lock_guard<mutex> lck (l_mutex);
      fputs(single_line.c_str(), l_fp);
   }
   return NULL;
}
/*
   该异步写入的任务函数存在一定缺陷：
   比如当缓冲队列的数据被全部取出后该线程的任务就执行完毕然后退出，但是如果此时又有新的日志数据被写入到任务队列中，由于异步写入到文件的线程关闭，新到来的所有日志信息都无法被写入日志文件，造成数据丢失。
   针对此问题的改进方法：
   使用任务队列中的条件变量（需要为任务队列再编写一个接口用于获取其私有的条件变量），当任务队列为空时，该线程阻塞等待新日志数据的到来，数据一到来就会被唤醒，将新到来的数据写入日志文件。
*/
