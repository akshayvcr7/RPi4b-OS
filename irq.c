#include <stdint.h>
#include "pl011_uart.h"
#include "timer.h"

void exc_handler(uint32_t type, uint32_t esr)
{
    // print out interruption type
    switch(type & 1U) {
        case 0: kprintf("Synchronous "); break;
        case 1: kprintf("IRQ "); break;
        case 2: kprintf("FIQ "); break;
        case 3: kprintf("SError "); break;
    }

    kprintf("%s, ESR: %x\r\n", esr);
}

void print_exception_message(int type, unsigned long esr, unsigned long address)
{
	kprintf("exception type %d, ESR: %x, address: %x\r\n", type, esr, address);
	handle_timer_irq();
}

void enable_interrupt_controller()
{
	mmio_write(&ARMC_IRQ_REG->IRQ0_SET_EN_0, 0x2);
	kprintf("interrupt_controller: %x\n", mmio_read(&ARMC_IRQ_REG->IRQ0_SET_EN_0));
}

void handle_irq(void)
{
	kprintf("pending irq: %x\n", mmio_read(&ARMC_IRQ_REG->IRQ0_PENDING0));
	uint32_t irq = mmio_read(&ARMC_IRQ_REG->IRQ0_PENDING0);
	switch (irq) {
		case (SYS_TIMER_IRQ_1):
			handle_timer_irq();
			break;
		default:
			kprintf("Unknown pending irq: %x\n", irq);
	}
}