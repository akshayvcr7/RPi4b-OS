ARMCC = aarch64-none-elf
ARMPATH = /home/av/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin
ARMCCFLAGS = -Wall -O2 -ffreestanding -Iinc -nostdlib -nostartfiles
CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)

all: clean kernel8.img

boot.o: boot.S
	$(ARMPATH)/$(ARMCC)-gcc $(ARMCCFLAGS) -I./include -ggdb3 -c boot.S -o boot.o

%.o: %.c
	$(ARMPATH)/$(ARMCC)-gcc $(ARMCCFLAGS) -I ./include -ggdb3 -c $< -o $@

kernel8.img: boot.o $(OFILES)
	$(ARMPATH)/$(ARMCC)-ld -nostdlib boot.o $(OFILES) -T link.ld -Map kernel8.map -o kernel8.elf
	$(ARMPATH)/$(ARMCC)-objcopy -O binary kernel8.elf kernel8.img
	$(ARMPATH)/$(ARMCC)-objdump -d kernel8.elf > kernel8.objdump # for debugging

clean:
	rm -rf kernel8.elf 
	rm -rf *.o *.img *.map
