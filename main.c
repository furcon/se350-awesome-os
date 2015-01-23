#include <LPC17xx.h>
#include <uart_polling.h>
#include "rtx.h"

extern U32 Image$$RW_IRAM1$$ZI$$Limit;

void printInt(int x) {
    while(x) {
        uart0_put_char((x % 10) + '0');
        x /= 10;
    }
}

int main() {
    U32 memoryStart = (unsigned int)&Image$$RW_IRAM1$$ZI$$Limit;
    U32 memoryEnd = 0x10008000;
    
    printInt(memoryStart);
    
    SystemInit();
    uart0_init();
    uart0_put_string("Hello World!\n\r");
    return 0;
}
