#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_dma.h>
#include "spidma.h"

#define MIN_DMA_BLOCK 4 + 1

void spiInit(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);
    SPI_StructInit(&SPI_InitStructure);

    if (SPIx == SPI2) {
        /* Enable clocks, configure pins...*/
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        // Configure SCK and MOSI
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        // Configure MISO
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    } else {
        /* Enable clocks, configure pins...*/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

        // Configure SCK and MOSI
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Configure MISO
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }

    // DMA clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_FAST];
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Cmd(SPIx, ENABLE);
}

/**
 * half = 0 => 16 bit
 * half = 1 => 8  bit transfer size
 * 
 * exchange datablock by spi dma
 */
static int xchng_datablock(SPI_TypeDef *SPIx, int half, const void *tbuf, void *rbuf, unsigned count)
{
    DMA_Channel_TypeDef *rxChan, *txChan;
    uint32_t rxFlag, txFlag;

    if (SPIx == SPI1) {
        rxChan = DMA1_Channel2;
        txChan = DMA1_Channel3;

        rxFlag = DMA1_FLAG_TC2;
        txFlag = DMA1_FLAG_TC3;
    } else {
        rxChan = DMA1_Channel4;
        txChan = DMA1_Channel5;

        rxFlag = DMA1_FLAG_TC4;
        txFlag = DMA1_FLAG_TC5;
    }

    DMA_InitTypeDef DMA_InitStructure;
    
    DMA_DeInit(rxChan);
    DMA_DeInit(txChan);

    // Common to both channels
    
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPIx->DR));
    DMA_InitStructure.DMA_PeripheralDataSize = (half) ? DMA_PeripheralDataSize_Byte : DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = (half) ? DMA_PeripheralDataSize_Byte : DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_BufferSize = count;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    // Rx Channel
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rbuf;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_Init(rxChan, &DMA_InitStructure);
    
    // Tx channel
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)tbuf;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_Init(txChan, &DMA_InitStructure);

    // Enable channels
    
    DMA_Cmd(rxChan, ENABLE);
    DMA_Cmd(txChan, ENABLE);

    // Enable SPI TX/RX request
    SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx , ENABLE);

    // Wait for completion
    
    if (tbuf) {
        while (DMA_GetFlagStatus(txFlag) == RESET);
    } 
    if (rbuf) {
        while (DMA_GetFlagStatus(rxFlag) == RESET);
    }

    // Disable channels

    DMA_Cmd(rxChan, DISABLE);
    DMA_Cmd(txChan, DISABLE);
    SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx , DISABLE);
    
    return count;
}

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, const uint8_t *tbuf, int cnt, enum spiSpeed speed)
{
    if (cnt >= MIN_DMA_BLOCK) {
        return xchng_datablock(SPIx, 1, tbuf, rbuf, cnt);
    }

    int i;
    //SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];
    SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);

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

    if (cnt >= MIN_DMA_BLOCK) {
        int res = xchng_datablock(SPIx, 0, tbuf, rbuf, cnt);
        SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);
        return res;
    }

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
