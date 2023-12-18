#ifndef _UART_PL011_H_
#define _UART_PL011_H_

#include <stdint.h>
#include <stdbool.h>

/*
 *========================================
 * Driver Register Bit definitions
 *========================================
 */

/* ===== Flag register bits ============================ */
#define FR_BIT_RI   (1 << 8) /* Ring indicator */
#define FR_BIT_TXFE (1 << 7) /* Transmit FIFO empty */
#define FR_BIT_RXFF (1 << 6) /* Receive FIFO full */
#define FR_BIT_TXFF (1 << 5) /* Transmit FIFO full */
#define FR_BIT_RXFE (1 << 4) /* Receive FIFO empty */
#define FR_BIT_BUSY (1 << 3) /* UART busy */
#define FR_BIT_DCD  (1 << 2) /* Data carrier detect */
#define FR_BIT_DSR  (1 << 1) /* Data set ready */
#define FR_BIT_CTS  (1 << 0) /* Clear to send */

/* ===== Control register bits ============================ */
#define CR_BIT_UARTEN   (1 << 0) /* UART enable */

/* ===== Line control register bits ======================= */
#define LCRH_BIT_SPS        (1 << 7)    /* Stick parity select */

/* UART word lengths */
#define LCRH_WLEN_8_BITS    (1 << 6 | 1 << 5)
#define LCRH_WLEN_7_BITS    (1 << 6 | 0 << 5)
#define LCRH_WLEN_6_BITS    (0 << 6 | 1 << 5)
#define LCRH_WLEN_5_BITS    (0 << 6 | 0 << 5)

#define LCRH_BIT_FEN        (1 << 4)    /* Enable FIFOs */
#define LCRH_BIT_STP2       (1 << 3)    /* Two stop bits select */
#define LCRH_BIT_EPS        (1 << 2)    /* Even parity select */
#define LCRH_BIT_PEN        (1 << 1)    /* Parity enable */

/* ===== Data register bits ============================ */
#define DR_DATA_MASK    0xFFu   /* bits 7:0 */

/* ===== Masked Interrupt Status register bits ============================ */
#define MIS_BIT_BEMIS   (1 << 9)    /* Break error */
#define MIS_BIT_RXMIS   (1 << 4)    /* Receiver error */

/* ===== Receive status / error clear register bits ========================== */
#define RSRECR_BIT_BE   (1 << 2)    /* Break error */

/*
 *========================================
 * UART (PL011) register offsets
 *========================================
 */
typedef volatile struct __attribute__((packed)) uart011_registers {
    uint32_t        DR;             /* 0x0 Data register. */
    uint32_t        RSRECR;         /* 0x4 Receive status / error clear register. */
    uint32_t        _reserved0[4];  /* 0x8-0x14 Reserved */
    const uint32_t  FR;             /* 0x18 Flag register (read-only) */
    uint32_t        _reserved1;     /* 0x1C Reserved */
    uint32_t        ILPR;           /* 0x20 Low power counter register */
    uint32_t        IBRD;           /* 0x24 Integer baud rate register */
    uint32_t        FBRD;           /* 0x28 Fractional baud rate register */
    uint32_t        LCRH;           /* 0x2C Line control register */
    uint32_t        CR;             /* 0x30 Control register */
    uint32_t        IFLS;           /* 0x34 Interrupt FIFO Level Select register */
    uint32_t        IMSC;           /* 0x38 Interrupt Mask Set/Clear register */
    const uint32_t  RIS;            /* 0x3C Raw Interrupt Status register */
    const uint32_t  MIS;            /* 0x40 Masked Interrupt Status register */
    uint32_t        ICR;            /* 0x44 Interrupt Clear register */
} uart011_registers_t;

/*
 *========================================
 * Error codes
 *========================================
 */

/* UART driver error codes */
typedef enum uart_error {
    UART_OK = 0,
    UART_INVALID_ARGUMENT_BAUD_RATE,
    UART_INVALID_ARGUMENT_WORD_SIZE,
    UART_INVALID_ARGUMENT_STOP_BITS,
} uart_error_t;

/* UART driver programmable configurations */
typedef struct uart_config {
    uint32_t baudrate;
    uint8_t data_bits;  /* 5 to 8 */
    bool parity;        /* whether or not to enable parity */
    uint8_t stop_bits;  /* 1 or 2 */
} uart_config_t;

/*
 *========================================
 * Driver functions
 *========================================
 */

uart_error_t uart_init(void);

/* configure uart driver */
uart_error_t uart_configure(uart_config_t* config);

/* ===== UART read/write functions =============*/
void uart_putchar(char c);
void uart_write(const char* data);

void uart_getchar(char* c);

#endif /* _UART_PL011_H_ */
