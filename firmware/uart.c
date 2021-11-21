#include "uart.h"

#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void uart_init(void)
{
    _delay_ms(100);
    
    // set speed
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

    // set config
    UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);   // Async-mode 
    UCSRB = (1<<RXEN) | (1<<TXEN);                  // Enable Receiver and Transmitter

    _delay_ms(100);
}

void uart_putchar(char c)
{
    if (c == '\n')
        uart_putchar('\r');
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
}

uint8_t uart_getchar(void)
{
    loop_until_bit_is_set(UCSRA, RXC);
    return UDR;
}

void uart_print_P(PGM_P string)
{
    char c;
    while ((c = pgm_read_byte(string++)))
        uart_putchar(c);
}

static void uart_puthex_nibble(uint8_t v)
{
    if (v <= 9)
        uart_putchar(v + '0');
    else
        uart_putchar(v + 'A' - 10);
}

void uart_puthex(uint8_t v)
{
    uart_puthex_nibble(v >> 4);
    uart_puthex_nibble(v & 0xf);
    uart_putchar(' ');
}

void uart_puthex16(uint16_t v)
{
    uart_puthex_nibble(v >> 12);
    uart_puthex_nibble((v >> 8) & 0xf);
    uart_puthex_nibble((v >> 4) & 0xf);
    uart_puthex_nibble(v & 0xf);
    uart_putchar(' ');
}

void uart_putbin(uint8_t value)
{
    for (int i = 7; i >= 0; --i)
        uart_putchar(((value >> i) & 1) ? '1' : '0');
    uart_putchar(' ');
}

void uart_putdec(uint8_t value, uint8_t digits)
{
    if (value >= 100 || digits > 2)
        uart_putchar((value / 100) + '0');
    if (value >= 10 || digits > 1)
        uart_putchar(((value / 10) % 10) + '0');
    uart_putchar((value % 10) + '0');
}

void uart_clrscr(void)
{
    uart_print_P(PSTR("\033[2J\033[H"));
}

static void uart_reset(void)
{
    uart_print_P(PSTR("\e[0m"));
}

void uart_puthex_red(uint8_t value)
{
    uart_print_P(PSTR("\e[0;31m"));
    uart_puthex(value);
    uart_reset();
}

void uart_puthex_green(uint8_t value)
{
    uart_print_P(PSTR("\e[0;32m"));
    uart_puthex(value);
    uart_reset();
}
