#include "pmm.h"
#include "memlayout.h"
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096

struct run {
    struct run *next;
};// 指向下一个空闲页帧

static struct run *freelist = NULL;

// 对齐到页边界
typedef uint64_t physaddr_t;

void pmm_init(void) 
{
    extern char end[];//从 end 符号（内核代码和数据的结束位置）开始，将所有到 PHYSTOP（物理内存上限）的内存页帧标记为空闲
    physaddr_t p = (physaddr_t)((((uint64_t)end + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE);//将 end 地址向上对齐到最近的页边界，并将结果赋值给 p
    for (; p + PAGE_SIZE <= PHYSTOP; p += PAGE_SIZE) {
        free_page((void*)p);
    }
}
// alloc_page(void) 函数：
// 用于分配一个物理页帧
// 从空闲链表 freelist 中获取第一个空闲页帧
// 将该页帧从空闲链表中移除并返回其地址
// 如果没有空闲页帧（freelist 为空），返回 NULL
void* alloc_page(void) {
    if (!freelist) return NULL;
    struct run *r = freelist;
    freelist = r->next;
    return (void*)r;
}
// free_page(void* page) 函数：
// 用于释放一个物理页帧，将其归还给空闲链表
// 接收要释放的页帧地址作为参数
// 将该页帧添加到空闲链表的头部
// 如果传入 NULL 则不执行任何操作
void free_page(void* page) {
    if (!page) return;
    struct run *r = (struct run*)page;
    r->next = freelist;
    freelist = r;
}

// 统计当前空闲页数
int pmm_free_pages(void) {
    int cnt = 0;
    struct run *r = freelist;
    while (r) 
    {
        cnt++;
        r = r->next;
    }
    return cnt;
}
