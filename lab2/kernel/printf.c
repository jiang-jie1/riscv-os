#include <stdarg.h>
#include "types.h"
#include "defs.h"

// 外部函数
void consputc(char c);
static char digits[] = "0123456789abcdef";
static void
bufputc(char **buf, char c)
{
    *(*buf)++ = c;
}
static void
buf_print_number(char **buf, long long num, int base, int sign)
{
    char tmp[64];
    int i = 0;

    if (sign && num < 0) {
        bufputc(buf, '-');
        num = -num;
    }

    if (num == 0) {
        bufputc(buf, '0');
        return;
    }

    while (num > 0) {
        tmp[i++] = digits[num % base];
        num /= base;
    }

    while (--i >= 0)
        bufputc(buf, tmp[i]);
}
int sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int i, c;
    char *s;
    char *p = buf;

    if (fmt == 0)
        return -1;

    va_start(ap, fmt);
    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            bufputc(&p, c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if (c == 0)
            break;
        switch (c) {
        case 'd':
            buf_print_number(&p, va_arg(ap, int), 10, 1);
            break;
        case 'x':
            buf_print_number(&p, va_arg(ap, int), 16, 0);
            break;
        case 'p':
            bufputc(&p, '0');
            bufputc(&p, 'x');
            buf_print_number(&p, va_arg(ap, unsigned long), 16, 0);
            break;
        case 's':
            if ((s = va_arg(ap, char *)) == 0)
                s = "(null)";
            while (*s)
                           bufputc(&p, *s++);
            break;
        case 'c':
            bufputc(&p, va_arg(ap, int));
            break;
        case '%':
            bufputc(&p, '%');
            break;
        default:
            bufputc(&p, '%');
            bufputc(&p, c);
            break;
        }
    }
    va_end(ap);

    *p = '\0'; // 结尾加上字符串结束符
    return p - buf;
}
static void
print_number(long long xx, int base, int sign)
{
  char buf[20];
  int i;
  unsigned long long x;

  if(sign && (sign = (xx < 0)))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}

// printf 实现
int
printf(const char *fmt, ...)
{
    va_list ap;
    int i, c;
    char *s;

    if (fmt == 0)
        return -1;
    // 初始化ap，指向第一个可变参数
    va_start(ap, fmt);
    // 循环条件：逐个读取字符直到遇到字符串结束符'\0'
    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        // 如果不是格式说明符起始字符'%'，则直接输出字符
        if (c != '%') {
            consputc(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if (c == 0)
            break;
        switch (c) {
        case 'd':
            print_number(va_arg(ap, int), 10, 1);// va_arg(ap, int): 从参数列表取出一个int参数// 10: 十进制基数// 1: 有符号数（处理负数）
            break;
        case 'x':
            print_number(va_arg(ap, int), 16, 0);
            break;
        case 'p':
            consputc('0');
            consputc('x');
            print_number(va_arg(ap, unsigned long), 16, 0);
            break;
        case 's':
            if ((s = va_arg(ap, char *)) == 0)
                s = "(null)";
            for (; *s; s++)
                consputc(*s);
            break;
        case 'c':
            consputc(va_arg(ap, int));
            break;
        case '%':
            consputc('%');
            break;
        default:
            // 打印一个'%'和未知的格式字符
            consputc('%');
            consputc(c);
            break;
        }
    }
    va_end(ap);

    return 0;
}
