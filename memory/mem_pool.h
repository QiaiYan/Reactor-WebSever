#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__

#include <unordered_map>
#include <mutex>

#include "chunk.h"

using namespace std;

typedef unordered_map<int, Chunk*> pool_t;

#define MEM_CAP_MULTI_POWER (4)

typedef enum {
    mLow    = 4096, // 最低内存容量：4KB
    m4K     = mLow, // 4KB
    m16K    = m4K * MEM_CAP_MULTI_POWER, // 16KB
    m64K    = m16K * MEM_CAP_MULTI_POWER, // 64KB
    m256K   = m64K * MEM_CAP_MULTI_POWER, // 256KB
    m1M     = m256K * MEM_CAP_MULTI_POWER, // 1MB
    m4M     = m1M * MEM_CAP_MULTI_POWER, // 4MB
    mUp     = m4M // 最大内存容量：4MB
} MEM_CAP;

#define MAX_POOL_SIZE (4U *1024 *1024) // 最大内存池大小：4MB

class Mempool 
{
public:
    // 获取Mempool的单例实例
    static Mempool& get_instance() {
        static Mempool mp_instance;
        return mp_instance;
    }

    // 分配指定大小的Chunk
    Chunk *alloc_chunk(int n);
    // 默认分配4KB大小的Chunk
    Chunk *alloc_chunk() { return alloc_chunk(m4K); }

    // 回收Chunk内存
    void retrieve(Chunk *block);

    // FIXME: 使用智能指针管理Chunk或添加销毁接口以回收内存
    // static void destroy();

    // 析构函数
    ~Mempool() = default;

    // 用于调试的API
    [[deprecated("内存池调试API已弃用!")]]
    int get_total_size_kb(){ return mp_total_size_kb; }  //获取内存池总大小
    [[deprecated("内存池调试API已弃用!")]]
    int get_left_size_kb(){ return mp_left_size_kb; }   //获取内存池剩余大小
    [[deprecated("内存池调试API已弃用!")]]
    int get_list_size_byte(MEM_CAP index);      //获取指定内存池容量的字节大小
    [[deprecated("内存池调试API已弃用!")]] 
    void print_list_content(MEM_CAP index);    //用于打印指定内存池容量的内容
    
private:
    Mempool(); // 构造函数
    Mempool(const Mempool&) = delete; // 禁用拷贝构造函数
    Mempool(Mempool&&) = delete; // 禁用移动构造函数
    Mempool& operator=(const Mempool&) = delete; // 禁用赋值操作符重载
    Mempool& operator=(Mempool&&) = delete; // 禁用移动赋值操作符重载

    void mem_init(MEM_CAP size, int chunk_num); // 初始化内存

    pool_t mp_pool; // 内存池映射表
    uint64_t mp_total_size_kb; // 总内存大小（KB）
    uint64_t mp_left_size_kb; // 剩余内存大小（KB）
    mutex mp_mutex; // 互斥锁
};

#endif