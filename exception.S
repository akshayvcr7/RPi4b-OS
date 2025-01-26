.macro interrupt_entry type
	mov	x0, #\type
	mrs	x1, esr_el1
	mrs	x2, elr_el1
	bl	print_exception_message
	b	hang
.endm

//stack frame size
#define S_FRAME_SIZE			256

.macro kernel_entry
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
    mrs	x22, elr_el1
	mrs	x23, spsr_el1
	stp	x30, x22, [sp, #16 * 15]
	str	x23, [sp, #16 * 16]
.endm

.macro kernel_exit
    ldr	x23, [sp, #16 * 16]
	ldp	x30, x22, [sp, #16 * 15]

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
	ldr	x30, [sp, #16 * 15]
	add	sp, sp, #S_FRAME_SIZE
	eret
.endm

.align 11
.globl vector_table
vector_table:
	// synchronous_t
    .align  7
    interrupt_entry 0
    // IRQ_t
    .align  7
    interrupt_entry 1
    // FIQ_t
    .align  7
    interrupt_entry 2
    // SError_t
    .align  7
    interrupt_entry 3

	// synchronous_h
    .align  7
    interrupt_entry 4
    // IRQ_h
    .align  7
    bl handle_el1_irq
    // FIQ_h
    .align  7
    interrupt_entry 6
    // SError_h
    .align  7
    interrupt_entry 7

    // synchronous_t
    .align  7
    interrupt_entry 8
    // IRQ_t
    .align  7
    interrupt_entry 9
    // FIQ_t
    .align  7
    interrupt_entry 10
    // SError_t
    .align  7
    interrupt_entry 11

    // synchronous_t
    .align  7
    interrupt_entry 12
    // IRQ_t
    .align  7
    interrupt_entry 13
    // FIQ_t
    .align  7
    interrupt_entry 14
    // SError_t
    .align  7
    interrupt_entry 15

.globl elx_irq
elx_irq:
	bl	handle_irq
    eret

handle_el1_irq:
	kernel_entry
	bl	handle_irq
	kernel_exit

.globl enable_irq
enable_irq:
	msr    daifclr, #0xf
	ret

.globl disable_irq
disable_irq:
	msr	daifset, #0xf
	ret

.globl hang
hang: b hang

.global abt
abt:
    ldr x0, =0xFFFFFFFFFF000000
    ldr x1, [x0]
