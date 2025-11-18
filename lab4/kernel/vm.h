#ifndef VM_H
#define VM_H
#include <stdint.h>

typedef uint64_t* pagetable_t;
typedef uint64_t pte_t;

pagetable_t create_pagetable(void);
int map_page(pagetable_t pt, uint64_t va, uint64_t pa, int perm);
void destroy_pagetable(pagetable_t pt);
pte_t* walk_create(pagetable_t pt, uint64_t va);
pte_t* walk_lookup(pagetable_t pt, uint64_t va);
void dump_pagetable(pagetable_t pt, int level);

// 权限位定义
#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)

#define PAGE_SIZE 4096
#define PAGE_MASK (~(PAGE_SIZE-1))

// 地址解析宏
#define VPN_SHIFT(level) (12 + 9 * (level))
#define VPN_MASK(va, level) (((va) >> VPN_SHIFT(level)) & 0x1FF)
#define PTE_PA(pte)   (((pte) >> 10) << 12)
#define PA2PTE(pa)    (((pa) >> 12) << 10)

#endif // VM_H
