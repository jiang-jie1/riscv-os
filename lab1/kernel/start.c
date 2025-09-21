#include "types.h"
#include "defs.h"
#include "riscv.h"
void main();
char stack0[4096*8];

void start()
{
    //jump to main
    //M到S模式：设置mret返回值的模式位为s模式；设置mret的返回地址；关闭地址转换和保护；在s模式下，代理所有中断和异常；允许s模式访问所有物理内存；时钟中断初始化；执行mret指令
    unsigned long x = r_mstatus();//读取mstatus寄存器的值,在riscv.h中定义
    x &= ~MSTATUS_MPP_MASK;//清空之前模式
    x |= MSTATUS_MPP_S;//设置为s模式
    w_mstatus(x);//写入mstatus寄存器
    
    w_satp(0);//关闭地址转换和保护

    w_mepc((uint64)main);//设置mret的返回地址为main函数的地址

    w_medeleg(0xffff);//在s模式下，代理所有中断和异常
    w_mideleg(0xffff);

    w_sie(r_sie() | SIE_SEIE | SIE_STIE);//开启s模式的时钟中断

    w_pmpaddr0(0x3fffffffffffffull);//允许s模式访问所有物理内存
    w_pmpcfg0(0xf);//配置pmpcfg0寄存器

    //时钟中断初始化
    

    int id = r_mhartid();//读取当前CPU的id
    w_tp(id);//将id写入tp寄存器
    asm volatile("mret");
}