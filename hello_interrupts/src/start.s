/* processor modes */
.equ MODE_FIQ, 0x11     @ FIQ mode (10001)
.equ MODE_IRQ, 0x12     @ IRQ mode (10010)
.equ MODE_SVC, 0x13     @ SVC mode (10011)

.section .vector_table, "x"
.globl _start
_start: /* exception handlers */
        b Reset_Handler /* 0x0 Reset */
        b .             /* 0x4 Undefined Instruction */
        b .             /* 0x8 Software Interrupt */
        b .             /* 0xC Prefetch Abort */
        b .             /* 0x10 Data Abort */
        b .             /* 0x14 Reserved */
        b irq_handler   /* 0x18 Interrupt Request (IRQ) */
        b .             /* 0x1C Fast Interrupt (FIQ) */

.section .text
Reset_Handler:
        /*
         * Set vector table base address to 0x60000000.
         * This is a qemu specific modification since our
         * flash memory starts from 0x60000000. The cpu expects it 
         * to be at address 0x0 so we need offset the vector
         * table address to 0x60000000.
         */
        ldr r0, =0x60000000
        mcr p15, #0, r0, c12, c0, #0


        /* fiq stack */
        msr cpsr_c, MODE_FIQ    @ switch to FIQ mode without changing condition flags
        ldr r1, =_fiq_stack_start
        ldr sp, =_fiq_stack_end
        movw r0, #0xFEFE
        movt r0, #0xFEFE

.L_fiq_loop:
        cmp r1, sp
        strlt r0, [r1], #4
        blt .L_fiq_loop

        /* irq stack */
        msr cpsr_c, MODE_IRQ    @ switch to IRQ mode without changing condition flags
        ldr r1, =_irq_stack_start
        ldr sp, =_irq_stack_end

.L_irq_loop:
        cmp r1, sp
        strlt r0, [r1], #4
        blt .L_irq_loop

        /* supervisor mode */
        msr cpsr_c, MODE_SVC    @ switch to SVC mode without changing condition flags
        ldr r1, =_stack_start
        ldr sp, =_stack_end

.L_stack_loop:
        cmp r1, sp
        strlt r0, [r1], #4
        blt .L_stack_loop

        /* copy data section from ROM to RAM */
        ldr r0, =_text_end
        ldr r1, =_data_start    @ start data section in RAM
        ldr r2, =_data_end

/* copy everything after _text_end, to (_data_start - _data_end) region */
.L_copy_loop:
        cmp r1, r2
        ldrlt r3, [r0], #4      @ load 4 bytes from r0 to r3
        strlt r3, [r1], #4      @ store 4 bytes form r3 to r1
        blt .L_copy_loop

        /* zero the bss section */
        ldr r1, =_bss_start
        ldr r2, =_bss_end
        mov r0, #0

.L_clear_bss:
        cmp r1, r2
        strlt r0, [r1], #4
        blt .L_clear_bss

        cpsid if                @ disable interrupts, re-enable once the GIC is initialized

        bl main                 @ call main function in C
