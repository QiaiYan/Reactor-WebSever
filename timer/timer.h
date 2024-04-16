#ifndef __TIMER_H__
#define __TIMER_H__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>

#include "../threadpool/threadpool.h"
#include "hash_map.h"

using namespace std;

#define DEFAULT_TIMER_THREAD_POOL_SIZE 2

// 定时器类
class Timer {
public:
    // 定时器节点结构体，用于表示定时任务
    struct TimerNode {
        chrono::time_point<chrono::high_resolution_clock> tn_tm_point; // 定时任务执行时间点
        function<void()> tn_callback; // 定时任务回调函数
        int repeated_id; // 重复任务 ID
        int tn_id; // 任务 ID
        int tn_repeated_num; // 重复任务的执行次数
        bool tn_is_period{ false }; // 是否为间隔任务
        bool tn_is_repeated{ false }; // 是否为重复任务
        chrono::milliseconds tn_period; // 任务执行的间隔时间
        bool operator<(const TimerNode& b) const { return tn_tm_point > b.tn_tm_point; }
    };

    enum class IdState{ Running = 0 }; // 任务状态枚举类型

    Timer() : tm_thread_pool(DEFAULT_TIMER_THREAD_POOL_SIZE) { // 初始化函数
        tm_id.store(0); // 初始化任务 ID
        tm_running.store(true); // 设置定时器运行状态
    }

    // 析构函数
    ~Timer()
    {
        tm_running.store(false);  //设置关闭状态
        tm_cond.notify_all(); // 通知所有等待线程
        if(tm_tick_thread.joinable())
        {
            tm_tick_thread.join(); // 等待定时器处理线程结束
        }
    }

    // 启动定时器的处理线程
    void run()
    {
        tm_tick_thread = thread([this]() { run_local(); }); // 在新线程中运行处理函数
    }
    
    // 检查线程池中是否有可用线程
    bool is_available() { return tm_thread_pool.idl_thread_cnt()>=0; }

    // 获取定时器中的定时任务数量
    int size() { return tm_queue.size(); }

    // 添加一个定时任务，在一定时间后执行一次，支持绑定参数
    template <typename F, typename... Args>
    int run_after(int ms_time, bool is_period, F&& f, Args&&... args) {
        TimerNode s;
        s.tn_id = tm_id.fetch_add(1); // 为任务分配 ID
        tm_id_state_map.emplace(s.tn_id, IdState::Running); // 将任务状态置为运行中
        s.tn_is_period = is_period; // 设置是否为间隔任务
        s.tn_period = chrono::milliseconds(ms_time); // 设置间隔时间
        s.tn_tm_point = chrono::high_resolution_clock::now() + s.tn_period; // 计算任务执行的时间点
        s.tn_callback = bind(forward<F>(f), forward<Args>(args)...); // 绑定任务的回调函数及参数
        unique_lock<mutex> lock(tm_mutex); // 加锁
        tm_queue.push(s); // 将任务加入定时器队列
        tm_cond.notify_all(); // 通知所有等待线程
        return s.tn_id; // 返回任务 ID
    }


    // 添加一个定时任务，在特定的时间点执行一次，支持绑定参数
    template <typename F, typename... Args>
    int run_at(const chrono::time_point<chrono::high_resolution_clock>& time_point, F&& f, Args&&... args) {
        TimerNode s;
        s.tn_id = tm_id.fetch_add(1);
        tm_id_state_map.emplace(s.tn_id, IdState::Running);
        s.tn_is_period = false;
        s.tn_tm_point = time_point;  //设置定时任务执行时间点
        s.tn_callback = bind(forward<F>(f), forward<Args>(args)...); //绑定任务的回调函数以及参数
        unique_lock<mutex> lock(tm_mutex);
        tm_queue.push(s);      //将任务加入定时器队列
        tm_cond.notify_all();
        return s.tn_id;
    }

    // 添加一个重复执行的定时任务，支持绑定参数
    template <typename F, typename... Args>
    int run_repeated(int ms_time, int repeated_num, F&& f, Args&&... args)
    {
        TimerNode s;
        s.tn_id = tm_id.fetch_add(1);
        tm_id_state_map.emplace(s.tn_id, IdState::Running);
        s.tn_is_repeated = true;
        s.tn_repeated_num = repeated_num;
        s.tn_period = chrono::milliseconds(ms_time);   //设置任务执行间隔时间
        s.tn_tm_point = chrono::high_resolution_clock::now() + s.tn_period;
        s.tn_callback = bind(forward<F>(f), forward<Args>(args)...);
        unique_lock<mutex> lock(tm_mutex);
        tm_queue.push(s);
        tm_cond.notify_all();   
        return s.tn_id; 
    }

    // 取消一个定时任务
    void cancel(int id)
    {
        if(tm_id_state_map.is_key_exist(id))
        {
            tm_id_state_map.erase(id);
        }
    }

private:
    // 定时器内部处理方法
    void run_local()
    {
        while (tm_running.load()) {
            unique_lock<mutex> lock(tm_mutex);
            if (tm_queue.empty()) {
                tm_cond.wait(lock);
                continue;
            }
            auto s = tm_queue.top();
            auto diff = s.tn_tm_point - chrono::high_resolution_clock::now();  //时间间隔
            if (chrono::duration_cast<chrono::milliseconds>(diff).count() > 0) { //判断是否到了执行时间点
                tm_cond.wait_for(lock, diff); //如果最近的一个任务节点还为到达指定的调度时间则阻塞等待直到时间点到达
                continue;
            } else {
                tm_queue.pop();
                if(!tm_id_state_map.is_key_exist(s.tn_id))
                {
                    continue;
                }
                if(s.tn_is_period)  //是间隔任务
                {
                    s.tn_tm_point = chrono::high_resolution_clock::now() + s.tn_period;  //下一次任务执行时间点
                    tm_queue.push(s); //更新下一次执行时间点后重新加入定时器任务队列
                }
                else if(s.tn_is_repeated && s.tn_repeated_num>0)  //是重复执行任务
                {
                    s.tn_tm_point = chrono::high_resolution_clock::now() + s.tn_period;
                    s.tn_repeated_num--;  //重复次数减1
                    tm_queue.push(s);     //更新下一次执行时间点后重新加入定时器队列
                }
                lock.unlock();
                tm_thread_pool.post_task(move(s.tn_callback));  //将该定时任务的任务函数放入线程池的任务队列中
            }
        }
    }

    priority_queue<TimerNode> tm_queue; // 定时器队列，采用小根堆，将任务节点按照执行时间由先到后排列。
    atomic<bool> tm_running; // 定时器运行状态
    mutex tm_mutex; // 互斥锁
    condition_variable tm_cond; // 条件变量
    thread tm_tick_thread; // 定时器处理线程

    Threadpool tm_thread_pool; // 线程池
    atomic<int> tm_id; // 定时任务 ID
    hash_map<int, IdState> tm_id_state_map; // 任务状态映射，存放任务ID以及对应状态
};

#endif