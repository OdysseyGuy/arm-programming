#include <stddef.h>
#include "irq.h"

/* see gic.c for defitions */
extern uint32_t gic_acknowledge_interrupt(void);
extern void gic_signal_intrrupt_end(uint32_t);

static isr_ptr callbacks[1024] = { NULL };

void irq_register_isr(uint16_t irq_id, isr_ptr callback) {
    if (callbacks[irq_id] != NULL) {
        return;
    }

    callbacks[irq_id] = callback;
}

void __attribute__((interrupt)) irq_handler(void) {
    uint16_t irq = gic_acknowledge_interrupt();
    isr_ptr isr = callbacks[irq];
    if (isr != NULL) {
        isr();
    }
    gic_signal_intrrupt_end(37);
}