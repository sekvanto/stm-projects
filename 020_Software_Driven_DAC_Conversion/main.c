#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_dac.h>
#include "sine_wave.c"

void Delay(uint32_t nTime);
void dac_init();

int main(void){

    dac_init();

    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);

    while(1) {
        for (int i = 0; i < ARR_SIZE; i++) {
            DAC_SetChannel1Data(DAC_Align_12b_R, a441[i]);
            Delay(1);
        }
    }
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
    DAC_Init(DAC_Channel_1 , &DAC_InitStructure);

    // Enable DAC

    DAC_Cmd(DAC_Channel_1 , ENABLE);
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
