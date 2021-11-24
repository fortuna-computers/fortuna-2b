#ifndef UART_H_
#define UART_H_

#include <avr/pgmspace.h>
#include "response.h"

void    uart_init(void);

void    uart_putchar(char c);
uint8_t uart_getchar(void);

void    uart_print_P(PGM_P string);
void    uart_puthex(uint8_t value);
void    uart_puthex16(uint16_t value);
void    uart_putbin(uint8_t value);
void    uart_putdec(uint8_t value, uint8_t digits);
void    uart_clrscr(void);

void    uart_puthex_red(uint8_t value);
void    uart_puthex_green(uint8_t value);

void    uart_wait_for_enter(void);

#endif
