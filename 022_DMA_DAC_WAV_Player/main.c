#include <stm32f10x.h>
#include <stm32f10x_usart.h>
#include "uart.h"
#include "xprintf.h"
#include "spidma.h"
#include "wav.h"
#include "delay.h"
#include "ff9/src/ff.h"

void myputchar(unsigned char c)
{
    uart_putc(c, USART1);
}

unsigned char mygetchar()
{
    return uart_getc(USART1);
}

int main(void)
{
    systick_init();
    spiInit(SPI2);

    uart_open(USART1, 9600, USART_Mode_Tx);

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    xprintf("\nStart\n");

    FATFS Fatfs;
    f_mount(0, &Fatfs);
    while (1)
    {
        int result = wav_play("loop.wav");
    }
    f_mount(0, 0);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
