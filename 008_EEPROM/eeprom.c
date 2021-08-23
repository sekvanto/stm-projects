#include <stdint.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include "spi.h"

#define WIP(x) ((x) & 1)

#define EEPROM_PORT  GPIOC
#define EEPROM_CS    GPIO_Pin_10
#define EEPROM_SPI   SPI2
#define EEPROM_SPEED speeds[SPI_SLOW]

#define PAGE_SIZE    16 // B
 
enum eepromCMD   { cmdREAD  = 0x03, cmdWRITE = 0x02,
                   cmdWREN  = 0x06, cmdWRDI  = 0x04,
                   cmdRDSR  = 0x05, cmdWRSR  = 0x01 };

// Initialize chip select PC10
void csInit() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef Gpio;
    GPIO_StructInit(&Gpio);
    
    Gpio.GPIO_Pin = GPIO_Pin_10;
    Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    Gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &Gpio);
}

void eepromInit() {
    spiInit(EEPROM_SPI);
    csInit();
}

uint8_t eepromReadStatus() {
    uint8_t cmd[] = {cmdRDSR, 0xff};
    uint8_t res[2];
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, res, cmd, 2, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
    return res[1];
}

void eepromWriteStatus(uint8_t status) {
    uint8_t cmd[] = {cmdWRSR, status};

    while (WIP(eepromReadStatus()));

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, 0, cmd, 2, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
}

void eepromWriteEnable() {
    uint8_t cmd = cmdWREN;

    while (WIP(eepromReadStatus()));

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
}

void eepromWriteDisable() {
    uint8_t cmd = cmdWRDI;

    while (WIP(eepromReadStatus()));

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
}

int eepromWrite(uint8_t *buf, uint8_t cnt, uint16_t offset) {
    uint8_t cmd[] = {cmdWRITE, (uint8_t) ((offset & 0xff00) >> 8), (uint8_t) (offset & 0xff)};
    int bytes_write = 0;
    eepromWriteEnable();
    
    while (WIP(eepromReadStatus()));

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, 0, cmd, 3, EEPROM_SPEED);

    if (cnt >= PAGE_SIZE) {
        spiReadWrite(EEPROM_SPI, 0, buf, PAGE_SIZE, EEPROM_SPEED);
        bytes_write = PAGE_SIZE;
    }
    else {
        spiReadWrite(EEPROM_SPI, 0, buf, cnt, EEPROM_SPEED);
        bytes_write = cnt;
    }

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
    eepromWriteDisable();
    return bytes_write;
}

int eepromRead(uint8_t *buf, uint8_t cnt, uint16_t offset) {
    uint8_t cmd[] = {cmdREAD, (uint8_t) ((offset & 0xff00) >> 8), (uint8_t) (offset & 0xff)};
    int bytes_read;

    while (WIP(eepromReadStatus()));

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, 0, cmd, 3, EEPROM_SPEED);
    bytes_read = spiReadWrite(EEPROM_SPI, buf, 0, cnt, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
    return bytes_read;
}