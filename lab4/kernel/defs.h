#ifndef DEFS_H
#define DEFS_H
#include <stdarg.h>
#include "types.h"

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
void panic(const char *s);

// vm_boot.c
void kvminit(void);
void kvminithart(void);

// main.c
void main(void);


//kernelvec.S
void kernelvec();

// trap.c
void trapinithart();
void kerneltrap();
int devintr();
uint64 get_time(void);
void test_timer_interrupt(void);
void test_exception_handling(void);
#endif