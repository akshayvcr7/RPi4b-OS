.section ".text.boot"

.globl _start

_start:
    // Check processor ID
	mrs	x3, mpidr_el1
	and	x3, x3,#0xFF
    // not the main core will hang in an infinite wait loop
	cbz	x3, setup
	b	hang

hang:
	b 	hang

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

//debugging using qemu
///home/av/qemu/bin/debug/native/qemu-system-aarch64 -M raspi4b -serial null -serial mon:stdio -kernel kernel8.img -nographic
///home/av/qemu/bin/debug/native/qemu-system-aarch64 -s -S -M raspi4b -nographic -kernel kernel8.img --debugging
// gdb-multiarch ; target remote :1234