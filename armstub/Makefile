ARMCC = aarch64-none-elf
ARMPATH = /home/av/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin

all: armstub

armstub8.o: armstub8.S
	$(ARMPATH)/$(ARMCC)-gcc -c $< -o $@

armstub: armstub8.o
	$(ARMPATH)/$(ARMCC)-ld --section-start=.text=0 armstub8.o -o armstub8.elf
	$(ARMPATH)/$(ARMCC)-objcopy armstub8.elf -O binary armstub8.tmp
	dd if=armstub8.tmp ibs=256 of=armstub8_v1.bin conv=sync

clean:
	rm -rf *.o *.elf *.bin

