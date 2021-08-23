#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "uart.h"

void Delay(uint32_t nTime);

void led_setup() {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable Peripheral Clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);

    // Configure Pins
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int main() {
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);

    uart_open(USART1, 9600, USART_Mode_Rx | USART_Mode_Tx);
    led_setup();

    char* msg = "Hello World!\n\r";

    GPIO_SetBits(GPIOC, GPIO_Pin_9);

    while (1) {
    
 GPIO_SetBits(GPIOC, GPIO_Pin_9);

        for (int i = 0; msg[i] != '\0'; i++) {
            uart_putc(msg[i], USART1);
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
            //Delay(1);
        }
        uart_putc('s', USART1);
        Delay(250);

 GPIO_ResetBits(GPIOC, GPIO_Pin_9);

        Delay(250);
    }
    uart_close(USART1);
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
