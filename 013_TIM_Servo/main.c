#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include "delay.c"
#include "servo.h"

int main()
{
    servo_init();

    //TIM_SetCompare2(TIM2, 50); // one single turn

    int pw = 0; // pulse width, range 0...999
    int step = -1; // alternates positive and negative
    while (1)
    {
        if (pw == 100)
            pw = 0;
        pw++;
        TIM_SetCompare2(TIM2, pw);
        Delay(50);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif