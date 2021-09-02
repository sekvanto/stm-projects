#include <inttypes.h>
#include "nunchuk.h"
#include "i2c.h"

#define NUNCHUK_ADDRESS 0xA4

void nunchuk_init()
{
    I2C_LowLevel_Init(I2C1, 10000, 0x0);
    //I2C_LowLevel_Init(I2C1, 100000, 0x0);

    const uint8_t buf[] = {0xf0, 0x55};
    const uint8_t buf2[] = {0xfb, 0x00};

    I2C_Write(I2C1, buf, 2, NUNCHUK_ADDRESS);
    I2C_Write(I2C1, buf2, 2, NUNCHUK_ADDRESS);
}

// Get state of of wii nunchuk
void get_nunchuk_state(NunchukState *n)
{
    uint8_t data[6];
    const uint8_t buf[] = {0};

    I2C_Write(I2C1, buf, 1, NUNCHUK_ADDRESS);
    I2C_Read(I2C1, data, 6, NUNCHUK_ADDRESS);

    n->c = !(data[5] & 0x2);
    n->z = !(data[5] & 0x1);

    n->jx = data[0];
    n->jy = data[1];

    n->ax = data[2] << 2;
    n->ax |= ((data[5] >> 2) & 3);

    n->ay = data[3] << 2;
    n->ay |= ((data[5] >> 4) & 3);

    n->az = data[4] << 2;
    n->az |= ((data[5] >> 6) & 3);
}