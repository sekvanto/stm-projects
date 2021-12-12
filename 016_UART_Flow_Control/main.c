#include <stm32f10x.h>
#include "uartfc.h"

void Delay(uint32_t nTime);

int main()
{
    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);


    uart_open(1, 9600, 0); // flags => 0 because the module uses own values
    const int size = 10;
    char buff[size];
    int added = 0;
    while (1) {
        //uart_read(1, buff, size);
        //uart_write(1, "\r\n", 2);
        //uart_write(1, buff, size);
        uart_write(1, "hello\r\n", 8);
        Delay(10);
        //uart_putchar('h');
        //char z = uart_getchar();
        //uart_putchar(z);
    }
}

// Timer code
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

void SysTick_Handler(void){
    if(TimingDelay != 0x00)
        TimingDelay --;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif