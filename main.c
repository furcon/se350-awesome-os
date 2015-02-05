#include <LPC17xx.h>
#include <uart_polling.h>
#include "rtx.h"

void printInt(int x) {
    unsigned char digits[11]="          ";//10 num digits, 1 null delim \0
    int index=9;
    while(x) { 
        digits[index--]=x%10 + '0'; 
        x/=10;
    }
    for (x=0; x<10; ++x){
        if (digits[x]!=' ')
            uart0_put_char(digits[x]);
    }
    uart0_put_string("\n\r");
}
void nullproc(void) { while (1) release_processor(); }
void print1(void) {while (1) { printInt(1); k_release_processor();} }
void print2(void) {while (1) { printInt(2); k_release_processor();} }
int main() {
    int* test;
    
    void (*procs[])()={nullproc,print1,print2,print1,print1,
                       print2,print2,print2,print2,print2};
    int prios[]={4,0,1,1,1,0,1,1,1,1};
    
    SystemInit();
    uart0_init();
    uart0_put_string("Hello World!\n\r");
    printInt(12251212);
    printInt(12251212);
    printInt(12251212);
     
    initProcs(procs,prios);
    k_rtx_init();
    test = k_request_memory_block();
    test = k_request_memory_block();
    test = k_request_memory_block();
    test = k_request_memory_block();
    
    return 0;
}
