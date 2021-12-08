#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "uart.h"

int uart_open(USART_TypeDef* USARTx, uint32_t baud, uint32_t flags)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
                           RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_GPIOA , ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    // Initialize USART1_TX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize USART1_RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStructure;

    // Initialize USART structure
    USART_StructInit(&USART_InitStructure);

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_Mode  = flags;
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE);
}

int uart_close(USART_TypeDef* USARTx)
{
    USART_DeInit(USARTx);
}

int uart_putc(int c, USART_TypeDef* USARTx)
{
    //USART_SendData(USARTx, c);
    USART1->DR =  (c & 0xff);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return 0;
}

int uart_getc(USART_TypeDef* USARTx)
{
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return USART1->DR & 0xff;
}
