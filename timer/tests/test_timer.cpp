#include <atomic>

#include "timer.h"
#include "log.h"
#include "pr.h"

using namespace std;

//重复执行任务回调函数
void test_repeated_func(chrono::time_point<chrono::steady_clock> t1, int num)
{
    static int cnt = num;
    long long tid = tid_to_ll(this_thread::get_id());

    auto t2 = chrono::steady_clock::now();
    int tm_diff = static_cast<int>( chrono::duration<double, milli>(t2-t1).count() );
    LOG_INFO("[tid:%lld] hello, repeated_func! total repeated_cnt = %d, left cnt: %d, time diff is %d ms\n",
                                                                     tid, num, --cnt, tm_diff);    
}

//在特定时间执行的任务的回调函数，输出该任务实际执行的时间点和设置的定时执行点之间的时间差
void test_run_at_func(chrono::time_point<chrono::steady_clock> t1, int ms)
{
    long long tid = tid_to_ll(this_thread::get_id());

    auto t2 = chrono::steady_clock::now();
    int tm_diff = static_cast<int>( chrono::duration<double, milli>(t2-t1).count() );
    LOG_INFO("[tid:%lld] hello, run_at_func ! run at %d ms, time diff is %d ms\n", tid, ms, tm_diff);   
}

//仿函数作为任务回调函数
class period_cls
{
public:
    void operator()(chrono::time_point<chrono::steady_clock> t1, int a)
    {
        long long tid = tid_to_ll(this_thread::get_id());

        auto t2 = chrono::steady_clock::now();
        int tm_diff = static_cast<int>( chrono::duration<double, milli>(t2-t1).count() );
        LOG_INFO("[tid:%lld] hello, period_cls_func with param %d! period_cls_run_cnt = %d, time diff is %d ms\n", 
                                        tid, a, period_cls_cnt.load(), tm_diff);
        period_cls_cnt.fetch_add(1);
        return;
    }
    period_cls() = default;
    period_cls(const period_cls&){}
    period_cls(period_cls&&){}
private:
    atomic<int> period_cls_cnt = 0;
};

//定时器测试
void test_timer() {

    long long tid = tid_to_ll(this_thread::get_id()); //获取主线程id

    Timer t;  
    t.run();  //启动定时器时内部会自动创建一个tick线程用于处理到期任务节点，将其任务回调函数放入线程池

    /* test repeated */
    auto t1 = std::chrono::steady_clock::now();
    int repeated_cnt = 5; //重复池数
    int repeated_timeout_ms = 800;  //间隔时间
    LOG_INFO("[tid:%lld] start to test repeated run, repeated cnt is %d, timeout is %d ms\n", tid, repeated_cnt, repeated_timeout_ms);
    //添加重复执行定时任务到定时器队列
    auto repeated_id = t.run_repeated(repeated_timeout_ms, repeated_cnt, test_repeated_func, t1, repeated_cnt);

    /* test run at certain time */
    int run_at_ms = 1250;
    t1 = std::chrono::steady_clock::now();
    auto now = chrono::high_resolution_clock::now();
    LOG_INFO("[tid:%lld] start to test run at certain time, run at %d ms after now\n", tid, run_at_ms); 
    auto certion_id = t.run_at(now + std::chrono::milliseconds(run_at_ms), test_run_at_func, t1, run_at_ms);

    /* test run once after certain time */
    int run_after_ms = 2150;
    t1 = chrono::steady_clock::now();
    LOG_INFO("[tid:%lld] start to test run once after certain time, run after %d ms\n", tid, run_after_ms);
    //任务回调函数是一个lamda表达式
    auto run_after_lamda = [t1, run_after_ms]{
                                            long long tid = tid_to_ll(this_thread::get_id());
                                            auto t2 = chrono::steady_clock::now();
                                            int tm_diff = static_cast<int>( chrono::duration<double, milli>(t2-t1).count() );
                                            LOG_INFO("[tid:%lld] hello, run_after_once_lambda_func! time diff is %d ms\n", 
                                                                                                tid, tm_diff);
                                        };
    auto once_id = t.run_after(run_after_ms, false, run_after_lamda);  //非间隔任务

    /* test run after certain time periodically */
    int run_after_ms_period = 500;
    period_cls p_cls;
    int p_param = 99;
    t1 = chrono::steady_clock::now();
    LOG_INFO("[tid:%lld] start to run after certain time periodically, period is %d ms\n", tid, run_after_ms_period);
    //添加间隔任务，任务的回调函数是一个lamda表达式（其中调用了一个仿函数）
    auto period_id = t.run_after(run_after_ms_period, true, [t1, &p_cls](int val){ p_cls(t1, val); }, p_param);
    // auto period_id = t.run_after(run_after_ms_period, true, bind(period_cls(), placeholders::_1), p_param);
    

    this_thread::sleep_for(chrono::seconds(5)); 
    /* test cancel */
    t.cancel(period_id);  //取消该定时任务
    LOG_INFO("[tid:%lld] cancel periodically running\n", tid);
}

int main()
{
    Logger::get_instance()->init(NULL);
    test_timer();
    return 0;
}