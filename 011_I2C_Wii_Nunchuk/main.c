#include "ST7735.h"
#include "nunchuk.h"

int main()
{
    ST7735_init();

    while (1) {
        nunchuk_init();

        int c, z; // buttons c,z
        uint8_t jx, jy; // joystick x,y
        uint16_t ax, ay, az; // accelerometer x,y,z
        get_nunchuk_state(c, z, jx, jy, ax, ay, az);

        // Two cursors: c (C) for joystick, x (X) for accelerometer
        // Capital letter when button "c" is pressed
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif