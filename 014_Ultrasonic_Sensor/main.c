#include <stm32f10x_usart.h>
#include "xprintf.h"
#include "uart.h"
#include "ultrasonic.h"

void myputchar(unsigned char c)
{
    uart_putc(c, USART2);
}

unsigned char mygetchar()
{
    return uart_getc(USART2);
}

int main()
{
    uart_open(USART2, 9600, USART_Mode_Tx);

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    xprintf("Ultrasonic sensor\n\r");

    ultrasonic_init();

    while (1) {
        int distance_mm = ultrasonic_get_distance();
        xprintf("%d mm\n\r", distance_mm);
    }

    uart_close(USART1);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif