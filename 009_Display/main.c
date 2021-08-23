#include <stdint.h>
#include "spi.h"
#include "ST7735.h"

int colors [] = {
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    WHITE,
    0
};

void outputColors() {
    for (int *i = colors; *i != 0; i++) {
        ST7735_fillScreen(*i);
        Delay(500);
    }
}

void outputString() {
    ST7735_fillScreen(BLACK);
    ST7735_outputString("HELLO EVERYONE, I'm zip sekvanto", BLACK, YELLOW);
}

void drawFigures() {
    ST7735_fillScreen(BLACK);
    ST7735_drawLine(55, 55, 0, 0, YELLOW);
    ST7735_drawLine(10, 0, 10, 100, GREEN);
    ST7735_drawLine(20, 30, 20, 50, RED);
    ST7735_drawLine(80, 20, 120, 28, MAGENTA);

    ST7735_drawCircle(50, 60, 23, WHITE);
}

int main()
{
    ST7735_init();
    drawFigures();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif