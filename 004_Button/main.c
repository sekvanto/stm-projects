#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

void Delay(uint32_t nTime);

int main(void){
    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);

    // Enable Peripheral Clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA , ENABLE);

    // Configure LED
    Gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &Gpio);

    // Configure button
    //Gpio.GPIO_Pin = GPIO_Pin_0;
    //Gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    //GPIO_Init(GPIOA, &Gpio);
    
    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);
    
    //GPIO_WriteBit (GPIOC,  GPIO_Pin_8, Bit_SET);
    //Delay(1000);

    while(1) {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
            GPIO_ResetBits ( GPIOC,  GPIO_Pin_9 );
        else
            GPIO_SetBits ( GPIOC,  GPIO_Pin_9 );
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
