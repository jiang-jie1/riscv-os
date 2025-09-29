#include "types.h"
#include "defs.h"

// ANSI 转义序列
#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_CURSOR_HOME  "\033[H"

// 外部函数声明
void uart_putc(char c);
void uart_puts(char *s);
// 向控制台输出一个字符
void consputc(char c) {
    uart_putc(c);
}
void consputs(char *s) {
    uart_puts(s);
}
// 清除屏幕
void clear_screen(void) {
    char *p = "\033[2J\033[H";
    uart_puts(p);
}
//光标定位
void goto_xy(int x, int y)
{
  // ANSI 序列，x/y 从 1 开始
  printf("\033[%d;%dH", y, x);
}
void
clear_line()
{
  uart_puts("\033[K");
}