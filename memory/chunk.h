#ifndef __CHUNK_H__
#define __CHUNK_H__

struct Chunk{
    
    //有参数构造
    explicit Chunk(int size);  

    ~Chunk();

    void clear();

    void adjust();

    void copy(const Chunk *other);

    void pop(int len);

    void print_data();

    int capacity{ 0 };  //容量
    int length{ 0 };    //数据长度
    int head{ 0 };      //当前数据的起始位置的偏移量
    char *data{ nullptr };  //存储数据
    Chunk *next{ nullptr };
};

#endif