#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>
#include <projdefs.h>
#include <FreeRTOS.h>
#include <queue.h>
#include "uartfc_freertos.h"

#define QUEUE_SIZE 100
#define HIGH_WATER (QUEUE_SIZE - 6)

int RxOverflow = 0;

// TxPrimed is used to signal that Tx send buffer needs to be primed
// to commence sending -- it is cleared by the IRQ, set by uart_write

static int TxPrimed = 0;

xQueueHandle UART1_TXq, UART1_RXq;

int uart_open (uint8_t uart, uint32_t baud, uint32_t flags)
{
  USART_InitTypeDef USART_InitStructure; 
  GPIO_InitTypeDef GPIO_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;

  if (uart == 1) {
    
    // get things to a known state

    USART_DeInit(USART1);

    // Turn on clocks

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  |
			   RCC_APB2Periph_AFIO |
			   RCC_APB2Periph_USART1,
			   ENABLE);

    // Configure TX pin

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure RX pin

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure CTS pin

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure RTS pin -- software controlled

    GPIO_WriteBit(GPIOA, GPIO_Pin_12, 1);          // nRTS disabled
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure NVIC
  
    /* Enable the USART1 Interrupt */

    NVIC_InitStructure.NVIC_IRQChannel = 37; // USART1_IRQn
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = (uint8_t)(configKERNEL_INTERRUPT_PRIORITY >> 4);
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    // Configure the UART

    USART_StructInit(&USART_InitStructure); 
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
    USART_InitStructure.USART_Mode  = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1,&USART_InitStructure); 


    // Enable RX Interrupt.  TX interrupt enabled in send routine

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


    // Enable Usart1

    USART_Cmd(USART1, ENABLE); 

    // nRTS enabled

    GPIO_WriteBit(GPIOA, GPIO_Pin_12, 0);     

    UART1_RXq = xQueueCreate(QUEUE_SIZE, 1);
    UART1_TXq = xQueueCreate(QUEUE_SIZE, 1);     

    return 0;
  } 
  return 1;  // only handle UART1
}


int uart_close(uint8_t uart)
{

}

int uart_putc(int c){
  xQueueSend(UART1_TXq , &c, portMAX_DELAY);
  // kick the transmitter interrupt
  USART_ITConfig(USART1, USART_IT_TXE , ENABLE);
  return 0;
}

int uart_getc (){
  int8_t buf;
  xQueueReceive(UART1_RXq , &buf, portMAX_DELAY);
  return buf;
}

void USART1_IRQHandler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
  
    uint8_t data;

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    
    data = USART_ReceiveData(USART1) & 0xff;
    if (xQueueSendFromISR(UART1_RXq , &data, &xHigherPriorityTaskWoken) != pdTRUE)
      RxOverflow = 1;
  }

  if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
    uint8_t data;

    if (xQueueReceiveFromISR(UART1_TXq , &data, &xHigherPriorityTaskWoken) == pdTRUE){
      USART_SendData(USART1, data);
    }
    else {
      // turn off interrupt
      USART_ITConfig(USART1, USART_IT_TXE , DISABLE);
    }
  }

  // Cause a scheduling operation if necessary
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}