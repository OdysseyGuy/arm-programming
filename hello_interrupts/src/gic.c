#include "gic.h"
#include "uart_pl011.h"

/*
 * specifically for GIC registers since they allow 32-bit writes
 * other registers that needs 32-bit needs to write upper and lower 16-bits
 * separately
 */
#define write32(reg, val) (*(volatile uint32_t*)&reg = val)

static gic_distributor_registers_t* gicd_regs;
static gic_cpu_interface_registers_t* gicc_regs;

/* Return phriphbase from CP15 c15 */
uint32_t cpu_get_periphbase(void) {
    uint32_t result;
    /*
     * Co-processor 15 read instruction.
     * CP15 c15 Configuration base register
     * 
     * mrc coproc, op1, <rd>, CRn, CRm, op2
     */
    asm (
        "mrc p15, #4, %0, c15, c0, #0"
        : "=r" (result)
    );
    return result;
}

void gic_init(void) {
    gicd_regs = (gic_distributor_registers_t*)(cpu_get_periphbase() + GIC_DISTRIBUTOR_OFFSET);
    gicc_regs = (gic_cpu_interface_registers_t*)(cpu_get_periphbase() + GIC_IFACE_OFFSET);

    write32(gicc_regs->PMR, 0xFFFFu); /* Enable all interrupt priorities */
    write32(gicc_regs->CTLR, CCTRL_BIT_ENABLER);  /* Enable inerrupt forwarding to this CPU */

    /* Enable the interrupt distributor */
    write32(gicd_regs->CTLR, DCTRL_BIT_ENABLER);
}

void gic_enable_interrupt(uint8_t interrupt_id) {
    /* total 32, 32-bit registers */
    uint8_t reg_no = interrupt_id / 32;
    uint8_t reg_bit = interrupt_id % 32;

    /* enable the required bit in ISENABLERn */
    uint32_t reg_val = gicd_regs->ISENABLER[reg_no];
    reg_val |= (1u << reg_bit);
    write32(gicd_regs->ISENABLER[reg_no], reg_val);

    /* forward interrupt to CPU interface 0 */
    reg_no = interrupt_id / 4;
    /*
     * offset bit 0 : CPU Interface 0,
     * offset bit 1 : CPU Interface 1,
     * ...
     * offset bit 7 : CPU Interface 7,
     */
    reg_bit = (interrupt_id % 4) * 8; /* needs to offset the bit by 0, 8, 16 or 24 */

    reg_val = gicd_regs->ITARGETSR[reg_no];
    
    /*
     * each 1 byte refer to one interrupt
     * by writing to 1 on every first bit on 1 byte offset
     * means forward the interrupt to 1st CPU interface
     */
    reg_val |= (1u << reg_bit);
    write32(gicd_regs->ITARGETSR[reg_no], reg_val);
}

uint32_t gic_acknowledge_interrupt(void) {
    /* reading interrupt id from iar */
    return gicc_regs->IAR & CIAR_INT_ID_MASK;
}

void gic_signal_intrrupt_end(uint32_t interrupt_id) {
    write32(gicc_regs->EOIR, (interrupt_id & CEOIR_INT_ID_MASK));
}