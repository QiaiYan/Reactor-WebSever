#include <vector>
#include <memory>
#include <string.h>

#include "mem_pool.h"
#include "log.h"

using namespace std;
vector<Chunk*> chunks;   //全局变量，存储了所有缓冲区

//输出内存池的总大小
void print_total_size()
{
    int total_size = Mempool::get_instance().get_total_size_kb();
    LOG_INFO("total size of mem pool is %dkb\n", total_size);
}

//输出内存池的剩余大小
void print_left_size()
{
    int left_size = Mempool::get_instance().get_left_size_kb();
    LOG_INFO("left size of mem pool is %dkb\n", left_size);
}

//输出指定大小的内存池的容量
void print_list_size()
{
    int list_size;
    for(int index = mLow; index <= mUp; index = index * MEM_CAP_MULTI_POWER)
    {
        list_size = Mempool::get_instance().get_list_size_byte((MEM_CAP)index);
        LOG_INFO("total size of pool's %dkb list is %dkb\n", index/1024, list_size/1024);
    }  
}

//打印所有大小的内存块中的数据
void print_list_content()
{
    int i=0;
    while(i<chunks.size())
    {
       switch (chunks[i]->capacity)
      {
       case MEM_CAP::m4K:
            printf("***************start to print 4kb chunk_size list data*******************\n");
            while( i<chunks.size() && chunks[i]->capacity==MEM_CAP::m4K)
            {
                chunks[i]->print_data();
                i++;
            }
            break;
        case MEM_CAP::m16K:
            printf("***************start to print 16kb chunk_size list data*******************\n");
            while( i<chunks.size() && chunks[i]->capacity==MEM_CAP::m16K)
            {
                chunks[i]->print_data();
                i++;
            }
            break;
        case MEM_CAP::m64K:
            printf("***************start to print 64kb chunk_size list data*******************\n");
            while( i<chunks.size() && chunks[i]->capacity==MEM_CAP::m64K)
            {
                chunks[i]->print_data();
                i++;
            }
            break;
        case MEM_CAP::m256K:
            printf("***************start to print 256kb chunk_size list data*******************\n");
            while( i<chunks.size() && chunks[i]->capacity==MEM_CAP::m256K)
            {
                chunks[i]->print_data();
                i++;
            }
            break;
        case MEM_CAP::m1M:
            printf("***************start to print m1M chunk_size list data*******************\n");
            while( i<chunks.size() && chunks[i]->capacity==MEM_CAP::m1M)
            {
                chunks[i]->print_data();
                i++;
            }
            break;
        case MEM_CAP::m4M:
            printf("***************start to print m4M chunk_size list data*******************\n");
            while( i<chunks.size() && chunks[i]->capacity==MEM_CAP::m4M)
            {
                chunks[i]->print_data();
                i++;
            }
            break;
       default:
            break;
    }

    }

}

//将智能指针管理的数据块复制多份，并将它们存储在 vector 中
void alloc_and_fill(shared_ptr<Chunk>& sp_chunk, vector<Chunk*>& chunks, MEM_CAP index, int num)
{
    for(int i=0; i<num; i++)
    {
        Chunk *b = Mempool::get_instance().alloc_chunk(index);
        b->copy(sp_chunk.get());    //获取智能指针储存的原始指针
        chunks.push_back(b);
    }
}

//回收资源
void retrieve_chunks(vector<Chunk*>& chunks)
{
    for(auto i = chunks.begin(); i!=chunks.end(); ++i)
    {
        Mempool::get_instance().retrieve(*i);
    }
}

int main()
{
    Logger::get_instance()->init(NULL);   //在终端输出

    //未分配前
    LOG_INFO("===================before alloc...\n");
    print_total_size();
    print_left_size();
    print_list_size();
    //print_list_content();

    shared_ptr<Chunk> sp_chunk = make_shared<Chunk>(10);
    string s = "hello";
    strcpy(sp_chunk->data, s.c_str());
    sp_chunk->length = strlen(s.c_str());

    LOG_INFO("===================start to alloc...\n");
    alloc_and_fill(sp_chunk, chunks, m4K, 1000);
    LOG_INFO("alloc 1000 chunks, chunk size: %dkb\n", m4K/1024);
    alloc_and_fill(sp_chunk, chunks, m16K, 1000);   
    LOG_INFO("alloc 1000 chunks, chunk size: %dkb\n", m16K/1024);
    alloc_and_fill(sp_chunk, chunks, m64K, 500);
    LOG_INFO("alloc 500 chunks, chunk size: %dkb\n", m64K/1024);
    alloc_and_fill(sp_chunk, chunks, m256K, 30);
    LOG_INFO("alloc 30 chunks, chunk size: %dkb\n", m256K/1024);
    alloc_and_fill(sp_chunk, chunks, m1M, 30);
    LOG_INFO("alloc 30 chunks, chunk size: %dkb\n", m1M/1024);
    alloc_and_fill(sp_chunk, chunks, m4M, 30);
    LOG_INFO("alloc 30 chunks, chunk size: %dkb\n", m4M/1024);

    //分配后的内存池
    LOG_INFO("===================after alloc...\n");
    print_total_size();
    print_left_size();
    print_list_size();
    //print_list_content();

    //回收内存池
    LOG_INFO("===================start to retrieve...\n");
    retrieve_chunks(chunks);

     //回收后的内存池
    LOG_INFO("===================after retrieve...\n");
    print_total_size();
    print_left_size();
    print_list_size();
    //print_list_content();

    return 0;
}