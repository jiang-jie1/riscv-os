#include "memlayout.h"
#define Reg(reg) ((volatile unsigned char *)(UART0 + (reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))
#define ReadReg(reg) (*(Reg(reg)))
// the UART control registers.
// some have different meanings for
// read vs write.
// see http://byterunner.com/16550.html
#define RHR 0                 // 接收保持寄存器（用于存放接收到的字节）
#define THR 0                 // 发送保持寄存器（用于存放待发送的字节）
#define IER 1                 // 中断使能寄存器
#define IER_RX_ENABLE (1<<0)  // 接收中断使能（置1时允许接收中断）
#define IER_TX_ENABLE (1<<1)  // 发送中断使能（置1时允许发送中断）
#define FCR 2                 // FIFO控制寄存器
#define FCR_FIFO_ENABLE (1<<0)// FIFO使能（置1时启用发送/接收FIFO缓冲区）
#define FCR_FIFO_CLEAR (3<<1) // 清除FIFO（置位时清空发送和接收FIFO的内容）
#define ISR 2                 // 中断状态寄存器
#define LCR 3                 // 线路控制寄存器
#define LCR_EIGHT_BITS (3<<0) // 8位数据位设置（配置为8位数据格式）
#define LCR_BAUD_LATCH (1<<7) // 波特率锁存位（置1时进入波特率设置模式）
#define LSR 5                 // 线路状态寄存器
#define LSR_RX_READY (1<<0)   // 接收就绪（置1时表示RHR中有数据可读取）
#define LSR_TX_IDLE (1<<5)    // 发送空闲（置1时表示THR可接收新的发送数据）

void uart_init(void)//串口初始化
{
    /* 1. 关闭中断 
     * 2. 设置波特率
     * 3. 设置8位宽度
     * 4. 清空输入和输出队列
     */
    WriteReg(IER, 0); // 关闭中断
    WriteReg(LCR, LCR_BAUD_LATCH); // 进入波特率设置模式
// UART 的波特率（数据传输速率）由芯片内部时钟和 “波特率除数” 共同决定，计算公式为：波特率 = 参考时钟频率 / (16 × 除数)
// 其中，“除数” 是一个 16 位的数值，由两个 8 位寄存器组成：
// DLL（Divisor Latch Low）：除数的低 8 位
// DLM（Divisor Latch High）：除数的高 8 位
    WriteReg(0, 3); // DLL
    WriteReg(0, 0); // DLM

    WriteReg(LCR, LCR_EIGHT_BITS); 

    WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);
}
void uart_putc(char c)
{
    // 等待发送保持寄存器空闲
    while((ReadReg(LSR) & LSR_TX_IDLE) == 0)
        ;
    WriteReg(THR, c);
}
void uart_puts(char *s)
{
    while(*s != '\0') {
        uart_putc(*s);
        s++;
    }
}