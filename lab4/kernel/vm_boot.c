#include "vm.h"
#include "pmm.h"
#include "memlayout.h"
#include "defs.h"
#include "riscv.h"
pagetable_t kernel_pagetable;

// 区域映射辅助函数
// map_region 函数：
// 功能：将一段连续的虚拟地址区域映射到连续的物理地址区域
// 参数说明：
// pt：目标页表
// va：起始虚拟地址
// pa：起始物理地址
// sz：映射区域的总大小
// perm：页表项的权限标志（如可读、可写、可执行等）
// 实现方式：按页（PAGE_SIZE）为单位循环调用 map_page 函数，完成整个区域的映射
static void map_region(pagetable_t pt, uint64_t va, uint64_t pa, uint64_t sz, int perm) {
    for (uint64_t off = 0; off < sz; off += PAGE_SIZE) {
        map_page(pt, va + off, pa + off, perm);
    }
}

// 创建内核页表并映射内核代码/数据/设备
void kvminit(void) 
{
    extern char etext[];
    // 1. 创建内核页表
    kernel_pagetable = create_pagetable();

    // 2. 映射内核代码段（R+X权限）
    map_region(kernel_pagetable, KERNBASE, KERNBASE,
                (uint64)etext - KERNBASE, PTE_R | PTE_X);
    // 3. 映射内核数据段及剩余物理内存（R+W权限）
    map_region(kernel_pagetable, (uint64)etext, (uint64)etext,
                PHYSTOP - (uint64)etext, PTE_R | PTE_W);
    // 4. 映射设备（UART等）
    map_region(kernel_pagetable, UART0, UART0, PGSIZE, PTE_R | PTE_W);
}

void kvminithart(void) {
    // 激活内核页表
     w_satp(MAKE_SATP(kernel_pagetable));
     sfence_vma();
}