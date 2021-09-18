#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>
#include <misc.h>

void EXTI0_IRQHandler(void);

int main(void){
    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);

    // Enable Peripheral Clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA , ENABLE);

    // Configure LED and button
    Gpio.GPIO_Pin = GPIO_Pin_8;
    Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &Gpio);

    // Connect EXTI0 to PA0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource0);

    EXTI_InitTypeDef EXTI_InitStructure;

    // Configure EXTI0 line // see stm32f10x_exti.h
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Configure NVIC EXTI0_IRQn ...
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // Configure and enable interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    // No StructInit call in API
    NVIC_InitStructure.NVIC_IRQChannel = 6; // EXTI0_IRQn for STM32F10X_MD_VL
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    while(1) {}
}

static __IO int button_pressed = 1;

void EXTI0_IRQHandler(void)
{ 
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) { 
        GPIO_WriteBit(GPIOC, GPIO_Pin_8, button_pressed);
        button_pressed = 1 - button_pressed;
        EXTI_ClearITPendingBit(EXTI_Line0); 
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
