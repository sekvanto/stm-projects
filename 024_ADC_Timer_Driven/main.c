#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_tim.h>
#include <misc.h>

void ADC1_IRQHandler(void);
void tim_init();

int main()
{
    // Configure the clocks for GPIOA, GPIOC, and the ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);

    // Configure NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // Configure port PA6 as analog input
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure port PC9 as push/pull output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    tim_init();

    // Configure and enable TIM3 interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    // No StructInit call in API
    NVIC_InitStructure.NVIC_IRQChannel = 18; // ADC1_IRQn for STM32F10X_MD_VL
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Configure ADC_IN6
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    // Enable ADC
    ADC_Cmd(ADC1, ENABLE);

    // Check the end of ADC1 reset calibration register
    while(ADC_GetResetCalibrationStatus(ADC1));

    // Start ADC1 calibration
    ADC_StartCalibration(ADC1);

    // Check the end of ADC1 calibration
    while(ADC_GetCalibrationStatus(ADC1));

    // Enable Timer Interrupt , enable timer
    TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    while (1) { /* do nothing */ }
}

void ADC1_IRQHandler(void)
{
    uint16_t ain = ADC1->DR;
    if (ain > (0xFFF / 2))
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_9);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    }
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
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
	// so the timer overflow will occur with a frequency of 24MHz/24000 = 1000 Hz (1 ms)
    
    TIM_TimeBaseStructure.TIM_Period = 24000;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Configure the TIM3 output trigger so that it occurs on update events
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif