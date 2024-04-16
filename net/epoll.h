#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <set>
#include <vector>

using namespace std;

class Epoll {
public:
    typedef function<void()> EventCallback;  // 事件回调函数类型定义

    struct io_event 
    {
        int event;
        EventCallback read_callback;  // 读事件回调函数
        EventCallback write_callback;  // 写事件回调函数
    };

    Epoll();  // 构造函数

    ~Epoll();  // 析构函数

    void epoll_add(int fd, int event, const EventCallback& cb);  // 添加事件到epoll

    void epoll_del(int fd, int event);  // 从epoll中删除特定事件

    void epoll_del(int fd);  // 从epoll中删除特定文件描述符的所有事件

    int poll();  // 轮询事件

    int get_epoll_fd() { return ep_epoll_fd; }  // 获取epoll文件描述符

    void get_listen_fds(set<int> &fd_set) {  // 获取监听文件描述符集合
        fd_set = ep_listen_fds;
    }

 private:
    void execute_cbs(int event_count);  // 执行回调函数

    static const int MAXFDS = 100000;  // 最大文件描述符数
    int ep_epoll_fd;   // epoll文件描述符
    set<int> ep_listen_fds;   // 监听文件描述符集合

    typedef unordered_map<int, io_event>::iterator ep_event_map_iter;  // io_event映射迭代器类型定义
    unordered_map<int, io_event> ep_event_map;  // io_event映射表，用于找到对应文件描述符号的监听事件类型和对应回调函数

    /*
    struct epoll_event {
	uint32_t     events;      // Epoll事件 
	epoll_data_t data;        // 用户数据变量
    };
    */
    vector<epoll_event> ep_events;  // epoll事件数组
};

#endif