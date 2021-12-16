#ifndef UARTFC_H
#define UARTFC_H

#include <inttypes.h>

#define size_t unsigned long
#define ssize_t long

int uart_open (uint8_t uart, uint32_t baud, uint32_t flags);
int uart_close(uint8_t uart);

//ssize_t uart_write(uint8_t uart, const uint8_t *buf, size_t nbyte);
//ssize_t uart_read (uint8_t uart, uint8_t *buf, size_t nbyte);

int uart_putc(int c);
int uart_getc();

#endif