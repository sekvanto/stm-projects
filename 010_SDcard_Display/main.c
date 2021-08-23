#include "stm32f10x.h"
#include "mmcbb.c"
#include "ff9/src/ff.h"
#include "xprintf.h"
#include "uart.h"

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
    xprintf("\nStart!!!\n");

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    FATFS Fatfs;
    FRESULT rc;
    FIL Fil;
    UINT bw;

    configure_sys_tick();

    f_mount(0, &Fatfs); /* Register volume work area */
    
    /*
    xprintf("\nOpen an existing file (message.txt).\n");
    rc = f_open(&Fil, "MESSAGE.TXT", FA_READ);
    
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
    */

    xprintf("\nCreate a new file (hello.txt).\n");
    rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) return(rc);

    xprintf("\nWrite a text data. (Hello world!)\n");
    rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);
    if (rc) return(rc);
    xprintf("%u bytes written.\n", bw);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif