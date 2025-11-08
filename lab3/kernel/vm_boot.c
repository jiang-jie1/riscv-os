#include "vm.h"
#include "pmm.h"
#include "memlayout.h"
#include "defs.h"

pagetable_t kernel_pagetable = 0;

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
void kvminit(void) {
    extern char etext[];
    kernel_pagetable = create_pagetable();
    // 映射内核代码段（R+X）可读和可执行
    map_region(kernel_pagetable, 0x80000000, 0x80000000, (uint64_t)etext - 0x80000000, PTE_R|PTE_X);
    // 映射内核数据段（R+W）可读和可写
    map_region(kernel_pagetable, (uint64_t)etext, (uint64_t)etext, PHYSTOP - (uint64_t)etext, PTE_R|PTE_W);
    // 映射UART设备 可读和可写
    map_region(kernel_pagetable, UART0, UART0, PAGE_SIZE, PTE_R|PTE_W);
}

// 激活内核页表
// kvminithart 函数：
// 功能：在当前硬件线程（hart）上激活内核页表，使虚拟内存机制生效
// 主要操作：
// 构造 SATP（Supervisor Address Translation and Protection）寄存器的值：
// 高 4 位设置为 8，表示使用 Sv39 分页模式（支持 39 位虚拟地址）
// 低 44 位设置为内核页表物理地址的页号（PPN，通过右移 12 位获得）
// 使用csrw指令将构造好的值写入 SATP 寄存器，启用分页机制
// 执行sfence.vma指令刷新 TLB（Translation Lookaside Buffer），确保页表修改生效
void kvminithart(void) {
    asm volatile("sfence.vma zero, zero");
    // Sv39: MODE=8, ASID=0, PPN=kernel_pagetable>>12
    uint64_t satp = (8L << 60) | (((uint64_t)kernel_pagetable >> 12) & 0xFFFFFFFFFFF);
    asm volatile ("csrw satp, %0" :: "r"(satp));
    asm volatile ("sfence.vma zero, zero");
}
