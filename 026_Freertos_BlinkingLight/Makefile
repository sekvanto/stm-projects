TEMPLATEROOT = ..

FreeRTOS = /home/nav/workspace/src/FreeRTOSv202012.00/FreeRTOS/Source
CFLAGS  += -I$(FreeRTOS)/include -DGCC_ARMCM3 -I$(FreeRTOS) -I$(FreeRTOS)/portable/GCC/ARM_CM3

vpath %.c $(FreeRTOS)/
vpath %.c $(FreeRTOS)/portable/MemMang
vpath %.c $(FreeRTOS)/portable/GCC/ARM_CM3

# object files

OBJS  =  $(STARTUP) main.o
OBJS +=  tasks.o queue.o list.o timers.o heap_1.o port.o
OBJS +=  stm32f10x_gpio.o stm32f10x_rcc.o misc.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common
