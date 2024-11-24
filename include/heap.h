#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_MAX_SIZE 0x8000000U //128MB
#define HEAP_BLOCK_SIZE 0x1000U

//   ___7_   ___6____  _5_ _4_  _3_   _2_   _1_   _0_
// | HAS_N | IS_FIRST | 0 | 0 | ET3 | ET2 | ET1 | ET0 |

#define HEAP_ENTRY_TYPE_TAKEN    0x01U
#define HEAP_ENTRY_TYPE_FREE     0x00U

#define HEAP_HAS_NEXT  1 << 7U
#define HEAP_IS_FIRST  1 << 6U


typedef uint8_t HEAP_TABLE_ENTRY;

struct heap_table
{
    HEAP_TABLE_ENTRY* entries;
    size_t total;
    uint32_t heap_data_addr;
};

#endif