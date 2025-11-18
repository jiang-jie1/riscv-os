#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "vm.h"
#include "pmm.h"
#include "memlayout.h"
/*
void trapinithart();
void kerneltrap();
int devintr();
*/
// 中断处理函数类型定义
typedef void (*interrupt_handler_t)(void);
// 中断优先级定义
#define IRQ_PRIORITY_NONE     0
#define IRQ_PRIORITY_LOW      1
#define IRQ_PRIORITY_NORMAL   2
#define IRQ_PRIORITY_HIGH     3
// 中断向量表 - 存储所有中断处理函数指针
static interrupt_handler_t interrupt_handlers[64] = {0};
// 中断优先级表
static int irq_priorities[64] = {
    [1] = IRQ_PRIORITY_LOW,
    [5]    = IRQ_PRIORITY_NORMAL,
    [9] = IRQ_PRIORITY_HIGH,
};
// 嵌套中断管理
static int nested_level = 0;
static int current_priority = IRQ_PRIORITY_NONE;

// 系统时间变量
volatile uint64 ticks = 0;

// 测试用中断计数器
volatile int interrupt_count = 0;
volatile int software_interrupt_count = 0;
volatile int external_interrupt_count = 0;
// 注册中断处理函数
void register_interrupt(int irq, interrupt_handler_t handler)
{
    if (irq >= 0 && irq < 64) {
        interrupt_handlers[irq] = handler;
        //printf("register_interrupt: 已注册IRQ %d 的处理函数\n", irq);
    }
}
// 注销中断处理函数
void unregister_interrupt(int irq)
{
    if (irq >= 0 && irq < 64) {
        interrupt_handlers[irq] = 0;
        //printf("unregister_interrupt: 已注销IRQ %d 的处理函数\n", irq);
    }
}
// 启用特定中断
void enable_interrupt(int irq)
{
    switch (irq) {
    case 1:
        w_sie(r_sie() | SIE_SSIE);
        //printf("enable_interrupt: 已启用软件中断\n");
        break;
    case 5:
        w_sie(r_sie() | SIE_STIE);
        //printf("enable_interrupt: 已启用时钟中断\n");
        break;
    case 9:
        w_sie(r_sie() | SIE_SEIE);
        //printf("enable_interrupt: 已启用外部中断\n");
        break;
    default:
        printf("enable_interrupt: 未知中断号 %d\n", irq);
    }
}

// 关闭特定中断
void disable_interrupt(int irq)
{
    switch (irq) {
    case 1:
        w_sie(r_sie() & ~SIE_SSIE);
        //printf("disable_interrupt: 已关闭软件中断\n");
        break;
    case 5:
        w_sie(r_sie() & ~SIE_STIE);
        //printf("disable_interrupt: 已关闭时钟中断\n");
        break;
    case 9:
        w_sie(r_sie() & ~SIE_SEIE);
        //printf("disable_interrupt: 已关闭外部中断\n");
        break;
    default:
        printf("disable_interrupt: 未知中断号 %d\n", irq);
    }
}
// 获取嵌套层级
int get_nested_level(void)
{
    return nested_level;
}
// 获取当前优先级
int get_current_priority(void)
{
    return current_priority;
}
// 获取当前时间（mtime）
uint64 get_time(void) {
    return r_time();
}
// 设置中断优先级
void set_interrupt_priority(int irq, int priority)
{
    if (irq >= 0 && irq < 64 && priority >= IRQ_PRIORITY_LOW && priority <= IRQ_PRIORITY_HIGH) {
        irq_priorities[irq] = priority;
        //printf("set_interrupt_priority: IRQ %d 优先级设置为 %d\n", irq, priority);
    }
}

// 获取中断优先级
int get_interrupt_priority(int irq)
{
    if (irq >= 0 && irq < 64) {
        return irq_priorities[irq];
    }
    return IRQ_PRIORITY_LOW;
}
// 中断处理链，支持优先级和嵌套
void handle_interrupt_chain(int irq)
{
    if (!interrupt_handlers[irq]) {
        return;
    }

    // 获取当前中断的优先级
    int irq_priority = get_interrupt_priority(irq);
    
    // 检查是否允许嵌套：只有更高优先级的中断才能嵌套
    if (irq_priority <= current_priority && nested_level > 0) {
        //printf("handle_interrupt_chain: IRQ %d (优先级 %d) 被当前 IRQ (优先级 %d) 阻塞\n", 
               //irq, irq_priority, current_priority);
        return;
    }

    // 保存旧状态
    int old_priority = current_priority;
    
    // 更新当前状态
    current_priority = irq_priority;
    nested_level++;

    //printf("handle_interrupt_chain: 开始处理 IRQ %d, 优先级 %d, 嵌套层级 %d\n", 
           //irq, irq_priority, nested_level);

    // 禁用当前IRQ以防止重复中断，但允许全局中断以实现嵌套
    disable_interrupt(irq);
    
    // 启用全局中断以允许更高优先级的中断嵌套
    intr_on();

    // 执行中断处理函数
    interrupt_handlers[irq]();

    // 恢复中断状态
    intr_off();
    enable_interrupt(irq);
    
    // 恢复优先级
    current_priority = old_priority;
    nested_level--;

    //printf("handle_interrupt_chain: 完成处理 IRQ %d, 恢复优先级 %d, 嵌套层级 %d\n", 
           //irq, current_priority, nested_level);
}
// 软件中断处理函数
void software_interrupt_handler(void)
{
    software_interrupt_count++;
    printf("software_interrupt_handler: 第 %d 次软件中断，嵌套层级 %d\n", 
           software_interrupt_count, nested_level);
    
    // 清除软件中断挂起位
    w_sip(r_sip() & ~(1 << 1));
}
void software_interrupt_handler2(void)
{
    software_interrupt_count++;
    printf("software_interrupt_handler: 第 %d 次软件中断，嵌套层级 %d\n", 
           software_interrupt_count, nested_level);

    // 在软件中断处理期间主动触发时钟中断，实现嵌套测试
    uint64 current_time = get_time();
    uint64 next_time = current_time + 1000; // 触发时钟中断
    w_stimecmp(next_time);
    printf("software_interrupt_handler: 在处理中嵌套触发时钟中断\n");

    // 在软件中断处理期间尝试主动触发低优先级中断
   handle_interrupt_chain(10);
    printf("software_interrupt_handler: 在处理中嵌套触发低优先级中断\n");

    // 清除软件中断挂起位
    w_sip(r_sip() & ~(1 << 1));
}
// 时钟中断处理函数
void timer_interrupt(void) {
    // 1. 更新系统时间
    ticks++;
    interrupt_count++;
    // printf("timer_interrupt: 第 %d 次时钟中断，嵌套层级 %d，系统时间 ticks=%d\n", 
    //        interrupt_count, nested_level, ticks);

    // 2. 处理定时器事件（如进程时间片等，暂略）

    // 3. 触发任务调度（如 yield/scheduler，暂略）

    // 4. 设置下次中断时间
    w_stimecmp(get_time() + 1000000); // 0.1s后
    // 清除中断挂起位
    w_sip(r_sip() & ~(1 << 5));
}
// 外部中断处理函数
void external_interrupt_handler(void)
{
    external_interrupt_count++;
    printf("external_interrupt_handler: 第 %d 次外部中断，嵌套层级 %d\n", 
           external_interrupt_count, nested_level);    
    // 注意：实际的外部中断需要查询PLIC来确定具体设备
    // 这里简化处理，假设只有一个外部中断源
    
    // 清除外部中断挂起位
    w_sip(r_sip() & ~(1 << 9));
}
// 设置stvec寄存器
void trapinithart()
{
    w_stvec((uint64)kernelvec);
    intr_on();
}
void handle_illegal_instruction(void) {
    uint64 sepc = r_sepc();
    uint64 stval = r_stval();
    printf("非法指令异常:\n");
    printf("  sepc = 0x%lx\n", sepc);
    printf("  stval = 0x%lx\n", stval);
    w_sepc(sepc + 4);
    //panic("异常: 非法指令");
}

void handle_breakpoint_exception(void) {
    uint64 sepc = r_sepc();
    uint64 stval = r_stval();
    printf("断点异常:\n");
    printf("  sepc = 0x%lx\n", sepc);
    printf("  stval = 0x%lx\n", stval);
    panic("异常: 断点");
}

void handle_instruction_page_fault() {
    uint64 sepc = r_sepc();
    uint64 stval = r_stval();

    printf("指令页故障: sepc=0x%lx, stval=0x%lx\n", sepc, stval);
    panic("exception: 指令页故障，非法访问");
}

void handle_data_page_fault(uint64 cause) {
    uint64 sepc = r_sepc();
    uint64 stval = r_stval();
    const char *fault_type = (cause == 13) ? "加载" : "存储";
    printf("%s页故障: sepc=0x%lx, stval=0x%lx\n", fault_type, sepc, stval);

    extern pagetable_t kernel_pagetable;
    extern char etext[];
    uint64 va = stval & ~(PGSIZE - 1);

    if (stval >= (uint64)etext && stval < PHYSTOP) {
        void *pa = alloc_page();
        if (pa == 0) panic("分配物理页失败");
        if (map_page(kernel_pagetable, va, (uint64)pa, PTE_R | PTE_W) != 0)
            panic("建立映射失败");
        printf("%s页缺页已分配并映射 va=0x%lx -> pa=0x%lx\n", fault_type, va, (uint64)pa);
        return;
    }
    if(cause==13)
        panic("exception: 加载页故障，非法访问");
    else
        panic("exception: 存储页故障，非法访问");
}
// 处理异常
void handle_exception() {
    uint64 cause = r_scause();
    switch (cause) {
    case 2:  // 非法指令
        handle_illegal_instruction();
        break;
    case 3:  // 断点
        handle_breakpoint_exception();
        break;
    case 8:  // 系统调用
        panic("异常: 系统调用\n");
        break;
    case 12: // 指令页故障
        handle_instruction_page_fault();
        break;
    case 13: // 加载页故障
    case 15: // 存储页故障
        handle_data_page_fault(cause);
        break;
    default:
        panic("Unknown exception");
        break;
    }
}
// s模式下中断入口
void kerneltrap()
{
    uint64 scause = r_scause();
   // 中断处理
    if(scause & (1ULL << 63)) {
        uint64 interrupt_code = scause & ~(1ULL << 63);
        
        if(interrupt_handlers[interrupt_code]) {
            //interrupt_handlers[interrupt_code]();
            handle_interrupt_chain(interrupt_code);

            if(interrupt_code == 5) {
               //进程
            }
        } else {
            printf("未知中断: %d\n", interrupt_code);
        }
    } else {
        // 异常处理
        handle_exception();
    }
}

// 获得中断
int devintr()
{
    uint64 scause = r_scause();

    // 新版本
    // 1.... 101 就是5 Supervisor timer interrupt
    //           0x8000000000000005L
    if(scause == 0x8000000000000005L)
    {
        // ask for the next timer interrupt. this also clears
        // the interrupt request. 1000000 is about a tenth
        // of a second.
        //w_stimecmp(r_time() + 1000000);  // 0.1s
        timer_interrupt();
        return 2;
    }

    return 0;
}
// 测试函数
void test_timer_interrupt(void)
{
    printf("测试时钟中断...\n");
    
    // 记录中断前的时间
    uint64 start_time = get_time();
    int initial_count = interrupt_count;
    
    // 注册时钟中断处理函数
    register_interrupt(5, timer_interrupt);
    enable_interrupt(5);
    
    printf("sie = 0x%lx, sip = 0x%lx\n", r_sie(), r_sip());

    uint64 current_time = get_time();
    uint64 first_interrupt_time = current_time + 1000000; 
    w_stimecmp(first_interrupt_time);
    printf("设置第一次中断时间: current=%d, first=%d\n", current_time, first_interrupt_time);

    printf("等待5次中断...\n");
    
    // 等待几次中断
    while (interrupt_count < initial_count + 5) {
        // 可以在这里执行其他任务
        printf("等待中断 %d... 当前时间: %d\n", interrupt_count - initial_count + 1, get_time());
        // 简单延时
        for (volatile int i = 0; i < 1000000; i++);
    }
    
    uint64 end_time = get_time();
    printf("时钟中断测试完成: %d 次中断，用时 %d 周期\n", 
           interrupt_count - initial_count, end_time - start_time);
    
    // 清理
    disable_interrupt(5);
}
// 软件中断测试
void test_software_interrupt(void)
{
    printf("测试软件中断...\n");
    
    // 注册软件中断处理函数
    register_interrupt(1, software_interrupt_handler);
    enable_interrupt(1);
    
    int initial_count = software_interrupt_count;
    
    printf("触发软件中断...\n");
    
    // 手动设置软件中断挂起位来触发中断
    for (int i = 0; i < 3; i++) {
        w_sip(r_sip() | (1 << 1));
        printf("已设置软件中断挂起位，等待中断处理...\n");
        
        // 短暂延时，让中断处理完成
        for (volatile int j = 0; j < 100000; j++);
    }
    
    printf("软件中断测试完成: 触发 %d 次，处理 %d 次\n", 
           3, software_interrupt_count - initial_count);
    
    // 清理
    disable_interrupt(1);
}
void test_exception_handling(void) { 
    printf("Testing exception handling...\n"); 

    // 1. 触发指令页故障
   /* extern pagetable_t kernel_pagetable;
    extern char etext[]; // 内核代码结束
    uint64 va = (uint64)etext + 0x10000;
    while (walk_lookup(kernel_pagetable, va) && va < PHYSTOP) {
        va += PGSIZE;
    }
    void (*bad_func)() = (void (*)())va;
    printf("即将跳转到未映射地址: 0x%lx\n", va);
    bad_func();*/

    
    //2. 加载页和存储页异常
    extern pagetable_t kernel_pagetable;
    extern char etext[];
    uint64 va = (uint64)etext + 0x20000;
    // 找到一个未映射的虚拟地址
    while (walk_lookup(kernel_pagetable, va) && va < PHYSTOP) {
        va += PGSIZE;
    }

    printf("即将触发加载页异常，地址: 0x%lx\n", va);
    volatile uint64 val = *(volatile uint64 *)va; // 触发加载页异常
    printf("加载页异常已处理，读取值: 0x%lx\n", val);

    printf("即将触发存储页异常，地址: 0x%lx\n", va + PGSIZE);
    *(volatile uint64 *)(va + PGSIZE) = 0x12345678; // 触发存储页异常
    printf("存储页异常已处理，写入完成\n");

    // 3. 非法指令
    asm volatile(".word 0x0000000b"); // 制造一个非法指令

    // 4. 断点（ebreak 指令）
    //asm volatile("ebreak");

    printf("Exception tests completed\n"); 
    disable_interrupt(5);
}
volatile uint64 ctx_switch_start = 0, ctx_switch_end = 0;
    void ctx_switch_handler(void) {
        ctx_switch_end = get_time();
        w_sip(r_sip() & ~(1 << 1));
    }
void test_interrupt_overhead(void) {
    printf("测试中断处理开销...\n");

    // 1. 测量中断处理的时间开销
    register_interrupt(5, timer_interrupt);
    enable_interrupt(5);

    uint64 interval = 10000;
    int test_count = 100;
    interrupt_count = 0;
    uint64 start_time = get_time();
    w_stimecmp(start_time + interval);

    while (interrupt_count < test_count) {
        // 等待中断
    }
    uint64 end_time = get_time();
    uint64 total_cycles = end_time - start_time;
    uint64 avg_cycles = total_cycles / test_count;
    printf("[1] 中断次数: %d\n", test_count);
    printf("[1] 总用时: %d 周期\n", total_cycles);
    printf("[1] 平均每次中断处理用时: %d 周期\n", avg_cycles);

    disable_interrupt(5);

    // 2. 测量上下文切换的成本（模拟：主循环和中断处理函数各读一次时间戳）
    register_interrupt(1, ctx_switch_handler);
    enable_interrupt(1);

    ctx_switch_start = get_time();
    w_sip(r_sip() | (1 << 1)); // 触发软件中断
    while (ctx_switch_end == 0); // 等待中断处理完成
    printf("[2] 上下文切换成本: %d 周期\n", ctx_switch_end - ctx_switch_start);

    disable_interrupt(1);

    // 3. 分析中断频率对系统性能的影响
    uint64 intervals[] = {100000, 10000, 1000};
    for (int i = 0; i < 3; i++) {
        interval = intervals[i];
        interrupt_count = 0;
        register_interrupt(5, timer_interrupt);
        enable_interrupt(5);

        start_time = get_time();
        w_stimecmp(start_time + interval);

        while (interrupt_count < test_count) {
            // 等待中断
        }
        end_time = get_time();
        total_cycles = end_time - start_time;
        avg_cycles = total_cycles / test_count;
        printf("[3] interval=%d: 总用时=%d, 平均每次中断=%d\n", interval, total_cycles, avg_cycles);

        disable_interrupt(5);
    }

    printf("中断处理性能测试完成。\n");
}
