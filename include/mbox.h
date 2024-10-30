#ifndef MBOX_H
#define MBOX_H

#include "reg.h"
#include <stdbool.h>

#define VIDEOCORE_MBOX  (PERIPHERAL_BASE+0x0000B880)
#define MBOX_READ       (VIDEOCORE_MBOX+0x0)
#define MBOX_POLL       (VIDEOCORE_MBOX+0x10)
#define MBOX_SENDER     (VIDEOCORE_MBOX+0x14)
#define MBOX_STATUS     (VIDEOCORE_MBOX+0x18)
#define MBOX_CONFIG     (VIDEOCORE_MBOX+0x1C)
#define MBOX_WRITE      (VIDEOCORE_MBOX+0x20)
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

bool mbox_vc_2_arm(uint8_t ch, uint32_t* mbox);

#endif