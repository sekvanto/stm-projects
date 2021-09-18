#include "uartfc.h"

int main()
{
    uart_open(1, 9600, 0); // flags => 0 because the module uses own values
    char buff[10];
    int added = 0;
    while (1) {
        uart_read(1, buff, 10);
        uart_write(1, buff, 10);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif