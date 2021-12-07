#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_dac.h>
#include <stm32f10x_dma.h>
#include <misc.h>
#include "player.h"

#define SYSCLK_FREQ_24MHz 24000000

void DMA1_Channel3_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC3)) { // Transfer complete
        audioplayerWhole = 1;
        DMA_ClearITPendingBit(DMA1_IT_TC3);
    }
    else if (DMA_GetITStatus(DMA1_IT_HT3)) { // Half Transfer Complete
        audioplayerHalf = 1;
        DMA_ClearITPendingBit(DMA1_IT_HT3);
    }
}

void tim_init(unsigned int sampleRate)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // enable timer clock
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;

  	// set timer1 auto-reload value
	// so the timer overflow will occur with a frequency of 24MHz/544=approx. 44.1kHz
    
    TIM_TimeBaseStructure.TIM_Period = SYSCLK_FREQ_24MHz / sampleRate; // 544 with 41.1khz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Configure the TIM3 output trigger so that it occurs on update events
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    // Enable Timer Interrupt , enable timer
    TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);
}

void dma_init()
{   
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel3);

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &DAC->DHR8R1; // DHR12R1
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_BufferSize = AUDIOBUFSIZE;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) Audiobuf;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    // Enable interrupts for complete and half transfers
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC | DMA_IT_HT, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void dac_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC channel 1
    DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T3_TRGO;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void audioplayerInit(unsigned int sampleRate)
{
    tim_init(sampleRate);
    dma_init();
    dac_init();
}

void audioplayerStart()
{
    DMA_Cmd(DMA1_Channel3, ENABLE);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void audioplayerStop()
{
    TIM_Cmd(TIM3, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DAC_Cmd(DAC_Channel_1, DISABLE);
    DAC_DMACmd(DAC_Channel_1, DISABLE);
}