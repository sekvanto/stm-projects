#include <stdint.h>
#include "eeprom.h"

int main()
{
    eepromInit();

    uint8_t tbuff[20] = {'h', 'e', 'l', 'l', 'o',
                         't', 'h', 'e', 's', 'e',
                         'a', 'r', 'e', ' ', ' ',
                         't', 'e', 's', 't', 's'};
    uint8_t rbuff[20];

    int i;
    //eepromWrite(tbuff, 15, 0x01);
    //eepromWrite(tbuff + 15, 5, 0x16);
    eepromRead(rbuff, 32, 0x01);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
