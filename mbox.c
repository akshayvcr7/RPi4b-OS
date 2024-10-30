#include "reg.h"
#include <stdbool.h>
#include "mbox.h"

bool mbox_vc_2_arm(uint8_t ch, uint32_t* mbox)
{
    uint32_t r = ((((uint32_t)mbox) & ~0xFU) | (ch & 0xFU)); /* [4-0] bit for property channel */
    /* wait for mailbox_status and write to the mailbox_write reg */
    do{asm volatile("nop");}while(mmio_read(MBOX_STATUS) & MBOX_FULL);

    mmio_write(MBOX_WRITE, r);
    /* wait for the response */
    while(1) {
        do{asm volatile("nop");}while(mmio_read(MBOX_STATUS) & MBOX_EMPTY);

        if((r == mmio_read(MBOX_READ)) & ((mbox[4] & MBOX_RESPONSE) == MBOX_RESPONSE))
            /* check for response set */
            return (mbox[1] == MBOX_RESPONSE);
    }
    return 0;
}