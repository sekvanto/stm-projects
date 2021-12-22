TEMPLATEROOT = ..

# additional compilation flags

CFLAGS += -O0 -g -Iff9
ASFLAGS += -g
LDLIBS += -lm

FreeRTOS = /home/nav/workspace/src/FreeRTOSv202012.00/FreeRTOS/Source
CFLAGS  += -I$(FreeRTOS)/include -DGCC_ARMCM3 -I$(FreeRTOS) -I$(FreeRTOS)/portable/GCC/ARM_CM3

vpath %.c $(FreeRTOS)/
vpath %.c $(FreeRTOS)/portable/MemMang
vpath %.c $(FreeRTOS)/portable/GCC/ARM_CM3

# project files

vpath %.c ff9/src
vpath %.c ff9/src/option

# ccsbcs.o

OBJS= $(STARTUP) main.o
OBJS+= ff.o spidma_freertos.o ST7735.o bmp.o
OBJS+= tasks.o queue.o list.o timers.o heap_1.o port.o
OBJS+= stm32f10x_gpio.o stm32f10x_rcc.o stm32f10x_rtc.o stm32f10x_tim.o misc.o
OBJS+= stm32f10x_spi.o stm32f10x_dma.o core_cm3.o

include $(TEMPLATEROOT)/Makefile.common
