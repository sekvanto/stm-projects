#ifndef SPI_H
#define SPI_H

#define JOYSTICK_MAX 255
#define JOYSTICK_MAX_DYNAMIC 220
#define JOYSTICK_MIN_DYNAMIC 30
#define JOYSTICK_CENTER 128

#define ACCELEROMETER_MAX 1023

typedef struct {
    int c; // buttons, 1 - pressed
    int z;
    uint8_t jx; // joystick
    uint8_t jy;
    uint16_t ax; // acelerometer
    uint16_t ay;
    uint16_t az;
} NunchukState;

void nunchuk_init();
void get_nunchuk_state(NunchukState *n);

#endif
