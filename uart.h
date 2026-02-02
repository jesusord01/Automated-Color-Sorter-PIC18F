#ifndef UART_H
#define	UART_H
/*Declaracion de funciones*/
#define _XTAL_FREQ 8000000
#include <stdint.h>
#include <xc.h>
void Serial_begin(uint16_t baud);
uint8_t Serial_available(void);
void Serial_write(char caracter);
char Serial_read(void);
void Serial_print(char *s);
void Serial_println(char *s);
#endif	/* UART_H */