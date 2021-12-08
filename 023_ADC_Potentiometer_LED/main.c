#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_usart.h>
#include "uart.h"
#include "xprintf.h"

void Delay(uint32_t nTime);

void myputchar(unsigned char c)
{
    uart_putc(c, USART1);
}

unsigned char mygetchar()
{
    return uart_getc(USART1);
}

int main()
{
    uart_open(USART1, 9600, USART_Mode_Tx);

    xfunc_in = mygetchar;
    xfunc_out = myputchar;

    // Configure the clocks for GPIOA, GPIOC, and the ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);

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

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Configure ADC_IN6
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);

    // Enable ADC
    ADC_Cmd(ADC1, ENABLE);

    // Check the end of ADC1 reset calibration register
    while(ADC_GetResetCalibrationStatus(ADC1));

    // Start ADC1 calibration
    ADC_StartCalibration(ADC1);

    // Check the end of ADC1 calibration
    while(ADC_GetCalibrationStatus(ADC1));

    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (1)
    {
        uint16_t ain = ADC1->DR;
        xprintf("ADC: %d\n", ain);
        if (ain > (0xFFF / 2))
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_9);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_9);
        }
        Delay(1);
    }
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

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif