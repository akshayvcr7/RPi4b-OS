#include "pl011_uart.h"

void kernel_main()
{
    uart_init();
    kprintf("Hello world piAV OS %x\n", 20U);
    while (1);
}