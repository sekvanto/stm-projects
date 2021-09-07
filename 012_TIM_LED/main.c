#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include "ST7735.h"
//#include "delay.c"

// Initializing timer on PA1
void tim_init()
{

    // reconfigure PA1 to alternative function push-pull

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);
    
    Gpio.GPIO_Pin = GPIO_Pin_1;
    Gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    //Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    //Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &Gpio);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // enable timer clock
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    
    // configure timer
    // PWM frequency = 100 hz with 24,000,000 hz system clock
    // 24,000,000/240 = 100,000
    // 100,000/1000 = 100

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/100000 - 1; // 0..239
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 0..999
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // PWM1 Mode configuration: Channel2
    // Edge-aligned; not single pulse mode

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    // Enable Timer

    TIM_Cmd(TIM2, ENABLE);
}

int main()
{
    tim_init();
    ST7735_init();
    ST7735_fillScreen(MAGENTA);
    int pw = 999; // pulse width, range 0...999
    int step = -1; // alternates positive and negative
    while (1)
    {
        if (pw == 0)
            step = 1;
        if (pw == 999)
            step = -1;

        pw += step;
        TIM_SetCompare2(TIM2, pw);
        Delay(2);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif