#include "sysreg.h"

.macro interrupt_entry type
	mov	x0, #\type
	mrs	x1, esr_el1
	mrs	x2, elr_el1
	bl	print_exception_message
	b	hang
.endm

.macro kernel_entry, el
	sub 	sp, sp, #S_FRAME_SIZE
	stp 	x0, x1, [sp, #16 * 0]
	stp 	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]

	.if	\el == 0
	mrs	x21, sp_el0
	.else
	add	x21, sp, #S_FRAME_SIZE
	.endif /* \el == 0 */

    mrs	x22, elr_el1
	mrs	x23, spsr_el1
	stp	x30, x22, [sp, #16 * 15]
	str	x23, [sp, #16 * 16]
.endm

.macro	kernel_exit, el
	ldp	x22, x23, [sp, #16 * 16]
	ldp	x30, x21, [sp, #16 * 15]

	.if	\el == 0
	msr	sp_el0, x21
	.endif /* \el == 0 */

	msr	elr_el1, x22
	msr	spsr_el1, x23


	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	add	sp, sp, #S_FRAME_SIZE
	eret
	.endm

.align 11
.globl vector_table
vector_table:
	// EL1 synchronous_t
    .align  7
    interrupt_entry 0
    // EL1 IRQ_t
    .align  7
    interrupt_entry 1
    // EL1 FIQ_t
    .align  7
    interrupt_entry 2
    // EL1 SError_t
    .align  7
    interrupt_entry 3

	// EL1 synchronous_h
    .align  7
    interrupt_entry 4
    // EL1 IRQ_h
    .align  7
    bl handle_el1_irq
    // EL1 FIQ_h
    .align  7
    interrupt_entry 6
    // EL1 SError_h
    .align  7
    interrupt_entry 7

    //EL0 synchronous_t
    .align  7
    bl handle_el0_sync
    //EL0 IRQ_t
    .align  7
    bl el0_irq
    //EL0 FIQ_t
    .align  7
    interrupt_entry 10
    //EL0 SError_t
    .align  7
    interrupt_entry 11

    //EL0 synchronous_t
    .align  7
    interrupt_entry 12
    //EL0 IRQ_t
    .align  7
    interrupt_entry 13
    //EL0 FIQ_t
    .align  7
    interrupt_entry 14
    //EL0 SError_t
    .align  7
    interrupt_entry 15

.globl elx_irq
elx_irq:
	bl	handle_irq
    eret

handle_el1_irq:
	kernel_entry 1
	bl	handle_irq
	kernel_exit 1

.globl enable_irq
enable_irq:
	msr    daifclr, #0xf
	ret

.globl disable_irq
disable_irq:
	msr	daifset, #0xf
	ret

el0_irq:
	kernel_entry 0
	bl	handle_irq
	kernel_exit 0

handle_el0_sync:
	kernel_entry 0
	mrs	x25, esr_el1				// read the syndrome register
	lsr	x24, x25, #ESR_ELx_EC_SHIFT		// exception class
	cmp	x24, #ESR_ELx_EC_SVC64			// SVC in 64-bit state
	b.eq	handle_el0_svc
	b hang

sc_nr	.req	x25					// number of system calls
scno	.req	x26					// syscall number
stbl	.req	x27					// syscall table pointer

handle_el0_svc:
	adr    stbl, sys_call_table      // load syscall table pointer
    uxtw   scno, w8                  // syscall number in w8
    mov    sc_nr, #__NR_syscalls
    bl     enable_irq
    cmp    scno, sc_nr               // check upper syscall limit
    b.hs   ni_sys
    ldr    x16, [stbl, scno, lsl #3] // address in the syscall table
    blr    x16                       // call sys_* routine
    b      ret_from_syscall
ni_sys:
	b hang
ret_from_syscall:
	bl	disable_irq
	str	x0, [sp, #S_X0]		// returned x0
	kernel_exit 0

.globl ret_from_fork
ret_from_fork:
	bl	schedule_tail
	cbz	x19, ret_to_user			// not a kernel thread
	mov	x0, x20
	blr	x19 		//should never return
ret_to_user:
	bl disable_irq
	kernel_exit 0

.globl hang
hang:
	b test_error

.global abt
abt:
    ldr x0, =0xFFFFFFFFFF000000
    ldr x1, [x0]