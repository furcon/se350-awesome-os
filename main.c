#include <LPC17xx.h>
#include <uart_polling.h>
#include "rtx.h"

void printInt(int x) {
    while(x) {
        uart0_put_char((x % 10) + '0');
        x /= 10;
    }
}

int main() {
    int* test;
    
    SystemInit();
    uart0_init();
    uart0_put_string("Hello World!\n\r");
    
    k_rtx_init();
    test = k_request_memory_block();
    test = k_request_memory_block();
    test = k_request_memory_block();
    test = k_request_memory_block();
    
    return 0;
}
