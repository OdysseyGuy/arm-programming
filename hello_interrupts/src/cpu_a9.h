#ifndef _CPU_A9_H_
#define _CPU_A9_H_

#include <stdint.h>

/* Enable interrupts globally */
static inline void cpu_enable_interrupts(void) {
    asm ("cpsie if");
}

/* Disable interrupts globally */
static inline void cpu_disable_interrupts(void) {
    asm ("cpsid if");
}

#endif /* _CPU_H_ */