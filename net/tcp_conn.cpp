#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "tcp_conn.h"
#include "tcp_server.h"
#include "event_loop.h"
#include "../log/pr.h"
#include "../log/log.h"

using namespace std;

TcpConnection::TcpConnection(TcpServer *server, EventLoop* loop, int sockfd, struct sockaddr_in& addr, socklen_t& len) {
    tc_server = server;
    tc_peer_addr = addr;
    tc_peer_addrlen = len;
    tc_loop = loop;
    tc_fd = sockfd;

    set_sockfd(tc_fd);
}

//用于建立连接后，将连接建立的回调函数和以及该通信文件读事件触发的回调函数添加到事件循环中
void TcpConnection::add_task() {
LOG_INFO("tcp connection add connected task to poller, conn fd is %d\n", tc_fd);
    tc_loop->add_task([shared_this=shared_from_this()](){ shared_this->connected(); });  //连接成功事件加入事件循环
LOG_INFO("tcp connection add do read to poller, conn fd is %d\n", tc_fd);
    tc_loop->add_to_poller(tc_fd, EPOLLIN, [shared_this=shared_from_this()](){ shared_this->do_read(); }); //该通信连接读事件加入对应epoll实例中
}

TcpConnection::~TcpConnection() {
    LOG_INFO("TcpConnection descontructed, fd is %d\n", tc_fd);

}

//将通信的套接字设置为非阻塞
void TcpConnection::set_sockfd(int& fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK|flag);
    int op = 1; 
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &op, sizeof(op)); //启用套接字的TCP_NODELAY选项，数据直接被发送不会被缓存
}

void TcpConnection::do_read() {
    //从通信文件中读数据到输入缓冲区
    int ret = tc_ibuf.read_from_fd(tc_fd); 
    if (ret == -1) {
        PR_ERROR("read data from socket error\n");
        this->do_close();
        return;
    }
    else if (ret == 0) {
        LOG_INFO("connection closed by peer\n");
        this->do_close();
        return;
    }
     
     //消息到达后的回调函数（即接收到消息后要执行的操作）
    tc_message_cb(shared_from_this(), &tc_ibuf);

    return;
}

//将数据发送到输出缓冲区,并在必要时将写事件添加到事件循环或 epoll 实例中，以便在后续的事件循环中进行数据发送操作
bool TcpConnection::send(const char *data, int len) {
    bool should_activate_epollout = false; 
    if(tc_obuf.length() == 0) {
        should_activate_epollout = true;
    }
    int ret = tc_obuf.write2buf(data, len); 
    if (ret != 0) {
        PR_ERROR("send data to output buf error\n");
        return false;
    }

    if (should_activate_epollout == true) {
        tc_loop->add_to_poller(tc_fd,EPOLLOUT, [this](){ this->do_write(); });   //写事件添加到epoll中
    }

    return true;
}

//将数据从输出缓冲区发送到socket
void TcpConnection::do_write() {
    while (tc_obuf.length()) {
        int ret= tc_obuf.write2fd(tc_fd);
        if (ret == -1) {
            PR_ERROR("write2fd error, close conn!\n");
            this->do_close();
            return ;
        }
        if (ret == 0) {
            break;
        }
    }

    if (tc_obuf.length() == 0) {
        tc_loop->del_from_poller(tc_fd, EPOLLOUT);
    }

    return;    
}

//端开通信连接
void TcpConnection::do_close() {
    if (tc_close_cb) {
        tc_close_cb();
    }

    tc_loop->del_from_poller(tc_fd);  //取出事件循环
    //清空输入输出缓冲区
    tc_ibuf.clear(); 
    tc_obuf.clear();

    int fd = tc_fd;
    tc_fd = -1;
    close(fd);  //关闭通信文件描述符
    
    //从服务器的连接队列中移除该连接
    tc_server->do_clean(shared_from_this());
}

void TcpConnection::connected() {
    //连接建立的回调函数存在则直接调用
    if(tc_connected_cb) {
        LOG_INFO("execute connected callback, conn fd is %d\n", tc_fd);
        tc_connected_cb(shared_from_this());   
    }
    else {
        LOG_INFO("tcp connected callback is null\n");
    }
}
