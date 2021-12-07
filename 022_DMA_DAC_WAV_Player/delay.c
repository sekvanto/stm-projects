#include <stm32f10x.h>
#include "delay.h"

void systick_init()
{
    SysTick_Config(SystemCoreClock / 1000);
}

static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime)
{
    TimingDelay = nTime;
    while (TimingDelay != 0);
}

void SysTick_Handler(void)
{
    if (TimingDelay != 0x00)
        TimingDelay--;
}