#ifndef SPI_H
#define SPI_H

void nunchuk_init();
void get_nunchuk_state(int c, int z, uint8_t jx, uint8_t jy, uint16_t ax, uint16_t ay, uint16_t az);

#endif
