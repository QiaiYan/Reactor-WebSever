#include "pr.h" 

int pr_level = PR_LEVEL_INFO;  // 初始化日志级别变量为 PR_LEVEL_INFO

int pr_set_level(int level)  // 定义一个函数，用于设置日志级别，并返回旧的日志级别
{
    //处理设置级别大于3或者小于0的情况
    if(level >  PR_LEVEL_DEBUG)  // 如果level大于PR_LEVEL_DEBUG，则将其设置为PR_LEVEL_DEBUG
    {
        level = PR_LEVEL_DEBUG;
    }
    else if(level < PR_LEVEL_ERROR)  // 如果level小于PR_LEVEL_ERROR，则将其设置为PR_LEVEL_ERROR
    {
        level = PR_LEVEL_ERROR;
    }

    int old_level = pr_level;  // 保存旧的日志级别
    pr_level = level;  // 设置新的日志级别

    return old_level;  // 返回旧的日志级别
}

string tid_to_string(const thread::id& tid)  // 定义一个函数，用于将线程id转换为字符串
{
    ostringstream oss;  // 创建一个字符串流对象
    oss << tid << endl;  // 向字符串流中输入线程id，并添加换行符
    return oss.str();  // 返回字符串流中的内容并转换为字符串
}

long long tid_to_ll(const thread::id& tid)  // 定义一个函数，用于将线程id转换为长整型
{
    return atoll(tid_to_string(tid).c_str());  // 返回线程id转换为长整型的结果
}