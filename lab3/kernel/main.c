#include "defs.h"
#include "pmm.h"
#include "vm.h"
char stack0[4096];
void main(void)
{
    // 初始化 UART
    uart_init();
    printf("Screen cleared.\n");
    // 清屏
    clear_screen();
    // 测试 printf
    printf("Hello OS! This is a test of printf.\n");
    printf("=====================================\n");
    printf("Testing integer: %d\n", 42);
    printf("Testing negative: %d\n", -2147483648);
    printf("Testing zero: %d\n", 0);
    printf("Testing hex: 0x%x\n", 0xABC);
    printf("Testing pointer: %p\n", (void*)0x80000000);
    printf("Testing string: %s\n", "This is a test string.");
    printf("Testing char: %c\n", 'X');
    printf("Testing percent: %%\n");
    printf("NULL string: %s\n", (char*)0);
    printf("Empty string: %s\n", "");
    printf("=====================================\n");
    char buf[100];
    int num = 123;
    
    // 使用printf直接输出到屏幕
    printf("直接输出：%d\n", num);  // 结果：直接输出：123
    
    // 使用sprintf写入到buf数组
    sprintf(buf, "写入缓冲区：%d", num);  // buf中现在存储"写入缓冲区：123"
    
    // 可以通过printf输出缓冲区内容
    //printf("%s\n", buf);  // 结果：写入缓冲区：123

    // ====== 物理内存分配器测试 ======
    printf("free pages: %d\n", pmm_free_pages());
    pmm_init();
    printf("free pages: %d\n", pmm_free_pages());
    void *page1 = alloc_page();
    void *page2 = alloc_page();
    printf("alloc_page 1: %p\n", page1);
    printf("alloc_page 2: %p\n", page2);
    printf("free pages: %d\n", pmm_free_pages());
    if (page1 && page2 && page1 != page2) {
        *(int*)page1 = 0x12345678;
        printf("write/read test: %x\n", *(int*)page1);
    }
    free_page(page1);
    free_page(page2);
    void *page3 = alloc_page();
    printf("alloc_page 3 (should reuse): %p\n", page3);
    free_page(page3);
    printf("pmm test done.\n");
    printf("free pages after test: %d\n", pmm_free_pages());//正确
    // ====== 页表管理系统测试 ======
    pagetable_t pt = create_pagetable();
    uint64_t va = 0x1000000;
    void *pa = alloc_page();
    printf("map_page: va=0x%lx pa=%p\n", va, pa);
    int ret = map_page(pt, va, (uint64_t)pa, PTE_R|PTE_W);
    printf("map_page ret=%d\n", ret);
    pte_t *pte = walk_lookup(pt, va);
    if (pte && (*pte & PTE_V)) {
        printf("walk_lookup: PTE=0x%lx, PA=0x%lx\n", *pte, PTE_PA(*pte));
        if (*pte & PTE_R) printf("perm: R ");
        if (*pte & PTE_W) printf("perm: W ");
        if (*pte & PTE_X) printf("perm: X ");
        printf("\n");
    } else {
        printf("walk_lookup failed!\n");
    }
    printf("dump_pagetable:\n");
    dump_pagetable(pt, 0);
    destroy_pagetable(pt);
    free_page(pa);
    printf("vm test done.\n");

    // ====== 虚拟内存激活测试 ======
    printf("Before enabling paging...\n");
    kvminit();
    kvminithart();
    printf("After enabling paging...\n");
    uart_puts("hello os\n");
    // 这里可继续测试内核代码/数据/设备访问
    printf("paging enabled test done.\n");
}