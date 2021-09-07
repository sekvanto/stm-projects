#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include "delay.c"
#include "servo.h"
#include "nunchuk.h"

// only 180 degrees rotation is fine for this exercise
void endless_loop_rotation()
{
    int pw = 30; // pulse width, range 30...130
    int step = 1; // alternates direction
    
    while (1) {
        if (pw == 130)
            step = -1;
        if (pw == 30)
            step = 1;

        pw += step;
        TIM_SetCompare2(TIM2, pw);
        Delay(50);
    }
}

// Uses joystick X coordinate to calculate pulse width
uint16_t get_pw(uint8_t jx)
{
    // range conversion: from 0...255 to 0...100
    uint16_t result = (100 * jx) / JOYSTICK_MAX;

    // range conversion: from 0...100 to 30...130
    return result + 30;
}

int main()
{
    servo_init();
    nunchuk_init();
    NunchukState n;

    //TIM_SetCompare2(TIM2, 50); // one single turn

    while (1) {
        get_nunchuk_state(&n);
        uint16_t pw = get_pw(n.jx);
        TIM_SetCompare2(TIM2, pw);
    }    
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif