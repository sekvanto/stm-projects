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
#include "uartfc_freertos.h"

xSemaphoreHandle uartRcvMutex;

static void EndlessPrint(void *arg) {
    char* s = (char *) arg;
    while (1)
    {
        if (xSemaphoreTake(uartRcvMutex, portMAX_DELAY) == pdTRUE)
        {
            uart_putc(s[0]);
            xSemaphoreGive(uartRcvMutex);
        }
        vTaskDelay(200/portTICK_RATE_MS);
    }
}

static void Second(void *arg) {
    char* s = (char *) arg;
    while (1)
    {
        if (xSemaphoreTake(uartRcvMutex, portMAX_DELAY) == pdTRUE)
        {
            int z = uart_getc();
            uart_putc(z);
            xSemaphoreGive(uartRcvMutex);
        }
        vTaskDelay(500/portTICK_RATE_MS);
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
            "P",                        // Parameter
            tskIDLE_PRIORITY + 1 ,      // Scheduling priority
            NULL                        // Storage for handle (none)
    );

    xTaskCreate(Second, "UART print 2", 128,
                "H", 
                tskIDLE_PRIORITY + 1 , NULL);

    // Start scheduler
    vTaskStartScheduler();

    // Schedule never ends
}

void init_hardware()
{
    uart_open(1, 9600, USART_Mode_Tx);
    uartRcvMutex = xSemaphoreCreateMutex();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif