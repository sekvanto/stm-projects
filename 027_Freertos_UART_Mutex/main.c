#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>
#include "FreeRTOSConfig.h"
#include <projdefs.h>
#include <portmacro.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <semphr.h>
#include "uart.h"
#include "xprintf.h"

void myputchar(unsigned char c)
{
    uart_putc(c, USART1);
}

unsigned char mygetchar()
{
    return uart_getc(USART1);
}

xSemaphoreHandle uartRcvMutex;

static void EndlessPrint(void *arg) {
    char* s = (char *) arg;
    while (1)
    {
        if (xSemaphoreTake(uartRcvMutex, portMAX_DELAY) == pdTRUE)
        {
            xprintf(s);
            xSemaphoreGive(uartRcvMutex);
        }
        vTaskDelay(200/portTICK_RATE_MS);
    }
}

static void BlinkLight(void *arg) {
    int dir = 0;
    while (1) {
        // 2 Hz = 500 ms period
        vTaskDelay(500/portTICK_RATE_MS);
        GPIO_WriteBit(GPIOC, GPIO_Pin_8 , dir ? Bit_SET : Bit_RESET);
        dir = 1 - dir;
    }
}

void init_hardware();

int main(void)
{
    // set up interrupt priorities for FreeRTOS !!
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    // initialize hardware
    init_hardware();
    
    // Create tasks
    xTaskCreate(EndlessPrint,           // Function to execute
            "UART print 1",             // Name
            128,                        // Stack size
            "Printing from thread 1\n",   // Parameter
            tskIDLE_PRIORITY + 1 ,      // Scheduling priority
            NULL                        // Storage for handle (none)
    );

    xTaskCreate(EndlessPrint, "UART print 2", 128,
                "Hello, i'm thread 2\n", 
                tskIDLE_PRIORITY + 1 , NULL);

    xTaskCreate(BlinkLight, "Blinking", 128,
                NULL, tskIDLE_PRIORITY + 1, NULL);

    // Start scheduler
    vTaskStartScheduler();

    // Schedule never ends
}

void init_hardware()
{
    GPIO_InitTypeDef Gpio;

    // Enable Peripheral Clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB , ENABLE);

    // Configure pins
    GPIO_StructInit(&Gpio);
    Gpio.GPIO_Pin =  GPIO_Pin_8;
    Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &Gpio);

    uart_open(USART1, 9600, USART_Mode_Tx);

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    uartRcvMutex = xSemaphoreCreateMutex();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif