#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <inttypes.h>

// Fpwm = Fclk / (arr+1) * (psc+1)
//uint16_t calculate_psc(int Fpwm, int Fclk, int arr)
//{
//    return ((Fclk / Fpwm) / (arr+1)) - 1;
//}

void servo_init()
{
    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);


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
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/50000 - 1; // 50 hz or 20 ms cycle
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

// 0-90 degrees
void servo_rotate(int degrees)
{
    // 0 degrees (full left) = 1ms
    // 90 degrees (full right) = 2ms

    // 50...100 steps
    int pw = ((50 * degrees) / 90) + 50;
    TIM_SetCompare2(TIM2, pw);
}