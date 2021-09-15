#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>

static void trigger_init()
{
    // reconfigure PB9 to alternative function push-pull
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);
    
    Gpio.GPIO_Pin = GPIO_Pin_9;
    Gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &Gpio);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // enable timer clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
    
    // configure timer
    // PWM frequency = 10 hz with 24,000,000 hz system clock

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/100000 - 1; // 10 hz or 100 ms cycle
    TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 0..9999
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // PWM1 Mode configuration: Channel4
    // Edge-aligned; not single pulse mode

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);

    // Enable Timer
    TIM_Cmd(TIM4, ENABLE);

    // Set 10 μs trigger
    // Full period length = 100 ms
    // Resolution = 0...9999

    TIM_SetCompare4(TIM4, 1);
}

/*
static void echo_init()
{
    // reconfigure PA8 to alternative function push-pull
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);
    
    Gpio.GPIO_Pin = GPIO_Pin_8;
    Gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    //Gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &Gpio);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // enable timer clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

    // configure timer
    // PWM frequency = 10 hz with 24,000,000 hz system clock

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/100000 - 1; // 10 hz or 100 ms cycle
    //TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 0..9999
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // Channel 1 latches the timer on a rising input on t1
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = 0;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    // Channel 2 latches the timer on a falling input on t2
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = 0;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    // Enable Timer
    //TIM_Cmd(TIM1, ENABLE);

    // Configure the timer slave mode with TI1FP1 as reset signal
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

    // Enable Timer
    TIM_Cmd(TIM1, ENABLE);
}
*/

// Here i tried to utilize
static void echo_init()
{
    // reconfigure PA1 to alternative function push-pull
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);
    
    Gpio.GPIO_Pin = GPIO_Pin_1;
    Gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    //Gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &Gpio);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // enable timer clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

    // configure timer
    // PWM frequency = 10 hz with 24,000,000 hz system clock

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/100000 - 1; // 10 hz or 100 ms cycle
    //TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 0..9999
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Channel 1 latches the timer on a rising input on t1
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = 0;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    // Channel 2 latches the timer on a falling input on t2
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = 0;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    // Enable Timer
    //TIM_Cmd(TIM1, ENABLE);

    // Configure the timer slave mode with TI1FP1 as reset signal
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

    // Enable Timer
    TIM_Cmd(TIM2, ENABLE);
}

void ultrasonic_init()
{
    // Configure SysTick Timer
    //if(SysTick_Config(SystemCoreClock / 1000))
    //    while(1);

    trigger_init();
    echo_init();
}

// Returns distance in mm
int ultrasonic_get_distance()
{
    //while(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == RESET);
    //TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    //return (TIM_GetCapture2(TIM1) - TIM_GetCapture1(TIM1)) * 17/100;

    //return (TIM_GetCapture2(TIM2) - TIM_GetCapture1(TIM2)) * 17/100;

    //return TIM1->CNT;
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