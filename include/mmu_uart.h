#ifndef _MMU_H
#define _MMU_H


#ifdef MMU_ENABLE
#define MM_TYPE_PAGE_TABLE		0x3
#define MM_TYPE_PAGE 			0x3
#define MM_TYPE_BLOCK			0x1
#define MM_ACCESS			(0x1 << 10)
#define MM_ACCESS_PERMISSION		(0x01 << 6) 

/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	000	00000000
 *   NORMAL_NC		001	01000100
 */
#define MT_DEVICE_nGnRnE 		0x0
#define MT_NORMAL_NC			0x1
#define MT_DEVICE_nGnRnE_FLAGS		0x00
#define MT_NORMAL_NC_FLAGS  		0x44
#define MAIR_VALUE			(MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))

#define ATTRINDX_NORMAL		0
#define ATTRINDX_DEVICE		1
#define ATTRINDX_COHERENT	2

#define MAIR_VALUENEW (0xFF << ATTRINDX_NORMAL*8	\
	                | 0x04 << ATTRINDX_DEVICE*8	\
	                | 0x00 << ATTRINDX_COHERENT*8)


#define MMU_FLAGS	 		(MM_TYPE_BLOCK | (MT_NORMAL_NC << 2) | MM_ACCESS)	
#define MMU_DEVICE_FLAGS		(MM_TYPE_BLOCK | (MT_DEVICE_nGnRnE << 2) | MM_ACCESS)	
#define MMU_PTE_FLAGS			(MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)	

#define TCR_T0SZ			(64 - 48) 
#define TCR_T1SZ			((64 - 48) << 16)
#define TCR_TG0_4K			(0 << 14)
#define TCR_TG1_4K			(2 << 30)
#define TCR_VALUE			(TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K)

/* architectural feature access control register */
#define CPACR_EL1_FPEN    (1 << 21) | (1 << 20) // don't trap SIMD/FP registers
#define CPACR_EL1_ZEN     (1 << 17) | (1 << 16)  // don't trap SVE instructions
#define CPACR_EL1_VAL     (CPACR_EL1_FPEN | CPACR_EL1_ZEN)


// System registers
#define SCTLR_EL1_WXN		(1 << 19)		// SCTLR_EL1
#define SCTLR_EL1_I		(1 << 12)
#define SCTLR_EL1_C		(1 << 2)
#define SCTLR_EL1_A		(1 << 1)
#define SCTLR_EL1_M		(1 << 0)

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

#define HIGH_MEMORY             	0x40000000
#define PAGING_MEMORY 			(HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES 			(PAGING_MEMORY/PAGE_SIZE)

/*Descriptor format
`+------------------------------------------------------------------------------------------+
 | Upper attributes | Address (bits 47:12) | Lower attributes | Block/table bit | Valid bit |
 +------------------------------------------------------------------------------------------+
 63                 47                     11                 2                 1           0
*/

#define DESC_VALID                   (1 << 0)
#define DESC_BLOCK                   (0 << 1)
#define DESC_TABLE                   (1 << 1)
#define DESC_ACCESS                  (1 << 10)
#define DESC_KERNEL_PERMS            (1L << 54)
#define DESC_INNER_SHARABLE          (3 << 8)

#define DESC_KERNEL_TABLE_FLAGS      (DESC_TABLE | DESC_VALID)
#define DESC_KERNEL_BLOCK_FLAGS      (DESC_ACCESS | DESC_INNER_SHARABLE | DESC_KERNEL_PERMS | (MATTR_NORMAL_NC_INDEX << 2) | DESC_BLOCK | DESC_VALID)
#define DESC_DEVICE_BLOCK_FLAGS      (DESC_ACCESS | DESC_INNER_SHARABLE | DESC_KERNEL_PERMS | (MATTR_DEVICE_nGnRnE_INDEX << 2) | DESC_BLOCK | DESC_VALID)

#define MATTR_DEVICE_nGnRnE        0x0
#define MATTR_NORMAL_NC            0x44
#define MATTR_DEVICE_nGnRnE_INDEX  0
#define MATTR_NORMAL_NC_INDEX      1
#define MAIR_EL1_VAL               ((MATTR_NORMAL_NC << (8 * MATTR_NORMAL_NC_INDEX)) | MATTR_DEVICE_nGnRnE << (8 * MATTR_DEVICE_nGnRnE_INDEX))

#define PAGES_MAP              7   
#define PAGES_MAP_SIZE         (PAGES_MAP * PAGE_SIZE)
#define ENTRIES_PER_TABLE      512
#define PGD_SHIFT              (PAGE_SHIFT + 3 * TABLE_SHIFT)
#define PUD_SHIFT              (PAGE_SHIFT + 2 * TABLE_SHIFT)
#define PMD_SHIFT              (PAGE_SHIFT + TABLE_SHIFT)
#define PUD_ENTRY_MAP_SIZE     (1 << PUD_SHIFT)

#define BLOCK_SIZE 0x40000000 /* 1GB */

#endif
#endif