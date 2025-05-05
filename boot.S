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

	/*ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0

	mov	x0, #0x80000000
	msr	hcr_el2, x0

	// set up exception handlers
    ldr     x2, =vector_table
    msr     vbar_el1, x2

	mov	x0, #0x3c5
	msr	spsr_el2, x0

	adr	x0, setup
	msr	elr_el2, x0

    mov x0, sp
    msr SP_EL1, x0

    eret*/

	ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0

	mov	x0, #0x80000000
	msr	hcr_el2, x0

	ldr	x0, =SCR_VALUE
	msr	scr_el3, x0

	ldr x2, =vector_table // load VBAR_EL1 with virtual
	msr	vbar_el1, x2

	mov	x0, #0x1c5
	msr	spsr_el3, x0

	adr	x0, setup
	msr	elr_el3, x0

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

// debugging using qemu
// /home/av/qemu/bin/debug/native/qemu-system-aarch64 -M raspi4b -serial null -serial mon:stdio -kernel kernel8.img -nographic [El2]
// /home/av/qemu/bin/debug/native/qemu-system-aarch64 -M raspi4b -serial mon:stdio -nographic -cpu cortex-a72 -device loader,file=./kernel8.img -d int [EL3]
// /home/av/qemu/bin/debug/native/qemu-system-aarch64 -s -S -M raspi4b -nographic -kernel kernel8.img --debugging
// gdb-multiarch ; target remote :1234
// sudo ./raspbootcom /dev/ttyUSB0 /home/av/project/rpi4os_git_run/RPi4b-OS/kernel8.img