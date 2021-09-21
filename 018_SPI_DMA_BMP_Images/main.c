#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>
#include "ST7735.h"
#include "uart.h"
#include "xprintf.h"
#include "spidma.h"
#include "ff9/src/ff.h"

void myputchar(unsigned char c)
{
    uart_putc(c, USART1);
}

unsigned char mygetchar()
{
    return uart_getc(USART1);
}

void outputString() {
    ST7735_fillScreen(BLACK);
    ST7735_outputString("HELLO WORLD", BLACK, YELLOW);
}

void init()
{
    ST7735_init();
    outputString(); // remove later

    uart_open(USART1, 9600, USART_Mode_Tx | USART_Mode_Rx);
    spiInit(SPI1); // for sd card

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    xprintf("\nStart!!!\n"); // WORKS
}

int sdcard_test()
{
    FATFS Fatfs;
    FRESULT rc;
    FIL Fil;
    UINT bw, br; // bytes written, bytes read
    unsigned char Buff[1000];

    f_mount(0, &Fatfs); // Register volume work area
    
    xprintf("\nOpen an existing file (hello.txt).\n");
    rc = f_open(&Fil, "hello.txt", FA_READ);
    
    if (!rc) {
        xprintf("\nType the file content.\n");
        for (;;) {
            // Read a chunk of file
            rc = f_read(&Fil, Buff, sizeof Buff, &br);
            if (rc || !br) break; // Error or end of file
                for (int i = 0; i < br; i++) // Type the data
                    myputchar(Buff[i]);
        }
        if (rc) return(rc);
        xprintf("\nClose the file.\n");
        rc = f_close(&Fil);
        if (rc) return(rc);
    }

    f_close(&Fil);
    f_mount(0, 0);
}

int main()
{
    init();
    sdcard_test();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif