#ifndef REG_H
#define REG_H
#include <stdint.h>
#include <stddef.h>

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

//Supports only %d, %x, %u, %c, %s
void kprintf(char *fmt, ...);

void* memset(void* ptr, int c, size_t size);

int fork_process(unsigned long clone_flags, uintptr_t fn, uintptr_t arg, unsigned long stack);

#endif