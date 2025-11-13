#include "defs.h"
char stack0[4096];
//int x=42;
void main()
{
    uart_init();
    uart_puts("hello os\n");
    while (1) {
    }
}