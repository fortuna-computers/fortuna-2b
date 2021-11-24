#include "addr.h"

#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"

#define set_SER_H()   PORTC |=  (1 << PC2)
#define clear_SER_H() PORTC &= ~(1 << PC2)
#define set_SER_L()   PORTC |=  (1 << PC3)
#define clear_SER_L() PORTC &= ~(1 << PC3)
#define set_SRCLK()   PORTC |=  (1 << PC4)
#define clear_SRCLK() PORTC &= ~(1 << PC4)
#define set_OE()      PORTC |=  (1 << PC5)
#define clear_OE()    PORTC &= ~(1 << PC5)
#define set_RCLK()    PORTD |=  (1 << PD7)
#define clear_RCLK()  PORTD &= ~(1 << PD7)

void addr_init(void)
{
    DDRC |= _BV(DDC2) | _BV(DDC3) | _BV(DDC4) | _BV(DDC5);
    DDRD |= _BV(DDD7);
    set_OE();

    clear_SER_L();
    clear_SER_H();
    for (int i = 0; i < 9; ++i) {   // clear 595
        clear_SRCLK();
        set_SRCLK();
    }
    clear_RCLK();
    set_RCLK();
}

void addr_set(uint16_t addr)
{
    // disable 595 output
    set_OE();

    clear_SER_L();
    clear_SER_H();
    for (int i = 0; i < 9; ++i) {   // clear 595
        clear_SRCLK();
        set_SRCLK();
    }
    clear_RCLK();
    set_RCLK();

    // additional clock cycle
    for (int i = 0; i < 8; ++i) {

        /*
        uart_puthex((addr >> i) & 1);
        uart_putchar('?');
        uart_wait_for_enter();
        */

        // set low ADDR
        if ((addr >> i) & 1)
            set_SER_L();
        else
            clear_SER_L();

        if ((addr >> (i + 8)) & 1)
            set_SER_H();
        else
            clear_SER_H();

        // cycle
        set_SRCLK();
        clear_SRCLK();
    }

    // enable 595 output
    clear_RCLK();
    set_RCLK();
    clear_OE();

    /*
    // disable 595
    PORTC |= _BV(OE);

    uint8_t portc = PORTC & 0b11100011;  // bytes to keep

    for (int i = 0; i < 8; ++i) {

        uart_puthex((addr >> i) & 1);

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
    */
}

void addr_disable(void)
{
    set_OE();
}
