ARMCC = aarch64-none-elf
ARMPATH = /home/av/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin
ARMCCFLAGS = -Wall -O2 -ffreestanding -Iinc -nostdlib -nostartfiles -mgeneral-regs-only
CFILES = $(wildcard *.c)
C_OFILES = $(CFILES:.c=.o)
SFILES = $(wildcard *.S)
S_OFILES = $(SFILES:.S=.o)

ifeq ($(mmu),1)
	CFLAGS = -DMMU_ENABLE
else
	CFLAGS =
endif

all: clean kernel8.img

%.o: %.S
	$(ARMPATH)/$(ARMCC)-gcc $(ARMCCFLAGS) $(CFLAGS) -I ./include -ggdb3 -c $< -o $@

%.o: %.c
	$(ARMPATH)/$(ARMCC)-gcc $(ARMCCFLAGS) $(CFLAGS) -I ./include -ggdb3 -c $< -o $@

kernel8.img: $(S_OFILES) $(C_OFILES)
	$(ARMPATH)/$(ARMCC)-ld -nostdlib $(S_OFILES) $(C_OFILES) -T link.ld -Map kernel8.map -o kernel8.elf
	$(ARMPATH)/$(ARMCC)-objcopy -O binary kernel8.elf kernel8.img
	$(ARMPATH)/$(ARMCC)-objdump -d kernel8.elf > kernel8.objdump # for debugging

clean:
	rm -rf kernel8.elf
	rm -rf *.o *.img *.map *.objdump *.d
