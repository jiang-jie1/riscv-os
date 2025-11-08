#include "vm.h"
#include "pmm.h"
#include "defs.h"

// Sv39三级页表参数
enum { LEVELS = 3 };

// 创建空页表（根页表）
pagetable_t create_pagetable(void) {
    pagetable_t pt = (pagetable_t)alloc_page();
    if (pt)
        for (int i = 0; i < 512; i++) pt[i] = 0;
    return pt;
}

// 递归查找/创建页表项，
pte_t* walk_create(pagetable_t pt, uint64_t va) {
    for (int level = LEVELS-1; level > 0; level--) {
        int idx = VPN_MASK(va, level);
        if (!(pt[idx] & PTE_V)) {
            pagetable_t newpt = (pagetable_t)alloc_page();
            if (!newpt) return 0;
            for (int i = 0; i < 512; i++) newpt[i] = 0;
            pt[idx] = PA2PTE((uint64_t)newpt) | PTE_V;
        }
        pt = (pagetable_t)(PTE_PA(pt[idx]));
    }
    return &pt[VPN_MASK(va, 0)];
}

// 只查找，不创建
pte_t* walk_lookup(pagetable_t pt, uint64_t va) {
    for (int level = LEVELS-1; level > 0; level--) {
        int idx = VPN_MASK(va, level);
        if (!(pt[idx] & PTE_V)) return 0;
        pt = (pagetable_t)(PTE_PA(pt[idx]));
    }
    return &pt[VPN_MASK(va, 0)];
}

// 建立单页映射
int map_page(pagetable_t pt, uint64_t va, uint64_t pa, int perm) {
    if ((va & (PAGE_SIZE-1)) || (pa & (PAGE_SIZE-1))) return -1; // 必须页对齐
    pte_t *pte = walk_create(pt, va);
    if (!pte) return -1;
    if (*pte & PTE_V) return -2; // 已映射
    *pte = PA2PTE(pa) | perm | PTE_V;
    return 0;
}

// 递归释放所有页表
void destroy_pagetable(pagetable_t pt) {
    for (int i = 0; i < 512; i++) {
        if ((pt[i] & PTE_V) && !(pt[i] & (PTE_R|PTE_W|PTE_X))) {
            pagetable_t child = (pagetable_t)PTE_PA(pt[i]);
            destroy_pagetable(child);
        }
    }
    free_page(pt);
}

// 打印页表内容（递归）
void dump_pagetable(pagetable_t pt, int level) {
    for (int i = 0; i < 512; i++) {
        if (pt[i] & PTE_V) {
            for (int l = 0; l < level; l++) printf("  ");
            printf("L%d[%d]: PTE=0x%lx PA=0x%lx perm=%c%c%c\n", level, i, pt[i], PTE_PA(pt[i]),
                (pt[i]&PTE_R)?'R':'-', (pt[i]&PTE_W)?'W':'-', (pt[i]&PTE_X)?'X':'-');
            if (!(pt[i] & (PTE_R|PTE_W|PTE_X))) {
                pagetable_t child = (pagetable_t)PTE_PA(pt[i]);
                dump_pagetable(child, level+1);
            }
        }
    }
}
