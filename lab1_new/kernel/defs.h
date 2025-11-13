#ifndef DEFS_H
#define DEFS_H

// uart.c
void uart_init(void);
void uart_putc(char c);
void uart_puts(char *s);

// main.c
void main(void);

#endif