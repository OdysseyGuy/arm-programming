#include <stdint.h>

volatile uint8_t* uart0 = (uint8_t*)0x10009000;

void uart0_write(const char* str) {
    while (*str) {
        *uart0 = *str++; 
    }
}

int main() {
    const char* message = "Hello from bare-metal!\n";
    uart0_write(message);

    while (1) {};
    return 0;
}