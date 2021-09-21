TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS += -g -Iff9
ASFLAGS += -g
LDLIBS += -lm

# project files

vpath %.c ff9/src
vpath %.c ff9/src/option

# object files

OBJS  =  $(STARTUP) main.o ST7735.o bmp.o
OBJS += ff.o spidma.o uart.o xprintf.o
OBJS += stm32f10x_gpio.o stm32f10x_rcc.o stm32f10x_rtc.o stm32f10x_usart.o misc.o
OBJS += stm32f10x_spi.o stm32f10x_dma.o core_cm3.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common
