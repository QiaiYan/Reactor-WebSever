#include <chrono>
#include <thread>
#include <random>

#include "threadpool.h"
#include "pr.h"
#include "log.h"

using namespace std;

random_device rd;

mt19937 mt(rd());

uniform_int_distribution<int> dist(0, 1000);

auto rnd = bind(dist, mt);   //该函数用于生成一个0到1000之间的随机整数

// 一个简单的函数，用于打印消息并进行睡眠
void func(int slp)
{
    long long tid = tid_to_ll(this_thread::get_id());
    LOG_INFO("[tid:%lld]  hello, func !\n", tid);
    int slp_t = slp + rnd();

	if (slp > 0) {
        LOG_INFO("[tid:%lld]  ======= func sleep %d  =========  \n",tid, slp_t);
		this_thread::sleep_for(chrono::milliseconds(slp_t));
	}
}

// 函数对象，用于作为任务的回调，仿函数
struct stc_tn_callbackobj {
    stc_tn_callbackobj(int val) : ret_val(val)
    {}

	int operator()(int n) {
        long long tid = tid_to_ll(this_thread::get_id());
        LOG_INFO("[tid:%lld]  hello, stc_tn_callbackobj! ret value will be %d \n", tid, ret_val);
		return ret_val;
	}

private:
    int ret_val;
};

// 静态类成员函数，用于作为任务的回调
class cls_static_func {
public:
    cls_static_func()
    {
    }

	static int cls_static_tn_callbackint(int n = 0) {
        long long tid = tid_to_ll(this_thread::get_id());
        LOG_INFO("[tid:%lld]  hello, cls_static_tn_callbackint! ret value will be %d \n", tid, n);
		return n;
	}

	static string_view cls_static_tn_callbackstr(int n, string_view str) {
        long long tid = tid_to_ll(this_thread::get_id()); 
        LOG_INFO("[tid:%lld]  hello, cls_static_tn_callbackstr !\n", tid);
		return str;
	}
};

int main()
{
    long long tid = tid_to_ll(this_thread::get_id());

	try {
        // 初始化日志系统
        Logger::get_instance()->init(NULL);

        // 创建一个线程池
		Threadpool th_pool{ 50 };
		cls_static_func stc_func;

		// 将不同类型的任务提交到线程池中
        //提交普通函数
		auto tn_callbackf = th_pool.post_task(func, 0);
        
        //提交仿函数
        int param_strc_func = 100;
		auto strc_func = th_pool.post_task(stc_tn_callbackobj{param_strc_func}, 0);
        
        //提交静态成员函数
        int param_cls_tn_callbackint = 999;
		auto cls_tn_callbackint = th_pool.post_task(stc_func.cls_static_tn_callbackint, param_cls_tn_callbackint);

        string_view param_cls_tn_callbackstr("multi args");
		auto cls_tn_callbackstr = th_pool.post_task(cls_static_func::cls_static_tn_callbackstr, rnd(), param_cls_tn_callbackstr);
        
        //提交一个lamda表达式
		auto lmda_func = th_pool.post_task([]()->string {
                                                        long long tid = tid_to_ll(this_thread::get_id()); 
                                                        LOG_INFO("[tid:%lld] hello, lamda func !\n", tid);
                                                        return "hello, lamda func !"; 
                                                        });

        // 程序休眠 1000 微秒（1 毫秒）
        LOG_INFO("[tid:%lld] =======  sleep ========= \n", tid);
		this_thread::sleep_for(chrono::microseconds(1000));

        // 提交一系列带有不同睡眠时间的任务
		for (int i = 0; i < 50; i++) {
			th_pool.post_task(func, i*100 );
		}
        int idl_cnt = th_pool.idl_thread_cnt();  //空闲线程数量
        LOG_INFO("[tid:%lld] =======  post_task all 1 =========  idl_thread_cnt=%d\n", tid, idl_cnt);
        LOG_INFO("[tid:%lld] =======  sleep ========= \n", tid);  //主线程休眠
		this_thread::sleep_for(chrono::seconds(3));

        // 获取并检查函数执行结果
        auto ret_strc_func = strc_func.get();
        auto ret_cls_tn_callbackint = cls_tn_callbackint.get();
        auto ret_cls_tn_callbackstr1 = cls_tn_callbackstr.get();

        LOG_INFO("[tid:%lld] ret_val of struct_func: %d\n", tid, ret_strc_func);
        assert(ret_strc_func == param_strc_func);

        LOG_INFO("[tid:%lld] ret_val of cls_tn_callbackint: %d\n", tid, ret_cls_tn_callbackint);
        assert(ret_cls_tn_callbackint == param_cls_tn_callbackint);

        LOG_INFO("[tid:%lld] ret_val of cls_tn_callbackstr: %s", tid, ret_cls_tn_callbackstr1.data());
        assert(ret_cls_tn_callbackstr1 == param_cls_tn_callbackstr); 

		LOG_INFO("[tid:%lld] =======  sleep ========= \n", tid);
		this_thread::sleep_for(chrono::seconds(3));

		LOG_INFO("[tid:%lld] =======  end ========= \n\n", tid);


		// 创建另一个线程池，并提交任务
		Threadpool pool(4);
		vector< future<int> > results;

		for (int i = 0; i < 8; ++i) {
            //将任务函数执行的结果保存到一个结果集中
			results.emplace_back(
				pool.post_task([i]() ->int {
                    long long tid = tid_to_ll(this_thread::get_id()); 
                    LOG_INFO("[tid:%lld] hello, world %d\n", tid, i);
					this_thread::sleep_for(chrono::seconds(1));
					return i*i;
				})
			);
		}

        idl_cnt = th_pool.idl_thread_cnt();
        LOG_INFO("[tid:%lld] =======  post_task all 2 =========  idl_thread_cnt=%d\n", tid, idl_cnt);

        //遍历执行结果
		for (auto && result : results)
        {
            LOG_INFO("[tid:%lld] get result: %d\n",tid, result.get());
        }
	}
    catch (exception& e) {
        LOG_ERROR("[tid:%lld] %s", tid, e.what());
    }

    return 0;
}