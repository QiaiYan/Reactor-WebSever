#include <assert.h>

#include "../log/pr.h"
#include "mem_pool.h"

// 内存初始化函数，根据给定的大小和数量初始化内存池
void Mempool::mem_init(MEM_CAP size, int chunk_num)
{
    Chunk *prev; 
    // 尝试分配指定大小的第一个Chunk
    mp_pool[size] = new (std::nothrow) Chunk(size); // 在失败时返回null，不抛出异常
    // 检查分配是否成功
    if (mp_pool[size] == nullptr) {
        PR_ERROR("new chunk %d error", static_cast<int>(size));
        exit(1);
    }
    prev = mp_pool[size];

    // 迭代创建指定数量的Chunk并连接起来
    for (int i = 1; i < chunk_num; i ++) {
        prev->next = new (std::nothrow) Chunk(size);
        if (prev->next == nullptr) {
            PR_ERROR("new chunk m4K error");
            exit(1);
        }
        prev = prev->next;
    }
    // 更新总内存大小
    mp_total_size_kb += size / 1024 * chunk_num;
}

// Mempool类的构造函数，初始化内存池并设置总内存大小和剩余内存大小
Mempool::Mempool() : mp_total_size_kb(0), mp_left_size_kb(0)
{
    mem_init(m4K, 2000);
    mem_init(m16K, 500);
    mem_init(m64K, 250);
    mem_init(m256K, 100);
    mem_init(m1M, 25);
    mem_init(m4M, 10);
    mp_left_size_kb = mp_total_size_kb;
}

// 分配指定大小的Chunk内存块
Chunk *Mempool::alloc_chunk(int n) 
{
    int index;
    bool found = false;
    // 查找适合大小的内存块，采取向上整
    for(index = mLow; index <= mUp; index = index * MEM_CAP_MULTI_POWER)
    {
        if(n <= index)
        {
            found = true;
            break;
        }
    }

    if(!found)
    {
        return nullptr;
    }

    // 加锁并执行分配内存块的操作
    lock_guard<mutex> lck(mp_mutex);
    // 如果对应大小的内存池为空，尝试动态分配
    if (mp_pool[index] == nullptr) {
        if (mp_total_size_kb + index / 1024 >= MAX_POOL_SIZE) {
            PR_ERROR("beyond the limit size of memory!\n");
            exit(1);
        }

        Chunk *new_buf = new (std::nothrow) Chunk(index);
        if (new_buf == nullptr) {
            PR_ERROR("new chunk error\n");
            exit(1); 
        } 
        mp_total_size_kb += index / 1024; //新分配了内存块，内存池大小增加
        return new_buf;
    }

    // 从内存池中拿取内存块，拿取对应大小的内存池头部内存块
    Chunk *target = mp_pool[index];
    mp_pool[index] = target->next;  //更新头部内存块
    target->next = nullptr;
    mp_left_size_kb -= index / 1024;  //剩余内存池大小减小

    return target;
}

// 回收内存块到内存池
void Mempool::retrieve(Chunk *block)
{
    int index = block->capacity;
    //数据长度和起始偏移量置为0
    block->length = 0;
    block->head = 0;

    // 加锁并执行回收内存操作
    lock_guard<mutex> lck(mp_mutex);
    assert(mp_pool.find(index) != mp_pool.end());

    block->next = mp_pool[index];
    //更新块头
    mp_pool[index] = block;
    mp_left_size_kb += block->capacity / 1024;
}

// 获取指定内存池容量大小的总字节大小
int Mempool::get_list_size_byte(MEM_CAP index)
{
    int size = 0;
    // 加锁并计算指定内存池容量的总字节大小
    lock_guard<mutex> lck(mp_mutex);
    assert(mp_pool.find(index) != mp_pool.end());
    Chunk *node = mp_pool[index];

    while(node)
    {
        size += node->capacity;
        node = node->next;
    } 

    return size;
}

// 打印指定内存池容量大小的数据内容
void Mempool::print_list_content(MEM_CAP index)
{
    // 加锁并打印内存池容量大小的数据内容
    lock_guard<mutex> lck(mp_mutex);
    int cnt = 0;
    printf("***************start to print %dkb chunk_size list data*******************\n", index / 1024);
    assert(mp_pool.find(index) != mp_pool.end());
    Chunk *node = mp_pool[index];

    while (node)
    {
        if(cnt <= 5)
            node->print_data();
        cnt++;
        node = node->next;
    }
    printf("...\n");
    printf("******************end, node cnt is %d************************\n\n", cnt);
}