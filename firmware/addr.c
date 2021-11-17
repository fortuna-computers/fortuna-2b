#include "addr.h"

#include <avr/io.h>

// pins used
#define SER_H PC2
#define SER_L PC3
#define SRCLK PC4
#define OE    PC5

void addr_init()
{
    DDRC |= _BV(DDC2) | _BV(DDC3) | _BV(DDC4) | _BV(DDC5);
    PORTC |= _BV(OE);

    for (int i = 0; i < 9; ++i) {
        PORTC |= _BV(SRCLK);
        PORTC &= ~_BV(SRCLK);
    }
}

void addr_set(uint16_t addr)
{
    // disable 595
    PORTC |= _BV(OE);

    uint8_t portc = PORTC & 0b11100011;  // bytes to keep

    for (int i = 0; i < 8; ++i) {
    // for (int i = 7; i >= 0; --i) {

        // set SER_L
        if ((addr >> i) & 1)
            portc |= _BV(SER_L);
        else
            portc &= ~_BV(SER_L);

        // set SER_H
        if ((addr >> (i + 8)) & 1)
            portc |= _BV(SER_H);
        else
            portc &= ~_BV(SER_H);
        
        // 595 cycle
        PORTC = portc;
        PORTC |= _BV(SRCLK);
    }

    // extra CYCLE because RCLK and SRCLK are connected, so RCLK is one cycle behind
    PORTC &= ~_BV(SRCLK);
    PORTC |= _BV(SRCLK);

    // enable 595
    PORTC &= ~_BV(OE);
}

void addr_disable()
{
    PORTC |= _BV(OE);
}
