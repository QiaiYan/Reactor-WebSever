#include <assert.h>
#include <memory.h>
#include <stdio.h>

#include "chunk.h"

// 初始化date的大小为sizes
Chunk::Chunk(int size) : capacity(size), data(new char[size])
{
    assert(data); // 确保数据分配成功
}

// Chunk类的析构函数
Chunk::~Chunk()
{
    if (data)
    {
        delete[] data; // 释放分配的数据内存
    }
}

// 重置Chunk的长度和头索引
void Chunk::clear()
{
    length = head = 0;
}

// 调整Chunk的数据缓冲区
void Chunk::adjust()
{
    if (head != 0)
    {
        if (length != 0)
        {
            memmove(data, data + head, length); // 将数据移动到缓冲区的开头
        }
        head = 0; // 重置头索引
    }
}

// 从另一个Chunk复制数据
void Chunk::copy(const Chunk* other)
{
    memcpy(data, other->data + other->head, other->length); // 从另一个Chunk复制数据，不包括空白空间
    head = 0; // 重置头索引
    length = other->length; // 将长度设置为复制的数据长度
}

// 从Chunk中删除特定长度的数据
void Chunk::pop(int len)
{
    length -= len; // 减少指定数量的长度
    head += len; // 将头索引向前移动指定数量
}

// 打印存储在Chunk中的数据
void Chunk::print_data()
{
    for (int i = head; i < length; i++)
    {
        printf("%c", data[i]); // 打印数据缓冲区中的每个字符
    }
    printf("\n"); // 在结尾打印一个换行符
    return;
}