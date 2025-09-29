#ifndef DEFS_H
#define DEFS_H

// uart.c
void uart_init(void);
void uart_putc(char c);
void uart_puts(char *s);

// console.c
void consputc(char c);
void clear_screen(void);
void goto_xy(int x, int y);
//void printf_color(int color, const char *fmt, ...);
void clear_line(void);

// printf.c
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);

// main.c
void main(void);

#endif