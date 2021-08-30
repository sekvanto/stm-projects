#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "uart.h"
#include "spi.h"
#include "xprintf.h"
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
    uart_open(USART1, 9600, USART_Mode_Tx | USART_Mode_Rx);
    spiInit(SPI2);

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    xprintf("\nStart!!!\n"); // WORKS

    FATFS Fatfs;
    FRESULT rc;
    FIL Fil;
    UINT bw, br; // bytes written, bytes read
    unsigned char Buff[1000];

    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);

    f_mount(0, &Fatfs); // Register volume work area
    

    //*

    xprintf("\nOpen an existing file (hello.txt).\n");
    rc = f_open(&Fil, "HELLO.TXT", FA_READ);
    
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

    //*/


    /*
    
    xprintf("\nCreate a new file (hello.txt).\n");
    rc = f_open(&Fil, "HELLO.TXT", FA_OPEN_EXISTING | FA_WRITE);
    if (rc) return(rc);

    xprintf("\nWrite a text data. (Hello sek!)\n");
    char text[] = "\r\nHello sek!\r\n";
    rc = f_write(&Fil, text, sizeof(text), &bw);
    if (rc) return(rc);
    xprintf("%u bytes written.\n", bw);

    */

    f_close(&Fil);
    f_mount(0, 0);
    
    uart_close(USART1);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif