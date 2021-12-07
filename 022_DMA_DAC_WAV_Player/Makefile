TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -g -Wno-multichar -Iff9
ASFLAGS = -g
LDLIBS += -lm

vpath %.c ff9/src
vpath %.c ff9/src/option

# object files

OBJS  =  $(STARTUP) main.o
OBJS +=  delay.o ff.o spi.o wav.o uart.o xprintf.o
OBJS +=  stm32f10x_gpio.o stm32f10x_rcc.o stm32f10x_tim.o stm32f10x_dac.o stm32f10x_usart.o 
OBJS +=  stm32f10x_dma.o stm32f10x_spi.o core_cm3.o misc.o player.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


