#ifndef REG_H
#define REG_H
#include <stdint.h>

#define PERIPHERAL_BASE 0xFE000000
#define UART0_BASE 0xFE201000

inline volatile uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t *)(reg);
}

inline void mmio_write(const uint32_t reg, uint32_t val)
{
    *(volatile uint32_t *)(reg) = val;
}

#endif