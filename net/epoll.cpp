#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "epoll.h"
#include "../log/log.h"

using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 20000;

Epoll::Epoll() : ep_epoll_fd(epoll_create1(EPOLL_CLOEXEC)), ep_events(EVENTSNUM) {
    assert(ep_epoll_fd > 0);
}

Epoll::~Epoll() {}

//添加事件以及回调函数到epoll
void Epoll::epoll_add(int fd, int event, const EventCallback& cb) {
    int final_events;
    int op;

    auto ret = ep_event_map.find(fd);   //查找判断该事件是已经存在
    if (ret == ep_event_map.end()) {
        final_events = event;    
        op = EPOLL_CTL_ADD;
    }
    else {
        final_events = ret->second.event | event;
        op = EPOLL_CTL_MOD;
    }
    
    //由传入事件类型确定其回调函数
    if (event & EPOLLIN) { 
        ep_event_map[fd].read_callback = cb;  
    }
    else if (event & EPOLLOUT) {
        ep_event_map[fd].write_callback = cb;
    }
    
    ep_event_map[fd].event = final_events;

    struct epoll_event ev;
    ev.events = final_events;
    ev.data.fd = fd;
    //根据op操作类型对事件结构体进行相应操作
    if (epoll_ctl(ep_epoll_fd, op, fd, &ev) == -1) {
        PR_ERROR("epoll ctl error for fd %d\n", fd);
        return;
    }
LOG_INFO("epoll add, fd is %d, event is %d\n", fd, final_events);
    ep_listen_fds.insert(fd);
}

//从epoll中删除特定事件
void Epoll::epoll_del(int fd, int event) {

    ep_event_map_iter ret; 
    ret = ep_event_map.find(fd);
    //没有该事件
    if ( ret == ep_event_map.end()) {
        return ;
    }

    int &target_event = ret->second.event; //获取该文件描述符在epoll中的事件类型

    target_event = target_event & (~event);  //更新事件类型
    if (target_event == 0) {
        //如果事件类型变为0,从epoll中删除该事件
        this->epoll_del(fd);
    }
    else {
        //更新epoll中的事件
        struct epoll_event ev;
        ev.events = target_event;
        ev.data.fd = fd;
        if (epoll_ctl(ep_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
            PR_ERROR("epoll ctl error for fd %d\n", fd);
            return;
        }
    }
}

//从epoll中删除特定文件描述符的所有事件
void Epoll::epoll_del(int fd) {

    ep_event_map.erase(fd);

    ep_listen_fds.erase(fd);

    epoll_ctl(ep_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

//检测事件
int Epoll::poll() {
    while (true) {
        int event_count =
            epoll_wait(ep_epoll_fd, &*ep_events.begin(), ep_events.size(), EPOLLWAIT_TIME); //其中&*ep_events.begin()是传出参数, 这是结构体容器的地址, 里边存储了已就绪的文件描述符的信息
        if (event_count < 0)
        {
            PR_ERROR("epoll wait return val <0! error no:%d, error str:%s\n", errno, strerror(errno));
            continue;
        }
        //执行已经就绪的文件描述符的回调函数
        execute_cbs(event_count);
        return event_count; //返回处理的事件个数
    }
}

//执行回调函数，参数为epoll_wait()的返回值，即检测到的已就绪的文件描述符个数
inline void Epoll::execute_cbs(int event_count) {
    for (int i = 0; i < event_count; i++) {
        //通过文件描述符找到哈希表中的迭代器
        auto ev_ret = ep_event_map.find(ep_events[i].data.fd);
        assert(ev_ret != ep_event_map.end());
        
        //通过迭代器找到就绪文件描述符的i0_event结构体
        io_event *ev = &(ev_ret->second);
        //根据检测到的事件类型调用对应的回调函数
        if (ep_events[i].events & EPOLLIN) {
LOG_INFO("execute read cb\n");
            if(ev->read_callback) ev->read_callback();
        }
        else if (ep_events[i].events & EPOLLOUT) {
LOG_INFO("execute write cb\n");
            if(ev->write_callback) ev->write_callback();
        }
        //既有读事件又有写事件
        else if (ep_events[i].events & (EPOLLHUP|EPOLLERR)) {
            if (ev->read_callback) {
                ev->read_callback();
            }
            else if (ev->write_callback) {
                ev->write_callback();
            }
            else {
                LOG_INFO("get error, delete fd %d from epoll\n", ep_events[i].data.fd);
                epoll_del(ep_events[i].data.fd);
            }
        }
    }
}
