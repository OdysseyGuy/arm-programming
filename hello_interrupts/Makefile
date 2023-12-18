TOOLCHAIN_PREFIX := arm-none-eabi-

# u-boot bootloader
UBOOT := ../u-boot
MKIMAGE := $(UBOOT)/tools/mkimage

OUTELF := bare-arm.elf
OUTBIN := bare-arm.bin
LINKSCRIPT := src/linkscript.ld

CC := $(TOOLCHAIN_PREFIX)gcc
ASM := $(TOOLCHAIN_PREFIX)as
LD := $(TOOLCHAIN_PREFIX)ld
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy

# flags
CFLAGS := -nostdlib -nostartfiles -Wall -g -Og -mthumb -mcpu=cortex-a9
LDFLAGS := -T $(LINKSCRIPT) -nostdlib -L/home/rik/Downloads/gcc-arm-none-eabi/arm-none-eabi/lib\
	-L/home/rik/Downloads/gcc-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1

# u-boot mkimage uImage flags
MKIMAGEFLAGS := -A arm -C none -T kernel -a 0x60000000 -e 0x60000000

QEMUFLAGS := -M vexpress-a9 -m 512M -no-reboot\
	-monitor telnet:127.0.0.1:1234,server,nowait, -kernel\
	$(UBOOT)/u-boot -sd sdcard.img -serial mon:stdio


build:
	$(CC) $(CFLAGS) -c src/main.c -o main.o
	$(CC) $(CFLAGS) -c src/uart_pl011.c -o uart_pl011.o
	$(CC) $(CFLAGS) -c src/gic.c -o gic.o
	$(CC) $(CFLAGS) -c src/irq.c -o irq.o
	$(ASM) src/start.s -o start.o
	$(LD) $(LDFLAGS) start.o main.o uart_pl011.o gic.o irq.o -lm -lgcc -o $(OUTELF)
	$(OBJCOPY) -O binary $(OUTELF) $(OUTBIN)

uimage:
	$(MKIMAGE) $(MKIMAGEFLAGS) -d $(OUTBIN) bare-arm.uimg

sdcard:
	bash ./scripts/create-sd.sh sdcard.img bare-arm.uimg

all: build
	sdcard

qemu:
	qemu-system-arm $(QEMUFLAGS)

run: all qemu

clean:
	rm -rf *.elf *.img *.o *.bin *.uimg
