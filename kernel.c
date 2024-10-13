#include "pl011_uart.h"

void kernel_main()
{
    uart_init();
    pl011_uart_puts("Hello world piAV OS!\n");
    while (1);
}