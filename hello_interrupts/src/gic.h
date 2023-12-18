/*
 *==================================================
 * Implementation for GICv1 interrupt controller.
 *==================================================
 */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <stdint.h>

#define GIC_IFACE_OFFSET        0x100
#define GIC_DISTRIBUTOR_OFFSET  0x1000

/*
 *========================================
 * GIC register bit definitions
 *========================================
 */

#define DCTRL_BIT_ENABLER   0x1
#define CCTRL_BIT_ENABLER   0x1

#define CIAR_INT_ID_MASK    0x3FF
#define CEOIR_INT_ID_MASK   0x3FF

typedef enum interrupt_state {
    INACTIVE = 0,
    PENDING,
    ACTIVE,
    ACTIVE_AND_PENDING,
} interrupt_state_t;

/*
 *========================================
 * GIC register offsets
 *========================================
 */

// TODO: replace struct with macros to define (base + offset)

/* GIC Distributor registors */
typedef volatile struct __attribute__((packed)) gic_distributor_registers {
    uint32_t        CTLR;               /* 0x0 Distributor Control Register */
    const uint32_t  TYPER;              /* 0x4 Interrupt Controller Type Register */
    const uint32_t  IIDR;               /* 0x8 Distributor Implementer Identification Register */
    uint32_t        _reserved0[29];     /* 0xC - 0x7C Reserved */
    uint32_t        IGROUPR[32];        /* 0x80 - 0xFC Interrupt group registers */
    uint32_t        ISENABLER[32];      /* 0x100 - 0x17C Interrupt setenable registers */
    uint32_t        ICEANABLER[32];     /* 0x180 - 0x1FC Interrupt clearenable registers */
    uint32_t        ISPENDR[32];        /* 0x200 - 0x27C Interrupt setpending registers */
    uint32_t        ICPEND[32];         /* 0x280 - 0x2FC Interrupt clearpending registers */
    const uint32_t  ICDABR[32];         /* 0x300 - 0x3FC Active Bit Registers (GIC v1) */
    uint32_t        _reserved1[32];     /* 0x380 - 0x3FC reserved on GIC v1 */
    uint32_t        IPRIORITYR[255];    /* 0x400 - 0x7F8 Interrupt priority registers */
    uint32_t        _reserved2;         /* 0x7FC Reserved */
    const uint32_t  ITARGETSRO[8];      /* 0x800 - 0x81C Interrupt CPU targets, (read-only) */
    uint32_t        ITARGETSR[246];     /* 0x820 - 0xBF8 Interrupt CPU targets */
    uint32_t        _reserved3;         /* 0xBFC Reserved */
    uint32_t        ICFGRn[64];         /* 0xC00 - 0xCFC Interrupt config registers */
} gic_distributor_registers_t;

/* GIC CPU interface registors */
typedef volatile struct __attribute__((packed)) gic_cpu_interface_registers {
    uint32_t        CTLR;   /* 0x0 Control Register */
    uint32_t        PMR;    /* 0x4 Interrupt Priority Mask Register */
    uint32_t        BPR;    /* 0x8 Binary Point Register */
    const uint32_t  IAR;    /* 0xC Interrupt Acknowledge Register (read-only) */
    uint32_t        EOIR;   /* 0x10 End of Interrupt Register */
    const uint32_t  RPR;    /* 0x14 Running Priority Register */
    const uint32_t  HPPIR;  /* 0x18 Hightest Priority Pending Interrupt Register */
    uint32_t        ABPR;   /* 0x1C Aliased Binary Point Register */
} gic_cpu_interface_registers_t;

/*
 *========================================
 * Controller functions
 *========================================
 */

void gic_init(void);
void gic_enable_interrupt(uint8_t interrupt_id);

/* let gic know interrupt has been requested and signal when done */
/* @returns ID of acknowledged interrupt */
uint32_t gic_acknowledge_interrupt(void);
void gic_signal_intrrupt_end(uint32_t interrupt_id);

#endif /* _INTERRUPT_H_ */