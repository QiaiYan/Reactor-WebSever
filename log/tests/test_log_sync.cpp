#include <chrono>
#include <vector>
#include <atomic>
#include <thread>  
#include "../log.h"  
#include "../pr.h"   

using namespace std;

const int g_item_num = 11000;  // 每个线程要写入的日志条目数
const int g_t_num = 5;          // 线程数量
atomic<int> g_t_cnt = {0};      // 原子整型变量，用于给线程计数

// 每个线程执行的函数，用于写入日志
void log_sync_test()
{
    long long tid = tid_to_ll(this_thread::get_id());  // 获取线程 ID 并转换成长整型
    PR_INFO("[threa_%d]tid is: %lld\n", g_t_cnt.fetch_add(1), tid);  // 打印线程计数和线程 ID
    for(int i=0; i<g_item_num; i++)
    {
        LOG_INFO("[tid:%lld] log sync test: %d\n", tid, i);  // 写入日志信息，包括线程 ID 和序号
    }
}

int main()
{
    Logger::get_instance()->init("./log_sync.txt");  // 初始化日志系统，假设 Logger 类提供了初始化方法

    vector<thread> threads;  // 用于存放线程的容器
    
    PR_INFO("sync log test started\n");  // 打印日志写入测试开始信息
    PR_INFO("start to log\n");           // 打印开始写入日志的信息
    auto start = chrono::steady_clock::now();  // 记录开始时间

    for(int i=0; i<g_t_num; i++)
    {
        threads.emplace_back(log_sync_test);  // 创建多个线程，并将 log_sync_test 函数作为线程执行的函数
    }
    for(int i=0; i<g_t_num; i++)
    {
        threads[i].join();   // 等待所有线程结束
    }

    auto end = chrono::steady_clock::now();   // 记录结束时间
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);  // 计算总耗时
    PR_INFO("end logging\n");   // 打印结束日志写入的信息
    PR_INFO("totally write %d items into files\n", g_t_num * g_item_num);  // 打印总共写入的日志条目数
    PR_INFO("costed time: %d ms\n" ,static_cast<int>(duration.count()));  // 打印总耗时
    return 0;
}