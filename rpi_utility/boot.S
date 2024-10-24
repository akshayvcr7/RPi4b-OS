.section ".text.boot"

.globl _start

/*_start:
    // Check processor ID
	mrs	x0, mpidr_el1
	and	x0, x0,#0xFF
    // not the main core will hang in an infinite wait loop
	cbz	x0, setup
	b	hang
*/

_start:

    mov     x12, x0

    ldr     x1, =0x80000
    ldr     x2, =_start
    ldr     w3, =__load_size
1:  ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, 1b

setup:
	// Clean the BSS section
    ldr     x1, =__bss_start     // Start address
    ldr     x2, =__bss_size      // Size of the section
loop:
    cbz     x2, stack               // Quit loop if zero
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, loop

stack:
   // Set stack to start below our code
    ldr     x1, =_start
    mov     sp, x1
	bl	kernel_main-0x400

    // if return halt the master core too <which is not possible> and should never come here
hang:
    b hang