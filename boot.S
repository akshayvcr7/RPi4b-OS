#include "sysreg.h"

.section ".text.boot"

.globl _start
.globl get_el

_start:
    // Check processor ID
	mrs	x3, mpidr_el1
	and	x3, x3,#0xFF
    // not the main core will hang in an infinite wait loop
	cbz	x3, master
	b	hang

hang:
	b 	hang

master:
	ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0

	mov	x0, #0x80000000
	msr	hcr_el2, x0

	mov	x0, #0x3c5
	msr	spsr_el2, x0

	adr	x0, setup
	msr	elr_el2, x0

    mov x0, sp
    msr SP_EL1, x0

    eret


setup:
	// Clean the BSS section
    ldr     x4, =__bss_start     // Start address
    ldr     x5, =__bss_size      // Size of the section
loop:
    cbz     x5, stack               // Quit loop if zero
    str     xzr, [x4], #8
    sub     x5, x5, #1
    cbnz    x5, loop

stack:
   // Set stack to start below our code
    ldr     x3, =_start
    mov     sp, x3
	bl	kernel_main
    // if return halt the master core too <which is not possible> and should never come here
	b 	hang

get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

//debugging using qemu
///home/av/qemu/bin/debug/native/qemu-system-aarch64 -M raspi4b -serial null -serial mon:stdio -kernel kernel8.img -nographic
///home/av/qemu/bin/debug/native/qemu-system-aarch64 -s -S -M raspi4b -nographic -kernel kernel8.img --debugging
// gdb-multiarch ; target remote :1234