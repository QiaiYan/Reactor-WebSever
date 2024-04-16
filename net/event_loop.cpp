#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "event_loop.h"
#include "../log/pr.h"
#include "../log/log.h"

using namespace std;

EventLoop::EventLoop() : el_epoller(new Epoll()) {
    // 创建用于事件通知的文件描述符（非阻塞、执行 exec 时关闭）
    el_evfd = { eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC) };
    if(el_evfd < 0)
    {
        PR_ERROR("fail to create event_fd\n");
        exit(1);
    }
    LOG_INFO("create one eventloop, event fd is %d\n", el_evfd);

    // 将事件通知文件描述符添加到 Epoll 中
    el_epoller->epoll_add(el_evfd, EPOLLIN /*| EPOLLET*/, [this](){ this->evfd_read(); });
}

EventLoop::~EventLoop() {
    close(el_evfd);
}

// 唤醒事件循环
void EventLoop::evfd_wakeup()
{
    uint64_t one = 1;
    auto n = write(el_evfd, &one, sizeof one);
    if(n != sizeof one)
    {
        PR_ERROR("write %ld bytes to event_fd instead of 8\n", n);
    }
}

// 读取事件通知
void EventLoop::evfd_read()
{
    uint64_t one = 1;
    auto n = read(el_evfd, &one, sizeof one); 
    if(n != sizeof one)
    {
        PR_ERROR("read %ld bytes from event_fd instead of 8\n", n);
    }
}

// 添加任务到事件循环
void EventLoop::add_task(Task&& cb)
{
    LOG_INFO("eventloop, add one task\n");
    if (is_in_loop_thread())  //如果该函数调用在属于该事件循环的线程中，就直接执行处理
    {
        cb();
    }
    else
    {
        lock_guard<mutex> lock(el_mutex);
        el_task_funcs.emplace_back(move(cb));   //添加到待处理任务队列    
    }
    //在添加任务后，如果当前线程不是事件循环线程或者已经有任务在处理唤醒事件循环线程，确保当任务添加完成后，能够及时地通知事件循环进行处理
    if (!is_in_loop_thread() || el_dealing_task_funcs) { evfd_wakeup(); }
}

// 开始事件循环
void EventLoop::loop() {  //不断的从epoll中获取就绪的事件并处理，同时还会处理待处理的事件
    el_quit = false;
    while (!el_quit) {
        auto cnt = el_epoller->poll();  //检测epoll中就绪的文件描述符，并执行就绪事件相应的回调函数
        LOG_INFO("eventloop, tid %lld, loop once, epoll event cnt %d\n", tid_to_ll(this_thread::get_id()), cnt);
        execute_task_funcs();
    }
}

// 执行待处理任务
void EventLoop::execute_task_funcs() {
    std::vector<Task> functors;
    el_dealing_task_funcs = true;

    {
        lock_guard<mutex> lock(el_mutex);
        functors.swap(el_task_funcs); //从待处理任务队列中获取任务
    }

    for (size_t i = 0; i < functors.size(); ++i) functors[i](); //执行任务
    el_dealing_task_funcs = false;
}

// 退出事件循环
void EventLoop::quit() {
    el_quit = true;
    if (!is_in_loop_thread()) {
        evfd_wakeup();
    }
}