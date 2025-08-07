#ifdef MMU_ENABLE
#include "mmu_uart.h"
#include "reg.h"
#include "scheduler.h"

extern unsigned long pgd_map;
void uart_puts(const char *data);



void create_table_entry(uint64_t tbl, uint64_t next_tbl, uint64_t va, uint64_t shift, uint64_t flags) {
    uint64_t table_index = va >> shift;
    table_index &= (ENTRIES_PER_TABLE - 1);
    //kprintf("table_index = 0x%x\n", table_index);
    uint64_t descriptor = next_tbl | flags;
    *((uint64_t *)(tbl + (table_index << 3))) = descriptor;
}

void create_block_map(uint64_t pmd, uint64_t vstart, uint64_t vend, uint64_t pa) {

    vstart >>= SECTION_SHIFT;
    vstart &= (ENTRIES_PER_TABLE -1);
    //kprintf("vstart = 0x%x\n", vstart);

    vend >>= SECTION_SHIFT;
    vend--;
    vend &= (ENTRIES_PER_TABLE - 1);
    //kprintf("vend = 0x%x\n", vend);

    pa >>= SECTION_SHIFT;
    pa <<= SECTION_SHIFT;
    //kprintf("pa = 0x%x\n", pa);

    do {
        uint64_t _pa = pa;

        if (pa >= DEVICE_START) {
            _pa |= DESC_DEVICE_BLOCK_FLAGS;
        } else {
            _pa |= DESC_KERNEL_BLOCK_FLAGS;
        }

        *((uint64_t *)(pmd + (vstart << 3))) = _pa;
        pa += SECTION_SIZE;
        vstart++;
    } while(vstart <= vend);
}

uint64_t get_pgd();

#define UART_VIRT_ADDR 0x100000000UL

void mmu_init() {

    asm volatile ("msr tcr_el1, %0" :: "r"(TCR_VALUE));
    asm volatile ("msr mair_el1, %0" :: "r"(MAIR_VALUE));

    uint64_t pgd = get_pgd();
    memzero(pgd, PAGES_MAP_SIZE);

    uint64_t map_base = 0;
    uint64_t tbl = pgd;
    uint64_t next_tbl = tbl + PAGE_SIZE;

    create_table_entry(tbl, next_tbl, map_base, PGD_SHIFT, DESC_KERNEL_TABLE_FLAGS);

    tbl += PAGE_SIZE;
    next_tbl += PAGE_SIZE;

    uint64_t block_tbl = tbl;

    /* Identity mapping of 4GB*/
    for (uint64_t i=0; i<4; i++) {
        create_table_entry(tbl, next_tbl, map_base, PUD_SHIFT, DESC_KERNEL_TABLE_FLAGS);

        next_tbl += PAGE_SIZE;
        map_base += PUD_ENTRY_MAP_SIZE;

        block_tbl += PAGE_SIZE;

        uint64_t offset = BLOCK_SIZE * i;
        create_block_map(block_tbl, offset, offset + BLOCK_SIZE, offset);
    }

    /* UART MAP to virtual addrerss 0x100000000*/
    create_table_entry(tbl, next_tbl, UART_VIRT_ADDR, PUD_SHIFT, DESC_KERNEL_TABLE_FLAGS);
    block_tbl += PAGE_SIZE;
    //kprintf("blk =0x%x and next tbl=0x%x\n", block_tbl, next_tbl);
    create_block_map(block_tbl, UART_VIRT_ADDR, UART_VIRT_ADDR + SECTION_SIZE, UART0_BASE);

    asm volatile ("msr ttbr0_el1, %0" :: "r" ((unsigned long)&pgd_map));

    asm volatile ("dsb ish");
    asm volatile ("isb");

    uint64_t sctlr;
    asm volatile ("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1 << 0);
    asm volatile ("msr sctlr_el1, %0" :: "r"(sctlr));
    asm volatile ("isb");
}

#define UART_DR 0x1000
#define UART_FR 0x1018

int slen(const char* ptr)
{
    int i = 0;
    while(*ptr != 0)
    {
        i++;
        ptr += 1;
    }

    return i;
}

static inline void uart_write(uint32_t offset, uint32_t val) {
    volatile uint32_t *addr = (volatile uint32_t *)(uintptr_t)(UART_VIRT_ADDR + offset);
    *addr = val;
}

static inline uint32_t uart_read(uint32_t offset) {
    volatile uint32_t *addr = (volatile uint32_t *)(uintptr_t)(UART_VIRT_ADDR + offset);
    return *addr;
}

static void tx_complete(void)
{
    while (((uart_read(UART_FR) >> 3U) & 1U) != 0) {}
}

void uart_puts(const char *data)
{
    size_t len = slen(data);

    // make sure that there is no outstanding transfer just in case
    tx_complete();

    for (size_t i = 0; i < len; ++i) {
        if (data[i] == '\n') {
            uart_write(UART_DR, '\r');
            tx_complete();
        }
        uart_write(UART_DR, data[i]);
        tx_complete();
    }
}

#endif
