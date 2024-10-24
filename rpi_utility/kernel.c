#include "pl011_uart.h"

void kernel_main()
{
  uint32_t kernel_len = 0U;
  char *kernel_load = (char*)0x80000;

  // setup uart for console o/p
  uart_init();

loop:
  pl011_uart_puts("\x03\x03\x03"); //Ctrl+C signal ack for the utility to send the kernel

  // read the kernel's kernel_len
  kernel_len = uart_recv();
  kernel_len |= uart_recv() << 8U;
  kernel_len |= uart_recv() << 16U;
  kernel_len |= uart_recv() << 24U;

  if ( kernel_len < 0U ) {
    // kernel_len error
    pl011_uart_puts("Kernel size ERROR!");
    uart_send('\n');
    goto loop;
  }

  uart_send('O');
  uart_send('K');
  //uart_send('\n');

  // read the kernel
  // In UART standards, the protocol follows little endianness, i.e., data is sent from LSB to MSB
  while(kernel_len--) *kernel_load++ = uart_recv();

    // restore x0
    asm volatile (
        "mov x0, x12;"
        // change LR
        "mov x30, 0x80000; ret"
    );
}