#include <math.h>
#include "ST7735.h"
#include "nunchuk.h"

#define MAX_ATAN (M_PI / 2)

void get_joystick_coords(NunchukState *n, int *jx, int *jy) {
    *jx = ST7735_width * n->jx / JOYSTICK_MAX;
    *jy = ST7735_height * n->jy / JOYSTICK_MAX;

    // reverse height
    *jy = ST7735_height - *jy;

    if (*jx > (ST7735_width - CHAR_CONTAINER_WIDTH))
        *jx = ST7735_width - CHAR_CONTAINER_WIDTH;
    
    if (*jy > (ST7735_height - CHAR_CONTAINER_HEIGHT))
        *jy = ST7735_height - CHAR_CONTAINER_HEIGHT;
}

// generates cursor screen coordinates based on acceleration
// the faster you move the controller, the faster the cursor moves
void get_acceleration_coords(NunchukState *n, int *ax, int *ay) {
    *ax = ST7735_width * n->ax / ACCELEROMETER_MAX;
    *ay = ST7735_height * n->ay / ACCELEROMETER_MAX;

    if (*ax > (ST7735_width - CHAR_CONTAINER_WIDTH))
        *ax = ST7735_width - CHAR_CONTAINER_WIDTH;
    
    if (*ay > (ST7735_height - CHAR_CONTAINER_HEIGHT))
        *ay = ST7735_height - CHAR_CONTAINER_HEIGHT;
}

// generates screen coordinates based on tilt, doesn't work yet
/**
void get_tilt_coords(NunchukState *n, int *ax, int *ay) {
    // range conversion from 0..1023 to -512..511
    int rax = n->ax - 512;
    int ray = n->ay - 512;
    int raz = n->az - 512;

    float pitch = atan(rax / sqrt(ray*ray + raz*raz));
    float roll = atan(ray / sqrt(rax*rax + raz*raz));

    // range conversion from -π/2...π/2 to 0...1
    pitch = (pitch + MAX_ATAN) / 2*MAX_ATAN;
    roll = (roll + MAX_ATAN) / 2*MAX_ATAN;

    *ax = (int) (pitch * (float) ST7735_width);
    *ay = (int) (roll * (float) ST7735_height);
}
*/

int main()
{
    ST7735_init();
    ST7735_fillScreen(BLACK);

    nunchuk_init();
    NunchukState n;

    int jx = 0, jy = 0; // screen coordinates of 'c' cursor
    int ax = 0, ay = 0; // screen coordinates of 'x' cursor

    while (1) {
        get_nunchuk_state(&n);

        uint8_t c = 'c';
        uint8_t x = 'x';

        // Two cursors: c (C) for joystick, x (X) for accelerometer
        // Capital letter when button "c" is pressed

        // Button check
        if (n.c)
            c = 'C';
        if (n.z)
            x = 'X';

        int prev_x = jx, prev_y = jy;

        // Joystick manipulation
        get_joystick_coords(&n, &jx, &jy);
        if (prev_x != jx || prev_y != jy)
            ST7735_outputCharacter(' ', prev_x, prev_y, BLACK, BLACK); // Clearing previous coordinates
        ST7735_outputCharacter(c, jx, jy, BLACK, YELLOW);

        prev_x = ax, prev_y = ay;

        // Accelerometer cursor manipulation
        get_acceleration_coords(&n, &ax, &ay);
        //get_tilt_coords(&n, &ax, &ay);
        if (prev_x != ax || prev_y != ay)
            ST7735_outputCharacter(' ', prev_x, prev_y, BLACK, BLACK); // Clearing previous coordinates
        ST7735_outputCharacter(x, ax, ay, BLACK, RED);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif