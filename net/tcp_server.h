#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <netinet/in.h>
#include <vector>
#include <chrono>
#include <mutex>

#include "tcp_conn.h"
#include "../timer/timer.h"
#include "../log/log.h"

class EventLoop;
class Threadpool;
class Acceptor;

// 前向声明TcpConnection类，以便于声明友元关系
class TcpConnection;

class TcpServer
{ 
public:
    typedef TcpConnection::ConnectionCallback ConnectionCallback;
    typedef TcpConnection::CloseCallback CloseCallback;
    typedef TcpConnection::MessageCallback MessageCallback;

    // 友元类可以访问该类的私有成员
    friend class Acceptor;
    friend class TcpConnection;

    // 构造函数
    TcpServer(EventLoop* loop, const char *ip, uint16_t port); 

    // 析构函数
    ~TcpServer();

    // 设置工作线程数量
    void set_thread_num(int t_num) { ts_thread_num = t_num; }

    // 获取下一个事件循环对象
    EventLoop* get_next_loop();

    // 启动服务器
    void start();

    // 执行清理
    void do_clean(const TcpConnSP& tcp_conn);

    // 设置TCP连接超时时间
    void set_tcp_conn_timeout_ms(int ms) { ts_tcp_conn_timout_ms = ms; }

    // 设置连接建立回调函数
    void set_connected_cb(const ConnectionCallback& cb) { ts_connected_cb = cb; }

    // 设置消息到达回调函数
    void set_message_cb(const MessageCallback& cb) { ts_msg_cb = cb; }

    // 设置连接关闭回调函数
    void set_close_cb(const CloseCallback& cb) { ts_close_cb = cb; }

private:
    // 添加新的TCP连接
    void add_new_tcp_conn(const TcpConnSP& tcp_conn) { 
        //添加一个在设定的连接超时时间后的定时任务，用于处理超时连接
        // 使用定时器
        auto timer_id = ts_timer.run_after(ts_tcp_conn_timout_ms, false, [tcp_conn]{
            LOG_INFO("tcp conn timeout!\n"); //返回连接任务ID
            tcp_conn->active_close();
        });
        tcp_conn->set_timer_id(timer_id);
        
        ts_tcp_connections.emplace_back(tcp_conn); //加入连接对象列表
    }

    // 更新连接超时时间
    void update_conn_timeout_time(const TcpConnSP& tcp_conn) {
        ts_timer.cancel(tcp_conn->get_timer_id());  //定时器任务队列中取消该超时处理
        do_clean(tcp_conn);   //从连接列表中删除
        add_new_tcp_conn(tcp_conn); //再重新添加
    }

    const char *ip;  // IP地址
    uint16_t port;  // 端口号
    unique_ptr<Acceptor> ts_acceptor;  // 接受器对象

    EventLoop *ts_acceptor_loop;  // 接受器所属的事件循环
    vector<EventLoop*> ts_conn_loops;  // 连接事件循环对象列表
    unique_ptr<Threadpool> ts_thread_pool;  // 线程池对象
    int ts_thread_num{ 1 };  // 工作线程数量
    int ts_next_loop{ -1 };  // 下一个事件循环的索引

    Timer ts_timer;  // 计时器对象
    int ts_tcp_conn_timout_ms { 60000 };  // TCP连接超时时间，默认为60000毫秒

    mutex ts_mutex;  // 互斥量
    vector<TcpConnSP> ts_tcp_connections;  // TCP连接列表

    bool ts_started;  // 服务器是否已启动标志

    ConnectionCallback ts_connected_cb;  // 连接建立回调函数
    MessageCallback ts_msg_cb;  // 消息到达回调函数（创建服务器自定义的函数）
    MessageCallback ts_message_cb;  // 消息到达回调函数  （在ts_msg_cb基础上添加了更新连接超时时间的函数，也是最终使用的响应函数）
    CloseCallback ts_close_cb;  // 连接关闭回调函数
}; 

#endif