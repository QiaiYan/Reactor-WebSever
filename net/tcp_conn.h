#ifndef __TCP_CONN_H__
#define __TCP_CONN_H__

#include <memory>
#include <any>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>

#include "../memory/data_buf.h"

using namespace std;

class TcpConnection;  // 与客户端连接通信类

typedef shared_ptr<TcpConnection> TcpConnSP;  // TcpConnection 类的共享指针类型 TcpConnSP

class EventLoop;
class TcpServer;  // 声明服务器类

class TcpConnection : public enable_shared_from_this<TcpConnection> {
public:
    typedef function<void(const TcpConnSP&)> ConnectionCallback;  // 连接建立的回调函数类型
    typedef function<void()> CloseCallback;  // 连接关闭的回调函数类型
    typedef function<void(const TcpConnSP&, InputBuffer*)> MessageCallback;  // 收到消息的回调函数类型

    TcpConnection(TcpServer *server, EventLoop* loop, int sockfd, struct sockaddr_in& addr, socklen_t& len);  // 构造函数
    ~TcpConnection();  // 析构函数

    EventLoop* getLoop() const { return tc_loop; }  // 获取所属的事件循环

    void add_task();  // 向事件循环添加任务

    void set_context(const any& context) { tc_context = context; }  // 设置连接的上下文信息
    auto get_context() { return &tc_context; }  // 获取连接的上下文信息

    const char* get_peer_addr() { return inet_ntoa(tc_peer_addr.sin_addr);} // 获取对端地址
    auto get_fd() { return tc_fd; }  // 获取socket文件描述符

    bool send(const char *data, int len);  // 发送数据

    void set_connected_cb(const ConnectionCallback& cb) { tc_connected_cb = cb; }  // 设置连接建立时的回调函数
    void set_message_cb(const MessageCallback& cb) { tc_message_cb = cb; }  // 设置消息到达时的回调函数
    void set_close_cb(const CloseCallback& cb) { tc_close_cb = cb; }  // 设置连接关闭时的回调函数

    void connected();  // 连接建立处理
    void active_close() { do_close(); }  // 主动发起关闭连接请求

    void set_timer_id(int id) {tc_timer_id = id; }  // 设置定时器ID
    int get_timer_id() { return tc_timer_id; }  // 获取定时器ID

private:
    inline void set_sockfd(int& fd);  // 设置socket文件描述符
    void do_read();  // 读取数据处理
    void do_write();  // 写数据处理
    void do_close();  // 关闭连接处理

    TcpServer* tc_server;  // 指向所属的服务器对象
    EventLoop* tc_loop;    // 指向所属的事件循环对象
    int tc_fd;             // 连接的socket文件描述符
    int tc_timer_id{ -1 };  // 定时器ID，默认为-1

    struct sockaddr_in tc_peer_addr;  // 对端地址信息
    socklen_t tc_peer_addrlen;  // 对端地址结构体长度

    OutputBuffer tc_obuf;  // 输出缓冲区
    InputBuffer tc_ibuf;  // 输入缓冲区

    any tc_context;  // 连接的上下文信息，可以存储任意类型的数据

    ConnectionCallback tc_connected_cb;  // 连接建立时的回调函数
    MessageCallback tc_message_cb;  // 收到消息时的回调函数
    CloseCallback tc_close_cb;  // 连接关闭时的回调函数
};

#endif