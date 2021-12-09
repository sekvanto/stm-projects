#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <misc.h>
#include "FreeRTOSConfig.h"
#include <projdefs.h>
#include <portmacro.h>
#include <FreeRTOS.h>
#include <timers.h>

static void Thread1(void *arg) {
    int dir = 0;
    while (1) {
        vTaskDelay(300/portTICK_RATE_MS);
        GPIO_WriteBit(GPIOC, GPIO_Pin_9 , dir ? Bit_SET : Bit_RESET);
        dir = 1 - dir;
    }
}

static void Thread2(void *arg) {
    int dir = 0;
    while (1) {
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
    xTaskCreate(Thread1,           // Function to execute
            "Thread 1",            // Name
            128,                   // Stack size
            NULL,                  // Parameter (none)
            tskIDLE_PRIORITY + 1 , // Scheduling priority
            NULL                   // Storage for handle (none)
    );

    xTaskCreate(Thread2, "Thread 2", 128,
                NULL, tskIDLE_PRIORITY + 1 , NULL);

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
    Gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &Gpio);

}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif