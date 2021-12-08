#include <stdio.h>
#include <stm32f10x_usart.h>
#include "uart.h"

int main()
{
    uart_open(USART1, 9600, USART_Mode_Tx);
    printf("hello world\n");
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif