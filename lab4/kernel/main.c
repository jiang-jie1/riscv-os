#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "vm.h"
#include "pmm.h"
extern int interrupt_count;

void main()
{
    // 初始化 UART
    uart_init();
    pmm_init();
    kvminit();
    printf("Kernel initialized.\n");
    // 测试中断处理
    trapinithart();
    test_timer_interrupt();
    test_exception_handling();
    test_interrupt_overhead();
    printf("All tests completed.\n");
    while(1)
        ;
}