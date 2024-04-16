#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <sys/eventfd.h>

#include "epoll.h"

using namespace std;

class EventLoop {
public:
    typedef std::function<void()> Task;

    EventLoop();

    ~EventLoop();

    void loop();
    void quit();

    void add_task(Task&& cb);

    void add_to_poller(int fd, int event, const Epoll::EventCallback& cb) {
        el_epoller->epoll_add(fd, event, cb);
    }

    void del_from_poller(int fd, int event) {
        el_epoller->epoll_del(fd, event);
    }

    void del_from_poller(int fd) {
        el_epoller->epoll_del(fd);
    }

    // 判断当前线程是否是事件循环的线程
    bool is_in_loop_thread() const { return el_tid == this_thread::get_id(); }
    

private:
    shared_ptr<Epoll> el_epoller;  // Epoll 实例，用于事件管理
    bool el_quit{ false };  // 事件循环是否退出标志

    const thread::id el_tid{ this_thread::get_id() };  // 事件循环所在线程的 ID
    mutex el_mutex;  // 事件循环的互斥锁

    int el_evfd;  // 用于事件唤醒的文件描述符
    std::vector<Task> el_task_funcs;  // 待执行的任务列表，该任务队列可以用于对epoll实例中的文件描述符进行操作
    bool el_dealing_task_funcs{ false };  // 任务处理中标志

    void evfd_wakeup();  // 唤醒事件循环
    void evfd_read();  // 读取事件循环的事件
    void execute_task_funcs();  // 执行待处理任务
};

#endif