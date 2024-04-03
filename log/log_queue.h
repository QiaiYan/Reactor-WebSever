#ifndef __Logger_BUFFER_H__
#define __Logger_BUFFER_H__

#include <stdlib.h>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <assert.h>
#include <stdexcept>

#include "pr.h"

using namespace std;

// 声明一个模板类 buffer_queue，用于表示一个缓冲队列
template <class T>
class buffer_queue
{
public:
    // 默认构造函数，创建一个缓冲队列，初始大小为默认容量 capacity
    buffer_queue() : b_array(new T[capacity]) 
    {

    }

    // 显式构造函数，可以指定最大大小和是否开启调试模式
    // 当最大大小小于等于 0 时，抛出异常
    explicit buffer_queue(int max_size, bool debug=false)
    {
        if (max_size <= 0)
        {
            PR_ERROR("max_size is illegal!\n");
            throw invalid_argument("max_size is illegal!");
        }

        b_debug = debug;     //设置调式模式
        capacity = max_size; 
        b_array = new T[capacity];   //创建一个指定大小的缓冲数组  
    }

    // 清空缓冲队列
    void clear()
    {
        lock_guard<mutex> lck (b_mutex);
        if(b_debug)
        {
            b_rcnt = 0;
            b_wcnt = 0;
        }
        b_first = -1;
        b_last = -1;
        b_size = 0;
    }

    // 析构函数，释放缓冲队列的内存
    ~buffer_queue()
    {
        lock_guard<mutex> lck (b_mutex);
        if (b_array != NULL)
            delete [] b_array;
    }

    // 返回读取次数
    int get_rcnt() const
    {
        lock_guard<mutex> lck (b_mutex);
        if(b_debug)
        {
            return b_rcnt;    
        }
        PR_WARN("this method is invalid when q_debug is false!\n");
        return -1;
    }

    // 返回写入次数
    int get_wcnt() const
    {
        lock_guard<mutex> lck (b_mutex);
        if(b_debug)
        {
            return b_wcnt;    
        }
        PR_WARN("this method is invalid when q_debug is false!\n");
        return -1;
    }

    // 返回当前缓冲队列大小
    int get_size() const
    {
        lock_guard<mutex> lck (b_mutex);       
        assert(b_size >= 0 && b_size <= capacity);   //断言，如果不满足条件，则会终止程序，并输出错误信息，断言只能在调试时有效
        return b_size;
    }
 
    // 判断缓冲队列是否已满
    bool is_full() const
    {
        lock_guard<mutex> lck (b_mutex);       
        return b_size == capacity ? true : false;
    }

    // 判断缓冲队列是否为空
    bool is_empty() const
    {
        lock_guard<mutex> lck (b_mutex);       
        return b_size == 0 ? true :false;
    }

    // 返回缓冲队列容量
    int get_capacity() const
    {
        return capacity;
    }

    // 返回队首元素
    bool front(T &value) 
    {
        lock_guard<mutex> lck (b_mutex);
        if (0 == b_size)
        {
            return false;
        }
        value = b_array[b_first+1];
        return true;
    }

    // 返回队尾元素
    bool back(T &value) 
    {
        lock_guard<mutex> lck (b_mutex);
        if (0 == b_size)
        {
            return false;
        }
        value = b_array[b_last];
        return true;
    }

    // 往队尾添加元素
    bool push(const T &item)
    {
        unique_lock<mutex> lck (b_mutex);;
        if (b_size >= capacity)
        {
            b_cond.notify_all();       //唤醒消费者，此时队列中有缓存信息，可以读走
            return false;
        }

        b_last = (b_last + 1) % capacity;  //指向队尾指针
        b_array[b_last] = item;

        b_size++;
        if(b_debug)
        {
            b_wcnt++;
        }
        b_cond.notify_all();   //唤醒消费者
        
        return true;  //添加成功
    }

    // 通知等待中的线程
    void notify()
    {
        b_cond.notify_all();
    }

    // 从队首移除元素
    bool pop(T &item)
    {
        unique_lock<mutex> lck (b_mutex);
        if (b_size <= 0)
        {           
            b_cond.wait(lck);
            if(b_size <= 0)  //唤醒后必须再判断一次，在多线程模式中，可能其他线程提前被唤醒并移除了元素，导致b_size此时已经等于0
            {
                return false;
            }
        }
        
        //移除首元素
        b_first = (b_first + 1) % capacity;  //得到队首指针，最开始指向-1
        item = b_array[b_first];   //传出队首参数
  
        b_size--;
        if(b_debug)
        {
            b_rcnt++;
        }

        return true;       
    }

    // 从队首移除元素，支持超时参数
    bool pop(T &item, int ms_timeout)
    {
        unique_lock<mutex> lck (b_mutex);
        if (b_size <= 0)
        {           
            if(b_cond.wait_for(lck, chrono::milliseconds(ms_timeout)) == cv_status::timeout) //超过指定时间，条件变量自动唤醒
                return false;
        }

        if (b_size <= 0)
        {            
            return false;
        }

        b_first = (b_first + 1) % capacity;
        item = b_array[b_first];

        b_size--;
        if(b_debug)
        {
            b_rcnt++;
        }
        
        return true;
    }

private:
    mutable mutex b_mutex;      // 互斥锁用于保护缓冲队列的并发操作
    condition_variable b_cond;  // 条件变量，用于在队列满或空时等待或通知

    T *b_array;                 // 缓冲数组指针，指向元素类型为T的数组
    long long b_rcnt = 0;       // 读取次数
    long long b_wcnt = 0;       // 写入次数
    bool b_debug = false;       // 调试模式标志
    int b_first = -1;           // 队首指针,相当于哑节点
    int b_last = -1;            // 队尾指针
    int b_size = 0;             // 当前队列大小
    int capacity = 1000;        // 队列容量
};

#endif