#ifndef SPI_H
#define SPI_H

#include <stm32f10x_spi.h>

enum spiSpeed { SPI_SLOW , SPI_MEDIUM, SPI_FAST };

static const uint16_t speeds[] = {
    [SPI_SLOW] = SPI_BaudRatePrescaler_64,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void spiInit(SPI_TypeDef* SPIx);
int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, 
		 const uint8_t *tbuf, int cnt, 
		 enum spiSpeed speed);
int spiReadWrite16(SPI_TypeDef* SPIx, uint16_t *rbuf, 
                   const uint16_t *tbuf, int cnt, 
                   enum spiSpeed speed);
#endif