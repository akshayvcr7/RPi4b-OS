#include "heap.h"
#include "reg.h"

#include <stdio.h>

extern uint8_t __end;


struct heap_table kernel_heap_table;

void heap_init () {
    kernel_heap_table.total = HEAP_MAX_SIZE/HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = &__end;
    kernel_heap_table.heap_data_addr = &__end + (uint32_t)0x8000U; // kernel_end +32K

    kheap_create(&kernel_heap_table, HEAP_MAX_SIZE);
}

void kheap_create(struct heap_table* kernel_heap_table, size_t heap_size) {

    memset(kernel_heap_table->entries, 0U,  kernel_heap_table->total); //table_entries are free


}

uint32_t get_aligned_heap_block_len(size_t size) {

    uint32_t len = size - (size % HEAP_BLOCK_SIZE);
    if ((size % HEAP_BLOCK_SIZE)!= 0U) {
        len = len + HEAP_BLOCK_SIZE;
    }

    len = len / HEAP_BLOCK_SIZE;

    return len;
}

int kheap_malloc(struct heap_table* kernel_heap_table, uint32_t aligned_block_size) {

    int len = aligned_block_size;
    int start_block = 0U;
    int ret = -1;

    for (int i=0; i<kernel_heap_table->total; i++)
    {
        if ((kernel_heap_table->entries[i] & 0xFU) == HEAP_ENTRY_TYPE_TAKEN ) {
            continue;
        }
        else {
            start_block = i;
            /*while(len--) {
                if ((kernel_heap_table->entries[i] & 0xFU) == HEAP_ENTRY_TYPE_TAKEN ) {
                    continue;
                }
                i++;
            }*/
            break;
        }
    }

    kprintf("start_block is 0x%x heap_data_addr is 0x%x\n", start_block, kernel_heap_table->heap_data_addr);
    ret = kernel_heap_table->heap_data_addr + (start_block * HEAP_BLOCK_SIZE);

    // mark blocks as taken
    while(len--) {
        kernel_heap_table->entries[start_block++] = HEAP_ENTRY_TYPE_TAKEN;
    }

    return ret;
}

void* kmalloc(size_t size) {

    if ((size == 0U) || (size > HEAP_MAX_SIZE)) {
        return -1;
    }

    uint32_t aligned_block_size = get_aligned_heap_block_len(size);
    kprintf("heap alloted block_size=0x%x\n", aligned_block_size);

    return (kheap_malloc(&kernel_heap_table, aligned_block_size));

}