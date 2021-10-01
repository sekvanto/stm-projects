#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <misc.h>

void TIM2_IRQHandler(void);
void tim_init();

int main(void){

    // Configure clocks for GPIOC and TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Configure NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // Configure timer
    tim_init();

    // Configure green LED on PC9
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure and enable TIM2 interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    // No StructInit call in API
    NVIC_InitStructure.NVIC_IRQChannel = 28; // TIM2_IRQn for STM32F10X_MD_VL
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable Timer Interrupt , enable timer
    TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    //TIM_SetCompare2(TIM2, 1);
    
    while(1) { /* do nothing */ }
}

// 500 ticks need to complete 500 ms
// 250 cycles
// Each cycle is 2 ms, contains 2 ticks 
static __IO uint16_t TimeLeftBeforeBlink = 500; // 250 = bit_set, 0 = bit_reset

void TIM2_IRQHandler(void) // Is triggered on every tick
{
    if (TimeLeftBeforeBlink == 250) {
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
        TimeLeftBeforeBlink--;
    }
    else if (TimeLeftBeforeBlink == 0) {
        TimeLeftBeforeBlink = 500; // Reloading
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
    }
    else
        TimeLeftBeforeBlink--;
        
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

void tim_init()
{
    // reconfigure PA1 to alternative function push-pull
    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);
    
    Gpio.GPIO_Pin = GPIO_Pin_1;
    Gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &Gpio);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // enable timer clock
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    
    // configure timer
    // PWM frequency = 1 hz with 24,000,000 hz system clock

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/1000 - 1;
    TIM_TimeBaseStructure.TIM_Period = 2 - 1; // 0..1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // PWM1 Mode configuration: Channel2
    // Edge-aligned; not single pulse mode

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
