#include "uart.h"

#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void uart_init()
{
    _delay_ms(100);
    
    // set speed
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

    // set config
    UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);   // Async-mode 
    UCSRB = (1<<RXEN) | (1<<TXEN);     // Enable Receiver and Transmitter

    _delay_ms(100);
}

void uart_putchar(char c)
{
    if (c == '\n')
        uart_putchar('\r');
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
}

int uart_getchar()
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

void uart_putbin(uint8_t value)
{
    for (int i = 7; i >= 0; --i)
        uart_putchar(((value >> i) & 1) ? '1' : '0');
    uart_putchar(' ');
}

void uart_clrscr()
{
    uart_print_P(PSTR("\033[2J\033[H"));
}
