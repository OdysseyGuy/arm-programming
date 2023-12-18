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
CFLAGS := -nostdlib -nostartfiles -Wall
LDFLAGS := -T $(LINKSCRIPT) -nostdlib

# u-boot mkimage uImage flags
MKIMAGEFLAGS := -A arm -C none -T kernel -a 0x60000000 -e 0x60000000

QEMUFLAGS := -M vexpress-a9 -m 512M -nographic -no-reboot\
	-monitor telnet:127.0.0.1:1234,server,nowait, -kernel\
	$(UBOOT)/u-boot -sd sdcard.img

all:
	$(CC) $(CFLAGS) -c src/main.c -o main.o
	$(ASM) src/startup.s -o startup.o
	$(LD) $(LDFLAGS) startup.o main.o -o $(OUTELF)
	$(OBJCOPY) -O binary $(OUTELF) $(OUTBIN)
	$(MKIMAGE) $(MKIMAGEFLAGS) -d $(OUTBIN) bare-arm.uimg
	bash ./scripts/create-sd.sh sdcard.img bare-arm.uimg

qemu:
	qemu-system-arm $(QEMUFLAGS)

clean:
	rm -rf *.elf *.img *.o *.bin *.uimg