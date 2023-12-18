#include "cpu_a9.h"
#include "uart_pl011.h"
#include "gic.h"

int main() {
    /* 9600 8N1 */
    uart_config_t config = {
        .baudrate = 9600,
        .data_bits = 8,
        .parity = false,
        .stop_bits = 1,
    };
    uart_configure(&config);

    uart_write("Hello from bare metal world!\n");
    uart_write("Writing to UART working!\n");
    uart_write("I love drivers\n");

    /* gic controller initilization */
    gic_init();
    /* enable uart interrupt */
    gic_enable_interrupt(37);
    /* re-enable interrupts globally */
    cpu_enable_interrupts();

    while (1) {};
    return 0;
}