#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <assert.h>
#include <atomic>

#include "log_queue.h"
#include "pr.h"

using namespace std;

class Logger
{
public:
    typedef enum  //定义枚举类型，表示日志级别
    {
        LOG_LEVEL_ERROR,    //错误级别
        LOG_LEVEL_WARN,     //警告级别
        LOG_LEVEL_INFO,     //信息级别
        LOG_LEVEL_DEBUG,    //调试级别
        NUM_LOG_LEVELS,     //日志级别总数
    } log_level;

    static Logger *get_instance()  //使用静态方法获取Logger类唯一实例
    {
        //懒汉模式，采用局部静态变量的方法：函数首次调用的时候会创建并初始化(并且只能初始化一次），作用域为当前函数,具有记忆性，函数退出后依然存在，只是不能使用，一直到程序运行结束才会被销毁，
        static Logger instance;
        return &instance;
    }

    static void async_flush(void)
    {
        Logger::get_instance()->async_write();
    }

    static log_level get_log_level();  //获取日志级别

    static log_level set_log_level(log_level level);  //设置日志级别

    // 这个init函数不是可重入的（不支持多线程并发调用），并且应该在主线程中在启动子线程之前进行调用
    bool init(const char *file_name, int buffer_queue_size = 0, Logger::log_level = Logger::LOG_LEVEL_INFO,
                int buffer_size = 8192, int split_lines = 5000);
    
    //判断该日志对象是否被初始化
    bool is_inited()
    {
        return l_inited;
    }

    //写日志
    void write_log(const char* file_name, const char* tn_callbackname, int line_no, log_level level, const char *format, ...);

    void flush(void);

private:
    Logger();  // 私有构造函数
    Logger(const Logger&);  // 私有拷贝构造函数
    ~Logger();  // 析构函数
    void *async_write(); // 异步写入日志

private:
    char l_dir_name[128];    // 日志目录
    char l_file_name[128];   // 日志文件名
    int l_split_lines;       // 日志滚动行数
    int l_buf_size;          // 日志缓冲区大小
    long long l_count;       // 日志计数
    int l_today;             // 当前日期
    FILE *l_fp;              // 文件指针
    char *l_buf = nullptr;   // 日志缓冲区

    bool l_inited = false;   // 是否已经初始化
    buffer_queue<string> *l_buffer_queue = nullptr;   // 异步写入日志的缓冲队列
    bool l_is_async=false;         // 是否以异步方式写日志,默认情况不是以异步方式
    bool l_is_stdout = false;  // 是否输出到标准输出

    atomic<bool> is_thread_stop ={false};   // 用于控制异步写入日志的线程
    mutex l_mutex;   // 互斥量，保护关键资源

    thread *l_asyncw_thread = nullptr;  // 异步写入日志的线程
};

extern Logger::log_level g_log_level;  // 外部声明的日志级别

inline Logger::log_level Logger::get_log_level()
{
    return g_log_level;   // 获取日志级别
}

inline Logger::log_level Logger::set_log_level(Logger::log_level level)
{
    assert(level>=Logger::LOG_LEVEL_ERROR && level<=Logger::LOG_LEVEL_DEBUG);  // 使用断言，确保级别合法
    Logger::log_level old_level = g_log_level;  // 保存旧的日志级别
    g_log_level = level;  // 设置新的日志级别
    return old_level;  // 返回旧的日志级别
}

// 调试日志输出宏
#define LOG_DEBUG(format, ...)              \
    do {                                    \
        if(!Logger::get_instance()->is_inited())                       \
        {                                                               \
            PR_ERROR("logger must be inited before user!\n");           \
        }                                                               \
        if(Logger::LOG_LEVEL_DEBUG <= Logger::get_log_level())            \
        {                                       \
            Logger::get_instance()->write_log(__FILE__, __FUNCTION__,       \
                __LINE__, Logger::LOG_LEVEL_DEBUG, format, ##__VA_ARGS__);   \
            Logger::get_instance()->flush();                                \
        }                                                                   \
    } while(0)

// 信息日志输出宏
#define LOG_INFO(format, ...)               \
    do {                                   \
        if(!Logger::get_instance()->is_inited())                       \
        {                                                               \
            PR_ERROR("logger must be inited before user!\n");           \
        }                                                               \
        if(Logger::LOG_LEVEL_INFO <= Logger::get_log_level())             \
        {                                   \
            Logger::get_instance()->write_log(__FILE__, __FUNCTION__,       \
                __LINE__, Logger::LOG_LEVEL_INFO, format, ##__VA_ARGS__);   \
            Logger::get_instance()->flush();                                \
        }                                                                  \
    } while(0)

// 警告日志输出宏
#define LOG_WARN(format, ...)                                               \
    do {                                                                      \
        if(!Logger::get_instance()->is_inited())                       \
        {                                                               \
            PR_ERROR("logger must be inited before user!\n");           \
        }                                                               \
        if(Logger::LOG_LEVEL_WARN <= Logger::get_log_level())              \
        {                                                                    \
            Logger::get_instance()->write_log(__FILE__, __FUNCTION__,       \
                __LINE__, Logger::LOG_LEVEL_WARN, format, ##__VA_ARGS__);   \
            Logger::get_instance()->flush();                                  \
        }                                                                     \
    } while(0)

// 错误日志输出宏
#define LOG_ERROR(format, ...)                                              \
    do {                                                                      \
        if(!Logger::get_instance()->is_inited())                       \
        {                                                               \
            PR_ERROR("logger must be inited before user!\n");           \
        }                                                               \
        if(Logger::LOG_LEVEL_ERROR <= Logger::get_log_level())             \
        {                                                                   \
            Logger::get_instance()->write_log(__FILE__, __FUNCTION__,       \
                __LINE__, Logger::LOG_LEVEL_ERROR, format, ##__VA_ARGS__);   \
            Logger::get_instance()->flush();                                 \
        }                                                                   \
    } while(0)

#endif  // 结束条件编译指令