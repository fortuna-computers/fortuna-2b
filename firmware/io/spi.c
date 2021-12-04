#include "spi.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "uart.h"

void spi_init(void)
{
    DDRB |= (1 << DDB5) | (1 << DDB7);   // MOSI, SCK: output
    DDRB &= ~(1 << DDB6);  // MISO: input
    
    // enable SPI, set as MASTER, clock to fosc/128 (TODO - can it go faster?)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

uint8_t spi_send(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    uint8_t r = SPDR;
#if SPI_DEBUG
    if (byte != 0xff) {
        uart_puthex_green(byte);
        if (r != 0xff)
            uart_puthex_red(r);
    }
#endif
    return r;
}

uint8_t spi_recv(void)
{
    uint8_t r = spi_send(0xff);
#if SPI_DEBUG
    if (r != 0xff)
        uart_puthex_red(r);
#endif
    return r;
}

uint8_t spi_recv_ignore_ff(void)
{
    uint8_t i = 0, r;
    while ((r = spi_recv()) == 0xff) {
        ++i;
        _delay_us(30);
        if (i > 8)
            break;  // timeout
    }
    return r;
}
