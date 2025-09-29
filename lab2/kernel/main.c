#include "defs.h"
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
    printf("直接输出：%d", num);  // 结果：直接输出：123
    clear_line();
    printf("测试：这一行应该是清空后输出的内容\n");
    // 使用sprintf写入到buf数组
    sprintf(buf, "写入缓冲区：%d", num);  // buf中现在存储"写入缓冲区：123"
    
    // 可以通过printf输出缓冲区内容
    //printf("%s\n", buf);  // 结果：写入缓冲区：123
}