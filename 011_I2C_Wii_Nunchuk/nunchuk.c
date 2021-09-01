#include <inttypes.h>
#include "i2c.h"

#define NUNCHUK_ADDRESS 0xA4

void nunchuk_init()
{
    const uint8_t buf[] = {0xf0, 0x55};
    const uint8_t buf2[] = {0xfb, 0x00};

    I2C_Write(I2C1, buf, 2, NUNCHUK_ADDRESS);
    I2C_Write(I2C1, buf2, 2, NUNCHUK_ADDRESS);
}

// Get state of c,z buttons, joystick and accelerometer of wii nunchuk
void get_nunchuk_state(int c, int z, uint8_t jx, uint8_t jy, uint16_t ax, uint16_t ay, uint16_t az)
{
    uint8_t data[6];
    const uint8_t buf[] = {0};
    
    I2C_Write(I2C1, buf, 1, NUNCHUK_ADDRESS);
    I2C_Read(I2C1, data, 6, NUNCHUK_ADDRESS);
}