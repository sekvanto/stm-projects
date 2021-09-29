#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "uart.h"
#include "spi.h"
#include "xprintf.h"
#include "ff9/src/ff.h"
#include "ST7735.h"
#include "bmp.h"

void myputchar(unsigned char c)
{
    uart_putc(c, USART1);
}

unsigned char mygetchar()
{
    return uart_getc(USART1);
}

void loop_through_images() {
    DIR Dir;
    FRESULT res;
    FILINFO fno;
    FATFS Fatfs;

    f_mount(0, &Fatfs); // Register volume work area

    f_opendir(&Dir, "/");
    for (;;) {
        res = f_readdir(&Dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
        if (fno.fattrib & AM_DIR) {                    /* It is a directory */
            continue;
        } else {
            bmp_display(fno.fname);
            Delay(250);
        }
    }
    f_mount(0, 0);
}

int main(void)
{
    ST7735_init();

    uart_open(USART1, 9600, USART_Mode_Tx | USART_Mode_Rx);
    spiInit(SPI2); // Display = SPI1, sdCard = SPI2

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    xprintf("\nStart\n"); // works

    while (1) {
        loop_through_images();
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif