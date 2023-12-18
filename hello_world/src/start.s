/* processor modes */
.equ FIQ_MODE, 0x11     @ FIQ mode (10001)
.equ IRQ_MODE, 0x12     @ IRQ mode (10010)
.equ SVC_MODE, 0x13     @ SVC mode (10011)

.section .vector_table, "x"
.globl _start
_start: /* exception handlers */
        b Reset_Handler /* 0x00 Reset                   */
        b .             /* 0x04 Undefined Instruction   */
        b .             /* 0x08 Software Interrupt      */
        b .             /* 0x0C Prefetch Abort          */
        b .             /* 0x10 Data Abort              */
        b .             /* 0x14 Reserved                */
        b .             /* 0x18 Interrupt Request (IRQ) */
        b .             /* 0x1C Fast Interrupt (FIQ)    */

.section .text
Reset_Handler:
        /* fiq stack */
        msr cpsr_c, FIQ_MODE    @ switch to FIQ mode without changing condition flags
        ldr r1, =_fiq_stack_start
        ldr sp, =_fiq_stack_end
        movw r0, #0xFEFE
        movt r0, #0xFEFE

/* recursive stack filling */
fiq_loop:
        cmp r1, sp
        strlt r0, [r1], #4
        blt fiq_loop

        /* irq stack */
        msr cpsr_c, IRQ_MODE    @ switch to IRQ mode without changing condition flags
        ldr r1, =_irq_stack_start
        ldr sp, =_irq_stack_end

irq_loop:
        cmp r1, sp
        strlt r0, [r1], #4
        blt irq_loop

        /* supervisor mode */
        msr cpsr_c, SVC_MODE    @ switch to SVC mode without changing condition flags
        ldr r1, =_stack_start
        ldr sp, =_stack_end

stack_loop:
        cmp r1, sp
        strlt r0, [r1], #4
        blt stack_loop

        /* copy data section from ROM to RAM */
        ldr r0, =_text_end;
        ldr r1, =_data_start;   @ start data section in RAM
        ldr r2, =_data_end;

/* copy everything after _text_end, to (_data_start - _data_end) region */
copy_loop:
        cmp r1, r2
        ldrlt r3, [r0], #4      @ load 4 bytes from r0 to r3
        strlt r3, [r1], #4      @ store 4 bytes form r3 to r1
        blt copy_loop

        /* zero the bss section */
        ldr r1, =_bss_start
        ldr r2, =_bss_end
        mov r0, #0

bss_loop:
        cmp r1, r2
        strlt r0, [r1], #4
        blt bss_loop

        bl main                 @ call main function in C
