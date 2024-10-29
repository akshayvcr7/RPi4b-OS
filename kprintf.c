#include <stdarg.h>
#include <stdint.h>
#include <pl011_uart.h>

void (*putf) (const char*);

static void uint_2_ascii(uint32_t num, uint8_t base, char* bf)
{
    uint8_t n = 0x0U;
    uint32_t d = 0x1U;
    while (num/d >= base)
        d*= base;
    while (d != 0x0U) {
        uint8_t dgt = num / d;
        num %= d;
        d/= base;
        if (n || dgt > 0x0U || d == 0x0U) {
            *bf++ = dgt + (dgt < 10U ? '0' : ('a'-10));
            ++n;
        }
    }
    *bf=0;
}

static void int_2_ascii (uint32_t num, char* bf)
{
    if (num<0) {
        num = -num;
        *bf++ = '-';
    }
    uint_2_ascii(num, 10U, bf);
}

static void putch(char* bf)
{
    char* ch;
    while ((*ch= *bf++))
    putf(ch);
}


void p_f(char *fmt, va_list va)
{
    char bf[10];
    char* ch;

    while ((*ch = *(fmt++))) {
        if (*ch != '%')
            putf(ch);
        else {
            *ch = *(fmt++);
            if (*ch == '0') {
                *ch = *(fmt++);
            }
            switch (*ch) {
                case 0:
                    goto fail;
                case 'd' :
                    int_2_ascii(va_arg(va, uint32_t), bf);
                    putch(bf);
                    break;
                case 'x': case 'X' :
                    uint_2_ascii(va_arg(va, uint32_t), 16U, bf);
                    putch(bf);
                    break;
                case 'u' :
                    uint_2_ascii(va_arg(va, uint32_t), 10U, bf);
                    putch(bf);
                    break;
                case 'c' :
                    putf((char)(va_arg(va, int)));
                    break;
                case 's' :
                    putch(va_arg(va, char*));
                    break;
                case '%' :
                    putf(ch);
                default:
                    break;
            }
        }
    }
fail:
    return;
}

void kprintf(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    putf = &pl011_uart_puts;
    p_f(fmt, va);
    va_end(va);
}