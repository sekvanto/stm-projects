#ifndef ST7335_H
#define ST7335_H

#include <stdint.h>

/*  Colors are 565 RGB (5 bits Red, 6 bits green, 5 bits blue) */

/* Apparently my display has Red and Blue inverted (BGR), so i modified colors a bit */

#define BLACK           0x0000
#define BLUE            0xF800
#define GREEN           0x07E0
#define CYAN            0xFFE0
#define RED             0x001F
#define MAGENTA         0xF81F       
#define YELLOW          0x07FF  
#define WHITE           0xFFFF

#define SPILCD SPI2

#define CHAR_WIDTH   5
#define CHAR_HEIGHT  7
#define CHAR_CONTAINER_WIDTH  6
#define CHAR_CONTAINER_HEIGHT 10 // character container height

/* MADCTL [MY MX MV]
 *    MY  row address order   1 (bottom to top), 0 (top to bottom)
 *    MX  col address order   1 (right to left), 0 (left to right)
 *    MV  col/row exchange    1 (exchange),      0 normal
 */

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP      0x2

#define ST7735_width  128
#define ST7735_height 160

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, 
			  			  uint16_t x1, uint16_t y1, uint8_t madctl);
void ST7735_pushColor(uint16_t *color, int cnt);
void ST7735_fillScreen(uint16_t color);
void ST7735_init();

void ST7735_outputCharacter(uint8_t c, uint16_t x, uint16_t y, uint16_t background, uint16_t foreground);
void ST7735_outputString(char *line, uint16_t background, uint16_t foreground, int start_x, int start_y);

void ST7735_drawLine(uint16_t x0, uint16_t y0,
					 uint16_t x1, uint16_t y1, uint16_t color);
void ST7735_drawCircle(uint16_t x_centre, uint16_t y_centre,
					   uint16_t r, uint16_t color);

void Delay(uint32_t nTime);

#endif