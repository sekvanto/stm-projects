#include <stdint.h>
#include "spidma_freertos.h"
#include "ST7735.h"
#include <misc.h>

// Rectangle function parameters
const uint16_t cyan[] = {CYAN, 39};
const uint16_t magenta[] = {MAGENTA, 73};

#define RECTANGLE_HEIGHT 10
#define RECTANGLE_WIDTH  25

void drawRectangles(void* arg) {
    uint16_t* color = (uint16_t *)arg;
    uint16_t* seed  = ((uint16_t *)arg) + 1;

    // Random coordinates
    uint32_t x = *seed;
    uint32_t y = x * 3 + 17;

    while (1)
    {
        x = (x * 7237) % (ST7735_width - RECTANGLE_WIDTH);
        y = (y * 8261) % (ST7735_height - RECTANGLE_HEIGHT);

        ST7735_drawRectangle(x, y, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, *color);

        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

void init_hardware();

int main()
{
    // set up interrupt priorities for FreeRTOS !!
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    ST7735_init();
    ST7735_fillScreen(BLACK);

    // initialize hardware
    init_hardware();

    xTaskCreate(drawRectangles, "Cyan rectangles", 256,
                (void *)&cyan, tskIDLE_PRIORITY + 1 , NULL);

    xTaskCreate(drawRectangles, "Magenta rectangles", 256,
                (void *)&magenta, tskIDLE_PRIORITY + 1 , NULL);

    // Start scheduler
    vTaskStartScheduler();

    // Schedule never ends*/
}

void init_hardware()
{}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif