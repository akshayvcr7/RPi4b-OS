#include <stdint.h>

struct system_timer_reg {
    uint32_t CS;
    uint32_t CLO;
    uint32_t CHI;
    uint32_t C0;
    uint32_t C1;
};

#define SYS_TIMER_REG ((struct system_timer_reg *)(PERIPHERAL_BASE + 0x00003000))

//Core 0
struct armc_irq_regs {
    uint32_t IRQ0_PENDING0;
    uint32_t IRQ0_PENDING1;
    uint32_t IRQ0_PENDING2;
    uint32_t reserved0;
    uint32_t IRQ0_SET_EN_0;
    uint32_t IRQ0_SET_EN_1;
    uint32_t IRQ0_SET_EN_2;
    uint32_t reserved1;
    uint32_t IRQ0_CLR_EN_0;
    uint32_t IRQ0_CLR_EN_1;
    uint32_t IRQ0_CLR_EN_2;
};


#define ARMC_IRQ_REG ((struct armc_irq_regs *)(PERIPHERAL_BASE + 0x0000B200))

enum vc_irqs {
    SYS_TIMER_IRQ_0 = 1,
    SYS_TIMER_IRQ_1 = 2,
    SYS_TIMER_IRQ_2 = 4,
    SYS_TIMER_IRQ_3 = 8,
    AUX_IRQ = (1 << 29)
};

void timer_init(void);
void handle_timer_irq(void);