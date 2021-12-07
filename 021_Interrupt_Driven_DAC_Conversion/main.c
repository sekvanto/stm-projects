#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_dac.h>
#include <misc.h>
#include "sine_wave.c"

void TIM3_IRQHandler(void);
void tim_init();
void dac_init();

int main(void){

    // Configure clocks for TIM3 and DAC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_DAC, ENABLE);

    // Configure NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    dac_init();
    tim_init();

    // Configure and enable TIM2 interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    // No StructInit call in API
    NVIC_InitStructure.NVIC_IRQChannel = 29; // TIM3_IRQn for STM32F10X_MD_VL
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable Timer Interrupt , enable timer
    TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);
    TIM_Cmd(TIM3, ENABLE);
    
    while(1) { /* do nothing */ }
}

static __IO uint16_t waveArrIndex = 0;

void TIM3_IRQHandler(void) // Is triggered on every tick
{
    if (waveArrIndex >= ARR_SIZE) {
        waveArrIndex = 0;
    }
    DAC_SetChannel1Data(DAC_Align_12b_R, a441[waveArrIndex]);
    waveArrIndex++;
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

void tim_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // enable timer clock
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;

  	// set timer1 auto-reload value
	// so the timer overflow will occur with a frequency of 24MHz/544=approx. 44.1kHz
    
    TIM_TimeBaseStructure.TIM_Period = 544; //544; //450
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Configure the TIM3 output trigger so that it occurs on update events
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}

void dac_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC , ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC channel1 Configuration

    DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T3_TRGO;
    DAC_Init(DAC_Channel_1 , &DAC_InitStructure);

    // Enable DAC

    DAC_Cmd(DAC_Channel_1 , ENABLE);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
