/*************************************************** 
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card  
  ----> http://www.adafruit.com/products/358  
  as well as Adafruit raw 1.8" TFT display  
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams 
  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface (RST is optional) 
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  MIT license, all text above must be included in any redistribution
 ****************************************************/

/* 
 The code contained here is based upon the adafruit driver, but heavily modified
*/

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_tim.h>
#include <misc.h>
#include <stdint.h>
#include "spidma_freertos.h"
#include "ST7735.h"
#include "glcdfont.c"

#define LCD_PORT_BKL  GPIOA
#define LCD_PORT GPIOC
#define GPIO_PIN_BKL GPIO_Pin_1
#define GPIO_PIN_RST GPIO_Pin_1
#define GPIO_PIN_SCE GPIO_Pin_0
#define GPIO_PIN_DC GPIO_Pin_2

#define LCDSPEED SPI_FAST

#define LOW  0
#define HIGH 1

#define LCD_C LOW
#define LCD_D HIGH

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_MADCTL 0x36
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_COLMOD 0x3A

#define CHAR_WIDTH   5
#define CHAR_HEIGHT  7
#define CHAR_CONTAINER_WIDTH  6
#define CHAR_CONTAINER_HEIGHT 10

#define MAX_LINE_LEN (int) (ST7735_width / CHAR_CONTAINER_WIDTH)   // Max number of characters in line
#define MAX_LINE_NUM (int) (ST7735_height / CHAR_CONTAINER_HEIGHT) // Max number of lines

#define MADVAL(x) (((x) << 5) | 8)
static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

struct ST7735_cmdBuf {
  uint8_t command;   // ST7735 command byte
  uint8_t delay;     // ms delay after
  uint8_t len;       // length of parameter data
  uint8_t data[16];  // parameter data
};

static const struct ST7735_cmdBuf initializers[] = {
  // SWRESET Software reset 
  { 0x01, 150, 0, 0},
  // SLPOUT Leave sleep mode
  { 0x11,  150, 0, 0},
  // FRMCTR1, FRMCTR2 Frame Rate configuration -- Normal mode, idle
  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D) 
  { 0xB1, 0, 3, { 0x01, 0x2C, 0x2D }},
  { 0xB2, 0, 3, { 0x01, 0x2C, 0x2D }},
  // FRMCTR3 Frame Rate configureation -- partial mode
  { 0xB3, 0, 6, { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D }},
  // INVCTR Display inversion (no inversion)
  { 0xB4,  0, 1, { 0x07 }},
  // PWCTR1 Power control -4.6V, Auto mode
  { 0xC0,  0, 3, { 0xA2, 0x02, 0x84}},
  // PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
  { 0xC1,  0, 1, { 0xC5}},
  // PWCTR3 Power control, opamp current smal, boost frequency
  { 0xC2,  0, 2, { 0x0A, 0x00 }},
  // PWCTR4 Power control, BLK/2, opamp current small and medium low
  { 0xC3,  0, 2, { 0x8A, 0x2A}},
  // PWRCTR5, VMCTR1 Power control
  { 0xC4,  0, 2, { 0x8A, 0xEE}},
  { 0xC5,  0, 1, { 0x0E }},
  // INVOFF Don't invert display
  { 0x20,  0, 0, 0},
  // Memory access directions. row address/col address, bottom to top refesh (10.1.27)
  { ST7735_MADCTL,  0, 1, {MADVAL(MADCTLGRAPHICS)}},
  // Color mode 16 bit (10.1.30
  { ST7735_COLMOD,   0, 1, {0x05}},
  // Column address set 0..127 
  { ST7735_CASET,   0, 4, {0x00, 0x00, 0x00, 0x7F }},
  // Row address set 0..159
  { ST7735_RASET,   0, 4, {0x00, 0x00, 0x00, 0x9F }},
  // GMCTRP1 Gamma correction
  { 0xE0, 0, 16, {0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D,
			    0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 }},
  // GMCTRP2 Gamma Polarity corrction
  { 0xE1, 0, 16, {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
			    0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 }},
  // DISPON Display on
  { 0x29, 100, 0, 0},
  // NORON Normal on
  { 0x13,  10, 0, 0},
  // End
  { 0, 0, 0, 0}
};

void Delay(uint32_t nTime);

void selectCS(int select)
{
  if (select)
  {
    GPIO_ResetBits(LCD_PORT,GPIO_PIN_SCE);
  }
  else
  {
    GPIO_SetBits(LCD_PORT,GPIO_PIN_SCE);
  }
}

int abs(int x) {
  if (x < 0)
    return -x;
  return x;
}

static void LcdWrite(char dc, const char *data, int nbytes)
{
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_DC, dc);  // dc 1 = data, 0 = control
  spiReadWrite(SPILCD, 0, data, nbytes, LCDSPEED, selectCS);
}

static void LcdWrite16(char dc, const uint16_t *data, int cnt)
{
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_DC, dc);  // dc 1 = data, 0 = control
  spiReadWrite16(SPILCD, 0, data, cnt, LCDSPEED, selectCS);
}

static void ST7735_writeCmd(uint8_t c)
{
  LcdWrite(LCD_C, &c, 1);
}

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, 
			  uint16_t x1, uint16_t y1, uint8_t madctl)
{
  madctl = MADVAL(madctl);
  if (madctl != madctlcurrent){
      ST7735_writeCmd(ST7735_MADCTL);
      LcdWrite(LCD_D, &madctl, 1);
      madctlcurrent = madctl;
  }
  ST7735_writeCmd(ST7735_CASET);
  LcdWrite16(LCD_D, &x0, 1);
  LcdWrite16(LCD_D, &x1, 1);

  ST7735_writeCmd(ST7735_RASET);
  LcdWrite16(LCD_D, &y0, 1);
  LcdWrite16(LCD_D, &y1, 1);

  ST7735_writeCmd(ST7735_RAMWR);
}

void ST7735_pushColor(uint16_t *color, int cnt)
{
  LcdWrite16(LCD_D, color, cnt);
}

void ST7735_pushPixel(uint16_t x, uint16_t y, uint16_t *color) {
  ST7735_setAddrWindow(x, y, x - 1, y - 1, MADCTLGRAPHICS);
  ST7735_pushColor(color, 1);
}

void ST7735_fillScreen(uint16_t color)
{
  uint8_t x,y;
  ST7735_setAddrWindow(0, 0, ST7735_width -1, ST7735_height -1, MADCTLGRAPHICS);
  for (x=0; x < ST7735_width; x++) {
    for(y=0; y < ST7735_height; y++) {
      ST7735_pushColor(&color ,1);
    }
  }
}

// Initialize chip select (PC0), data control (PC2), reset (PC1)
void cs_dc_rstInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef Gpio;
  GPIO_StructInit(&Gpio);
    
  Gpio.GPIO_Pin = GPIO_PIN_SCE | GPIO_PIN_DC | GPIO_PIN_RST;
  Gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  Gpio.GPIO_Speed = GPIO_Speed_50MHz; //2
  GPIO_Init(LCD_PORT, &Gpio);
}

// Initialize timer for Delay() function
void tim_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // enable timer clock
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;

  	// set timer1 auto-reload value
	  // so the timer overflow will occur with a frequency of 24MHz/24000 = 1000 Hz (1 ms)
    
    TIM_TimeBaseStructure.TIM_Period = 24000;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Configure the TIM3 output trigger so that it occurs on update events
    //TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    // Configure and enable TIM3 interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    // No StructInit call in API
    NVIC_InitStructure.NVIC_IRQChannel = 29; // TIM3_IRQn for STM32F10X_MD_VL
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void ST7735_init() 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  const struct ST7735_cmdBuf *cmd;

  // set up pins and clocks

  spiInit(SPILCD);
  cs_dc_rstInit();
  tim_init();

  // set cs, reset low

  GPIO_WriteBit(LCD_PORT,GPIO_PIN_SCE, HIGH);
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_RST, HIGH);
  Delay(10);
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_RST, LOW);
  Delay(10);
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_RST, HIGH);
  Delay(10);

  // Send initialization commands to ST7735

  for (cmd = initializers; cmd->command; cmd++)
    {
      LcdWrite(LCD_C, &(cmd->command), 1);
      if (cmd->len)
        LcdWrite(LCD_D, cmd->data, cmd->len);
      if (cmd->delay)
        Delay(cmd->delay);
    }
}

void ST7735_outputCharacter(uint8_t c, uint16_t x, uint16_t y, uint16_t background, uint16_t foreground)
{
  ST7735_setAddrWindow(
    x,
    y,
    x + (CHAR_CONTAINER_WIDTH - 1),
    y + (CHAR_CONTAINER_HEIGHT - 1),
    MADCTLGRAPHICS
  );

  int i, j;
  for (i = 0; i < CHAR_CONTAINER_HEIGHT; i++) {
    for (j = 0; j < CHAR_CONTAINER_WIDTH; j++) {
      if (i >= CHAR_HEIGHT || j >= CHAR_WIDTH) { // Spacing between characters
        ST7735_pushColor(&background, 1);
        continue;
      }

      int pixel = ASCII[(c * CHAR_WIDTH) + j] & (1 << i);
      if (pixel)
        ST7735_pushColor(&foreground, 1);
      else
        ST7735_pushColor(&background, 1);
    }
  }
}

void ST7735_outputString(char *line, uint16_t background, uint16_t foreground)
{
  int x = 0, y = 0;
  int charsInLineWritten = 0;
  for (int i = 0; line[i] != '\0'; i++) {
    ST7735_outputCharacter(line[i], x, y, background, foreground);
    charsInLineWritten++;
    if (charsInLineWritten == MAX_LINE_LEN) {
      x = 0;
      y += CHAR_CONTAINER_HEIGHT;
      charsInLineWritten = 0;
      continue;
    }
    x += CHAR_CONTAINER_WIDTH;
  }
}

void drawLineLow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int D = (2 * dy) - dx;
  int y = y0;

  for (int x = x0; x <= x1; x++) {
    ST7735_pushPixel(x, y, &color);
    if (D > 0) {
      y = y + yi;
      D = D + (2 * (dy - dx));
    }
    else
      D = D + 2*dy;
  }
}

void drawLineHigh(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int D = (2 * dx) - dy;
  int x = x0;

  for (int y = y0; y <= y1; y++) {
    ST7735_pushPixel(x, y, &color);
    if (D > 0) {
      x = x + xi;
      D = D + (2 * (dx - dy));
    }
    else
      D = D + 2*dx;
  }
}

/* https://en.wikipedia.org/wiki/Bresenham's_line_algorithm */

void ST7735_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  //ST7735_setAddrWindow(x0, y0, x1 - 1, y1 - 1, MADCTLGRAPHICS);
  if (abs(y1 - y0) < abs(x1 - x0)) {
    if (x0 > x1)
      drawLineLow(x1, y1, x0, y0, color);
    else
      drawLineLow(x0, y0, x1, y1, color);
  } else {
    if (y0 > y1)
      drawLineHigh(x1, y1, x0, y0, color);
    else
      drawLineHigh(x0, y0, x1, y1, color);
  }
}

void ST7735_drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  ST7735_drawLine(x, y, x, y + height, color);
  ST7735_drawLine(x, y, x + width, y, color);
  ST7735_drawLine(x + width, y, x + width, y + height, color);
  ST7735_drawLine(x, y + height, x + width, y + height, color);
}

/* https://en.wikipedia.org/wiki/Midpoint_circle_algorithm */

void ST7735_drawCircle(uint16_t x_centre, uint16_t y_centre, uint16_t r, uint16_t color)
{
  int x = r, y = 0;
  ST7735_pushPixel(x + x_centre, y + y_centre, &color);
  if (r > 0) {
    ST7735_pushPixel(x + x_centre, -y + y_centre, &color);
    ST7735_pushPixel(y + x_centre, x + y_centre, &color);
    ST7735_pushPixel(-y + x_centre, x + y_centre, &color);
  }
  int P = 1 - r;
  while (x > y) {
    y++;
    if (P <= 0)
      P = P + 2*y + 1;
    else {
      x--;
      P = P + 2*y - 2*x + 1;
    }
    // All the perimeter points have already been printed
    if (x < y)
      break;
      
    ST7735_pushPixel(x + x_centre, y + y_centre, &color);
    ST7735_pushPixel(-x + x_centre, y + y_centre, &color);
    ST7735_pushPixel(x + x_centre, -y + y_centre, &color);
    ST7735_pushPixel(-x + x_centre, -y + y_centre, &color);

    if (x != y) {
      ST7735_pushPixel(y + x_centre, x + y_centre, &color);
      ST7735_pushPixel(-y + x_centre, x + y_centre, &color);
      ST7735_pushPixel(y + x_centre, -x + y_centre, &color);
      ST7735_pushPixel(-y + x_centre, -x + y_centre, &color);
    }
  }
}


// Timer code
static __IO uint32_t TimingDelay;

// Use outside of any tasks
void Delay(uint32_t nTime){
    // Enable Timer Interrupt , enable timer
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    TimingDelay = nTime;
    while(TimingDelay != 0);

    // Disable timer interrupt, disable timer
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
}

void TIM3_IRQHandler(void)
{
  if(TimingDelay != 0x00)
    TimingDelay --;
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}