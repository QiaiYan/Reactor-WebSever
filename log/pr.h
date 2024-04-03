#ifndef __PR_H__
#define __PR_H__

#include <stdio.h>       // 引入标准输入输出库
#include <sstream>       // 引入字符串流库
#include <thread>        // 引入线程库
#include <string>        // 引入字符串库

using namespace std;    // 使用标准命名空间

#define PR_LEVEL_ERROR 0    // 定义错误日志级别值为0
#define PR_LEVEL_WARN 1     // 定义警告日志级别值为1
#define PR_LEVEL_INFO 2     // 定义信息日志级别值为2
#define PR_LEVEL_DEBUG 3    // 定义调试日志级别值为3

extern int pr_level;       // 声明外部的日志级别变量

// 定义一个带变参的日志输出宏, 其中的"\" 相当于连接符号，表示该行的内容会继续到下一行。这在定义长的宏、初始化长的字符串或者分割过长的代码行时是很有用的。
#define PR(level, val, fmt, ...)  \
    do {                                                    \
            if( level <= pr_level )                         \
                printf("[%-5s]" "[%s:%d] " fmt, val,         \
                    __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
    } while(0)

// 定义调试日志输出宏
#define PR_DEBUG(fmt, ...)  \
    PR(PR_LEVEL_DEBUG, "debug", fmt, ##__VA_ARGS__)

// 定义信息日志输出宏
#define PR_INFO(fmt, ...)  \
    PR(PR_LEVEL_INFO, "info", fmt, ##__VA_ARGS__)

// 定义警告日志输出宏
#define PR_WARN(fmt, ...)  \
    PR(PR_LEVEL_WARN, "warn", fmt, ##__VA_ARGS__)

// 定义错误日志输出宏
#define PR_ERROR(fmt, ...)  \
    PR(PR_LEVEL_ERROR, "error", fmt, ##__VA_ARGS__)

int pr_set_level(int level);    // 声明一个函数，用于设置日志级别

string tid_to_string(const thread::id& tid);   // 声明一个函数，用于将线程id转换为字符串

long long tid_to_ll(const thread::id& tid);    // 声明一个函数，用于将线程id转换为长整型

#endif  // 结束条件编译指令