#ifndef _IRQ_H_
#define _IRQ_H_

#include <stdint.h>

/* interrupt service routine callback */
typedef void (*isr_ptr)(void);

/* register isr callbacks */
void irq_register_isr(uint16_t irq_id, isr_ptr callback);

#endif /* _IRQ_H_ */