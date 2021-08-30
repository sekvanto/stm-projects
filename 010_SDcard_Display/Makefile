TEMPLATEROOT = ..

# additional compilation flags

CFLAGS += -g -Iff9
ASFLAGS += -g
LDLIBS += -lm

# project files

vpath %.c ff9/src
vpath %.c ff9/src/option

# ccsbcs.o

OBJS= $(STARTUP) main.o
OBJS+= ff.o spi.o uart.o xprintf.o
OBJS+= stm32f10x_gpio.o stm32f10x_rcc.o stm32f10x_rtc.o stm32f10x_usart.o misc.o
OBJS+= stm32f10x_spi.o core_cm3.o

include $(TEMPLATEROOT)/Makefile.common
