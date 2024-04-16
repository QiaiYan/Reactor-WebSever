#include <sys/ioctl.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>

#include "data_buf.h"
#include "pr.h"

// BufferBase 构造函数，初始化为 nullptr
BufferBase::BufferBase() 
{
}

// BufferBase 析构函数，释放数据缓冲区
BufferBase::~BufferBase()
{
    clear();
}

// 返回数据缓冲区的长度
const int BufferBase::length() const 
{
    return data_buf != nullptr ? data_buf->length : 0;
}

// 从缓冲区中弹出指定长度的数据
void BufferBase::pop(int len) 
{
    assert(data_buf != nullptr && len <= data_buf->length);

    data_buf->pop(len);
    if(data_buf->length == 0) {
        Mempool::get_instance().retrieve(data_buf);
        data_buf = nullptr;
    }
}

// 清空数据缓冲区
void BufferBase::clear()
{
    if (data_buf != nullptr)  {
        Mempool::get_instance().retrieve(data_buf);
        data_buf = nullptr;
    }
}

// 从文件描述符中读取数据到缓冲区，返回读取到的长度
int InputBuffer::read_from_fd(int fd)
{
    int need_read;
    // 获取可读取的字节数到内核缓冲区

    if (ioctl(fd, FIONREAD, &need_read) == -1) {
        PR_ERROR("ioctl FIONREAD error\n");
        return -1;
    }
    
    //设置缓冲区大学
    //缓冲区大小为空
    if (data_buf == nullptr) {
        data_buf = Mempool::get_instance().alloc_chunk(need_read);
        if (data_buf == nullptr) {
            PR_INFO("no free buf for alloc\n");
            return -1;
        }
    }
    else {
        assert(data_buf->head == 0);
        if (data_buf->capacity - data_buf->length < (int)need_read) {   //剩余缓冲区大小装不下要读取的数据
            Chunk *new_buf = Mempool::get_instance().alloc_chunk(need_read + data_buf->length); //创建一个容量刚好满足的缓冲区
            if (new_buf == nullptr) {
                PR_INFO("no free buf for alloc\n");
                return -1;
            }
            new_buf->copy(data_buf);    //将原来的数据复制到新缓冲区
            Mempool::get_instance().retrieve(data_buf);   //回收旧缓冲区
            data_buf = new_buf;
        }
    }

    //开始读数据
    int already_read = 0;  //已读数据长度
    do { 
        if(need_read == 0) {//首次读取，或者上一次读取已完成
            already_read = read(fd, data_buf->data + data_buf->length, m4K);  //将读取到的数据放在缓冲区的数据之后
        } else {
            already_read = read(fd, data_buf->data + data_buf->length, need_read);
        }
    } while (already_read == -1 && errno == EINTR);
    if (already_read > 0)  {
        if (need_read != 0) {
            assert(already_read == need_read);
        }
        data_buf->length += already_read;
    }

    return already_read;
}

// 获取缓冲区中的数据
const char *InputBuffer::get_from_buf() const 
{
    return data_buf != nullptr ? data_buf->data + data_buf->head : nullptr;
}

// 调整数据缓冲区
void InputBuffer::adjust()
{
    if (data_buf != nullptr) {
        data_buf->adjust();
    }
}

// 将数据写入到缓冲区
int OutputBuffer::write2buf(const char *data, int len)
{
    //根据写入数据的长度，判断是否需要给缓冲区分配内存
    if (data_buf == nullptr) {
        data_buf = Mempool::get_instance().alloc_chunk(len);
        if (data_buf == nullptr) {
            PR_INFO("no free buf for alloc\n");
            return -1;
        }
    }
    else {
        assert(data_buf->head == 0);
        if (data_buf->capacity - data_buf->length < len) {
            Chunk *new_buf = Mempool::get_instance().alloc_chunk(len + data_buf->length);
            if (new_buf == nullptr) {
                PR_INFO("no free buf for alloc\n");
                return -1;
            }
            new_buf->copy(data_buf);
            Mempool::get_instance().retrieve(data_buf);
            data_buf = new_buf;
        }
    }

    memcpy(data_buf->data + data_buf->length, data, len);
    data_buf->length += len;

    return 0;
}

// 将数据从输入缓冲区写入到文件描述符
int OutputBuffer::write2fd(int fd)
{
    assert(data_buf != nullptr && data_buf->head == 0);

    int already_write = 0;

    do { 
        already_write = write(fd, data_buf->data, data_buf->length);
    } while (already_write == -1 && errno == EINTR);

    if (already_write > 0) {
        data_buf->pop(already_write);
        data_buf->adjust();
    }

    if (already_write == -1 && errno == EAGAIN) {
        already_write = 0;
    }

    return already_write;
}