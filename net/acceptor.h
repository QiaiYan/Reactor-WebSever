#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <functional>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class EventLoop;
class TcpServer;

class Acceptor
{
public:
    // 构造函数：传入TCP服务器、事件循环、IP地址和端口号
    Acceptor(TcpServer* server, EventLoop* loop, const char *ip, uint16_t port);
    ~Acceptor();  // 析构函数

    // 获取是否正在监听
    bool is_listenning() const { return ac_listening; }

    // 开始监听
    void listen();

private:
    // 处理接受连接
    void do_accept();

    TcpServer *ac_server;  // 指向所属的服务器对象
    int ac_listen_fd;  // 监听套接字文件描述符
    EventLoop *ac_loop;  // 指向所属的事件循环对象
    bool ac_listening;  // 监听状态
    int ac_idle_fd;  // 空闲套接字文件描述符
    sockaddr_in ac_server_addr;  // 服务器地址信息
};

#endif