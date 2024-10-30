#include "pl011_uart.h"
#include "mbox.h"


void kernel_main(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
    volatile uint32_t mbox[36];
    uart_init();
    kprintf("piAV r0=0x%x r1= 0x%x, r2=0x%x, r3=0x%x\n", r0, r1, r2, r3);
    uint32_t *addr = 0x00000100;
    //kprintf("Hello world piAV OS %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", *(addr+0), *(addr+4), *(addr+8), *(addr+12), *(addr+16), *(addr+20), *(addr+24), *(addr+28), *(addr+32), *(addr+36));

    mbox[0] = 8*4;          // length of mailbox includes header, tag, buffer etc
    mbox[1] = 0x0U;         // request code

    mbox[2] = 0x00010005;   // Get ARM memory
    mbox[3] = 8;            // total buffer size
    mbox[4] = 8;
    mbox[5] = 0;
    mbox[6] = 0;            // clear output buffer
    mbox[7] = 0;            // LAST TAG

    // message to VC requesting to Get ARM memory
    if (mbox_vc_2_arm(8U, mbox)) {
        kprintf("arm memory is: %x / %x\n",mbox[5], mbox[6]);
    } else {
        kprintf("Mail box error!\n");
    }

    while (1);
}