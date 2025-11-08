#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)// execute
#define PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)//物理地址转为页表项格式（物理页号和页表项标志）

#define PTE2PA(pte) (((pte) >> 10) << 12)//页表项转为物理地址格式

#define PTE_FLAGS(pte) ((pte) & 0x3FF)//提取页表项低10位的权限标志位

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits，用于提取9位的页表索引，因为页表项有512个（2^9）
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))//计算虚拟地址中某一级页表索引的起始位偏移量。
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)//从虚拟地址 va 中提取第 level 级页表的索引。

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))//虚拟地址最大值