.section ".text.boot"

.globl _start

_start:
    // Check processor ID
	mrs	x0, mpidr_el1		
	and	x0, x0,#0xFF
    // not the main core will hang in an infinite wait loop
	cbz	x0, setup
	b	hang

hang: 
	b 	hang

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
	bl	kernel_main
    // if return halt the master core too <which is not possible> and should never come here
	b 	hang		

//debugging using qemu
///home/av/qemu/bin/debug/native/qemu-system-aarch64 -M raspi4b -serial null -serial mon:stdio -kernel kernel8.img -nographic
///home/av/qemu/bin/debug/native/qemu-system-aarch64 -s -S -M raspi4b -nographic -kernel kernel8.img --debugging 
// gdb-multiarch ; target remote :1234