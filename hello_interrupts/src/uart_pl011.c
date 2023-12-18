#include <math.h>
#include "uart_pl011.h"
#include "irq.h"

static uart011_registers_t* uart0 = (uart011_registers_t*)0x10009000;

/* https://developer.arm.com/documentation/dui0447/j/hardware-description/clock-architecture */
static const uint32_t refclock = 24000000u; /* 24 MHz */

uart_error_t uart_init(void) {
    return UART_OK;
}

/* see gic.c for defitions */
extern uint32_t gic_acknowledge_interrupt(void);
extern void gic_signal_intrrupt_end(uint32_t);

/* uart interrupt handler */
void uart_isr(void) {
    uint32_t status = uart0->MIS;
    if (status & MIS_BIT_RXMIS) {
        /* read the received character and print it back */
        char c = uart0->DR & DR_DATA_MASK;
        uart_putchar(c);
        if (c == '\r') {
            uart_putchar('\n'); /* in case of return print new line */
        }
    } else if (status & MIS_BIT_BEMIS) {
        uart_write("Break error detected!\n");
        /* write to RSRECR clears the error */
        uart0->RSRECR = RSRECR_BIT_BE;
        uart0->ICR = MIS_BIT_BEMIS;
    }

    uart0->ICR = 0x7FF; /* clear all the interrupts */
}

uart_error_t uart_configure(uart_config_t* config) {
    if (config->stop_bits == 0  || config->stop_bits > 2u) {
        return UART_INVALID_ARGUMENT_STOP_BITS;
    }

    if (config->data_bits < 5u || config-> data_bits > 8u) {
        return UART_INVALID_ARGUMENT_WORD_SIZE;
    }

    /* disable the uart to change the UARTLCR_H, UARTIBRD, and UARTFBRD registers */
    uart0->CR &= ~CR_BIT_UARTEN;

    /*
     * Finish any current transmission, flush the FIFOs.
     * FIFO needs to be disabled only after data transmission is complete
     * else FIFO would loose integrity.
     */
    while (uart0->FR == FR_BIT_BUSY);
    uart0->LCRH &= ~LCRH_BIT_FEN;   /* way to flush transmit queue */

    /* configure baudrate */
    double intpart, fractpart;

    /* https://developer.arm.com/documentation/ddi0183/g/functional-overview/operation/uart-operation */
    /*
     * UART Baud Rate =  ( clock frequency / 16 x baud divisor)
     * Baud divisor =  ( clock frequency / 16 x baud rate)
     */
    double baudrate_divisor = (double)refclock / (16u * config->baudrate);
    fractpart = modf(baudrate_divisor, &intpart);

    uart0->IBRD = (uint16_t)intpart;
    uart0->FBRD = (uint8_t)((fractpart * 64) + 0.5);

    /* configure data bits, parity bits and stop bits */
    uint32_t lcrh = 0u;

    switch (config->data_bits) {
    case 5: lcrh |= LCRH_WLEN_5_BITS; break;
    case 6: lcrh |= LCRH_WLEN_6_BITS; break;
    case 7: lcrh |= LCRH_WLEN_7_BITS; break;
    case 8: lcrh |= LCRH_WLEN_8_BITS; break;
    };

    /* enable parity bits & set parity */
    if (config->parity) {
        lcrh |= LCRH_BIT_EPS;
        lcrh |= LCRH_BIT_PEN;
        lcrh |= LCRH_BIT_SPS;
    } else {
        lcrh &= ~LCRH_BIT_EPS;
        lcrh &= ~LCRH_BIT_PEN;
        lcrh &= ~LCRH_BIT_SPS;
    }

    /* 1 or 2 stop bits */
    if (config->stop_bits == 1u) {
        lcrh &= ~LCRH_BIT_STP2;
    } else {
        lcrh |= LCRH_BIT_STP2;
    }

    /* re-enable FIFOs for better performance */
    lcrh |= LCRH_BIT_FEN;

    /* write modification to LCRH register */
    uart0->LCRH = lcrh;

    /* enable receive interrupt */
    uart0->IMSC = (1u << 4u);

    /* re-enable control register */
    uart0->CR |= CR_BIT_UARTEN;
    
    irq_register_isr(37, uart_isr);

    return UART_OK;
}

void uart_putchar(char c) {
    /* spin while transmit FIFO is full */
    while (uart0->FR & FR_BIT_TXFF);
    uart0->DR = c;  /* put char in the fifo */
}

void uart_write(const char* data) {
    while (*data) {
        uart_putchar(*data++);
    }
}

void uart_getchar(char* c) {
    if (uart0->FR & FR_BIT_RXFE) {
        return;
    }

    *c = uart0->DR & DR_DATA_MASK;
    if (uart0->RSRECR & 0) {
        return;
    }
}