#ifndef _SYSREGS_H
#define _SYSREGS_H



// SCTLR_EL1, System Control Register (EL1)
#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// SCR_EL3, Secure Configuration Register (EL3)
#define SCR_RW				(1 << 10) // The next lower level is AArch64.
#define SCR_NS				(1 << 0) // EL0 and EL1 are in Non-secure state, and so memory accesses from those Exception levels cannot access Secure memory
#define SCR_VALUE	    	(SCR_RW | SCR_NS)

#endif