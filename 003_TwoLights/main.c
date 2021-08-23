#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

void Delay(uint32_t nTime);

int main(void){
    GPIO_InitTypeDef Gpio;

    // Enable Peripheral Clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB , ENABLE);

    // Configure pins
    GPIO_StructInit(&Gpio);
    Gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &Gpio);

    // Configure SysTick Timer
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);

    while(1) {
        static int greenledval = 0;
        static int blueledval = 1;

        // toggle led
        GPIO_WriteBit(GPIOC, GPIO_Pin_9 , (greenledval) ?  Bit_SET : Bit_RESET);
        GPIO_WriteBit(GPIOC, GPIO_Pin_8 , (blueledval) ? Bit_SET : Bit_RESET);
        greenledval = 1 - greenledval;
        blueledval = 1 - blueledval;
    
        Delay(1000);  // wait 1000ms
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
