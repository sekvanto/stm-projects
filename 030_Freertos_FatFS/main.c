#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "spidma_freertos.h"
#include "ff9/src/ff.h"
#include "ST7735.h"
#include "bmp.h"

void loop_through_images() {
    DIR Dir;
    FRESULT res;
    FILINFO fno;
    FATFS Fatfs;

    f_mount(0, &Fatfs); // Register volume work area
    // running in a loop
    while (1)
    {
        f_opendir(&Dir, "/");
        for (;;) {
            res = f_readdir(&Dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                continue;
            } else {
                bmp_display(fno.fname);
                Delay(250);
            }
        }
    }
    f_mount(0, 0);
}

void init_hardware();

int main(void)
{
    // set up interrupt priorities for FreeRTOS !!
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    // initialize hardware
    init_hardware();

    // Create tasks
    xTaskCreate(loop_through_images, "Display images", 4096,
                NULL, tskIDLE_PRIORITY + 1 , NULL);

    //xTaskCreate(drawRectangles, "Magenta rectangles", 256,
    //            (void *)&magenta, tskIDLE_PRIORITY + 1 , NULL);

    // Start scheduler
    vTaskStartScheduler();

    // Schedule never ends*/
}

void init_hardware()
{
    // Display = SPI1, sdCard = SPI2
    ST7735_init();
    spiInit(SPI2);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
