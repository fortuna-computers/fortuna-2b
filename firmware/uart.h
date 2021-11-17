#ifndef UART_H_
#define UART_H_

#include <avr/pgmspace.h>

void uart_init();

void uart_putchar(char c);
int uart_getchar();

void uart_print_P(PGM_P string);
void uart_puthex(uint8_t value);
void uart_putbin(uint8_t value);
void uart_clrscr();

#endif
