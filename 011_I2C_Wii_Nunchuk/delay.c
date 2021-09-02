#ifndef DELAY_H
#define DELAY_H

#include <inttypes.h>
#include <stm32f10x.h>

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

#endif