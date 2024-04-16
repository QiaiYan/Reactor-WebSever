#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <condition_variable>
#include <thread>
#include <functional>
#include <stdexcept>
#include <assert.h>

#define  THREADPOOL_MAX_NUM 64
//#define  THREADPOOL_AUTO_GROW

using namespace std;

class Threadpool
{
public:
	typedef function<void()> Task;
    
	//构造函数，默认创建线程池的线程数为4
	inline Threadpool(unsigned short size = 4) { 
        assert(size <= THREADPOOL_MAX_NUM);
        add_thread(size); 
    }

	inline ~Threadpool()
	{
		tp_run=false; //线程池关闭状态
		tp_task_cv.notify_all();  //唤醒所有线程执行剩余任务
		for (thread& thread : tp_pool) {
			if(thread.joinable())
				thread.join(); 
		}
	}

    //使用模板，可以提交不同类型的任务，并返回得到处理结果
	template<class F, class... Args>
	decltype(auto) post_task(F&& f, Args&&... args)  //decltype(auto)c++14新增语法，可用来声明变量以及指示函数返回类型
	{
		if (!tp_run)
			throw runtime_error("post_task on Threadpool has been stopped.");

		using return_type = typename std::result_of_t<F(Args...)>; // 使用 std::result_of_t 来推断函数返回类型
		
		// 创建一个 shared_ptr 来持有 packaged_task类的可调用函数对象
		auto task = make_shared<packaged_task<return_type()>>(
			bind(forward<F>(f), forward<Args>(args)...)
		);

        // 从 packaged_task 中获取 future 对象
		future<return_type> res = task->get_future();
        
		//将该任务加入任务队列
		{
			lock_guard<mutex> lock{ tp_lock };
			tp_tasks.emplace([task](){
				(*task)();  /* *task获取智能指针管理的对象，然后使用operator（）调用这个对象，触发了这个延迟任务
				               实际上该步就是调用std::packaged_task所包装的被绑定的函数对象
				            */
			});
		}
		 // 如果开启了自动增长线程池的宏，且空闲线程数为零，则增加一个线程
#ifdef THREADPOOL_AUTO_GROW
		if (tp_idl_tnum < 1 && tp_pool.size() < THREADPOOL_MAX_NUM)
			add_thread(1);
#endif
        // 通知一个等待中的线程来执行任务
		tp_task_cv.notify_one(); 

		return res;         // 返回一个 future 对象，用于获取任务执行的结果
	}


	int idl_thread_cnt() { return tp_idl_tnum; } 

	int thread_cnt() { return tp_pool.size(); }

#ifndef THREADPOOL_AUTO_GROW
private:
#endif
    //添加线程
	void add_thread(unsigned short size)
	{
		for (; tp_pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
		{
			//添加新线程
			tp_pool.emplace_back( [this]{
				while (tp_run)
				{
					Task task; //task为包装器对象
					{
						unique_lock<mutex> lock{ tp_lock };
						tp_task_cv.wait(lock, [this]{
								return !tp_run || !tp_tasks.empty();
						});//线程池关闭或者任务队列不为空时，唤醒
						if (!tp_run && tp_tasks.empty())
							return;
						task = move(tp_tasks.front()); //取任务
						tp_tasks.pop();
					}
					tp_idl_tnum--;
					task();       //执行任务 
					tp_idl_tnum++;
				}
			});
			tp_idl_tnum++;
		}
	}

private:
    Threadpool(const Threadpool &) = delete;  //禁用拷贝构造函数

    Threadpool(Threadpool &&) = delete;   //禁用移动构造函数

    Threadpool & operator=(const Threadpool &) = delete; //禁用拷贝赋值操作符重载
 
    Threadpool & operator=(Threadpool &&) = delete;  //禁用移动赋值操作符重载

	vector<thread> tp_pool;  //存储线程的容器
	queue<Task> tp_tasks;   //任务队列
	mutex tp_lock;
	condition_variable tp_task_cv;
	atomic<bool> tp_run{ true };     //运行线程数
	atomic<int>  tp_idl_tnum{ 0 };  //空闲线程
};

#endif