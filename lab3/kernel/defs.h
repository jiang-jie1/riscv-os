#ifndef DEFS_H
#define DEFS_H
#include <stdarg.h>
// uart.c
void uart_init(void);
void uart_putc(char c);
void uart_puts(char *s);

// console.c
void consputc(char c);
void clear_screen(void);
void goto_xy(int x, int y);
void printf_color(int color, const char *fmt, ...);
void clear_line(void);

// printf.c
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list ap);

// vm_boot.c
void kvminit(void);
void kvminithart(void);

// main.c
void main(void);

#endif