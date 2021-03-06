#include <stm32f10x.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
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

    const int buff_length = 100;
    char msg[buff_length];
    int msg_len = 0;
    while (1) {
        char rx = uart_getc(USART1);
        if (rx == 'd')
            GPIO_SetBits(GPIOC, GPIO_Pin_9);
        if (rx == '\n' || rx == '\r') { // String is full?
            for (int i = 0; i < msg_len; i++) { // Echoing it
                uart_putc(msg[i], USART1);
            }
            uart_putc('\n', USART1);
            uart_putc('\r', USART1);
            msg_len = 0;
        }
        else { // Otherwise, adding new character to buffer
            if (msg_len == buff_length) {
                msg_len = 0;
            }
            msg[msg_len] = rx;
            msg_len++;
        }
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
