cmake_minimum_required (VERSION 3.6)

set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  ARM)
set(CMAKE_CROSSCOMPILING    TRUE)

set(UBOOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../u-boot")
set(MKIMAGE "${UBOOT_PATH}/tools/mkimage")

project (bare-metal-arm C ASM)

#==============================
# Compiler setup
#==============================
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "arm-none-eabi-as")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")

#==============================
# U-Boot
#==============================
add_custom_target(u-boot 
    COMMAND make vexpress_ca9x4_config ARCH=arm CROSS_COMPILE=arm-none-eabi- 
    COMMAND make all ARCH=arm CROSS_COMPILE=arm-none-eabi- 
    WORKING_DIRECTORY ${UBOOT_PATH})

#==============================
# Bare metal app
#==============================
file(GLOB LINKSCRIPT "src/linkscript.ld")
set(ASMFILES    src/start.s)
set(SRCLIST     src/main.c src/uart_pl011.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles -g -Wall")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKSCRIPT} -lgcc -lm")

add_executable(bare-metal ${SRCLIST} ${ASMFILES})
set_target_properties(bare-metal PROPERTIES OUTPUT_NAME "bare-metal.elf")
add_dependencies(bare-metal u-boot)

add_custom_command(TARGET bare-metal POST_BUILD COMMAND ${CMAKE_OBJCOPY}
    -O binary bare-metal.elf bare-metal.bin COMMENT "Converting ELF to binary")

add_custom_command(TARGET bare-metal POST_BUILD COMMAND ${MKIMAGE}
    -A arm -C none -T kernel -a 0x60000000 -e 0x60000000 -d bare-metal.bin bare-arm.uimg
    COMMENT "Building U-Boot image")

add_custom_command(TARGET bare-metal POST_BUILD COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create-sd.sh 
    sdcard.img bare-arm.uimg
    COMMENT "Creating SD card image")

#==============================
# Qemu run command
#==============================
add_custom_target(run)
add_custom_command(TARGET run POST_BUILD COMMAND 
    qemu-system-arm -M vexpress-a9 -m 512M -no-reboot -nographic 
    -monitor telnet:127.0.0.1:1234,server,nowait -kernel ${UBOOT_PATH}/u-boot -sd sdcard.img -serial mon:stdio
    COMMENT "Running QEMU...")
