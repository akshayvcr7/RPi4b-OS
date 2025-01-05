#include "timer.h"
#include "pl011_uart.h"

const uint32_t interval = 0x100000;
uint32_t curVal = 0;

void timer_init (void)
{
	curVal = mmio_read(&SYS_TIMER_REG->CLO);
	kprintf("Timer curval =0x%x\n", curVal);
	curVal += interval;
	mmio_write(&SYS_TIMER_REG->C1, curVal);
	kprintf("Timer C1 =0x%x\n", mmio_read(&SYS_TIMER_REG->C1));
}

void handle_timer_irq(void)
{
	curVal += interval;
	mmio_write(&SYS_TIMER_REG->C1, curVal);
	mmio_write(&SYS_TIMER_REG->CS, 0x2U);
	kprintf("Timer interrupt received\n\r");
}