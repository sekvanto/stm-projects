#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>
#include "spi.h"

static const uint16_t speeds[] = {
    [SPI_SLOW] = SPI_BaudRatePrescaler_64,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void spiInit(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);
    SPI_StructInit(&SPI_InitStructure);

    if (SPIx == SPI1) {
        /* Enable clocks, configure pins...*/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

        // Configure SCK and MOSI
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Configure MISO
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else {
        return;
    }

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_SLOW];
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Cmd(SPIx, ENABLE);
}

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, const uint8_t *tbuf, int cnt, enum spiSpeed speed)
{
    int i;
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];
    for (i = 0; i < cnt; i++){
        if (tbuf) {
            SPI_I2S_SendData(SPIx, *tbuf++);
        } else {
            SPI_I2S_SendData(SPIx, 0xff);
        }
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if (rbuf) {
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        } else {
            SPI_I2S_ReceiveData(SPIx);
        }
    }
    return i;
}

int spiReadWrite16(SPI_TypeDef* SPIx, uint16_t *rbuf, const uint16_t *tbuf, int cnt, enum spiSpeed speed)
{
    int i;
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];
    SPI_DataSizeConfig(SPIx, SPI_DataSize_16b);

    for (i = 0; i < cnt; i++){
        if (tbuf) {
            SPI_I2S_SendData(SPIx, *tbuf++);
        } else {
            SPI_I2S_SendData(SPIx, 0xffff);
        }
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if (rbuf) {
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        } else {
            SPI_I2S_ReceiveData(SPIx);
        }
    }
    SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);
    return i;
}
