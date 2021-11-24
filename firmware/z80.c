#include "z80.h"

#include <avr/io.h>
#include <util/delay.h>

#define set_BUSRQ()    PORTB |=  (1 << PB0)
#define clear_BUSRQ()  PORTB &= ~(1 << PB0)
#define set_RST()      PORTB |=  (1 << PB2)
#define clear_RST()    PORTB &= ~(1 << PB2)
#define set_WAITST()   PORTB |=  (1 << PB3)
#define clear_WAITST() PORTB &= ~(1 << PB3)
#define set_CLK()      PORTD |=  (1 << PD5)
#define clear_CLK()    PORTD &= ~(1 << PD5)
#define set_INT()      PORTC |=  (1 << PC7)
#define clear_INT()    PORTC &= ~(1 << PC7)
#define set_NMI()      PORTC |=  (1 << PC6)
#define clear_NMI()    PORTC &= ~(1 << PC6)

#define get_BUSACK()   (PINB & (1 << PINB1))
#define get_IORQ()     (PIND & (1 << PIND3))

volatile uint16_t z80_speed_khz;

void z80_init(uint16_t speed_khz)
{
    z80_speed_khz = speed_khz;
    
    // output: BUSRQ, RST, WAITST
    DDRB |= _BV(DDB0) | _BV(DDB2) | _BV(DDB3);
    
    // output: CLK
    DDRD |= _BV(DDD5);
    
    // output: INT, NMI
    DDRC |= _BV(DDC6) | _BV(DDC7);
    
    // input: BUSACK
    DDRB &= ~_BV(DDB1);
    
    // input: IORQ
    DDRD &= ~_BV(DDD3);
    
    // set pins to initial configuration
    clear_RST();
    set_BUSRQ();
    set_WAITST();
    set_INT();
    set_NMI();
}

void z80_powerdown(void)
{
    clear_RST();
}

void z80_powerup(bool start_clock)
{
    clear_RST();
    
    if (start_clock) {
        // clock output on OC1A (PD5)
        OCR1A = F_CPU / (2UL * (unsigned long) z80_speed_khz * 1000UL) - 1UL;  // calculate speed
        TCCR1A = (1 << COM1A0);                  // Toggle OC1A/OC1B on compare match
        TCCR1B = (1 << WGM12) | (1 << CS10);     // CTC mode, top OCR1A, no prescaling (clk/1)
        _delay_ms(10);
        
    } else {
        // run some cycles
        for (uint8_t i = 0; i < 50; ++i)
            z80_single_cycle();
    }
    
    set_BUSRQ();
    set_WAITST();
    set_INT();
    set_NMI();
    set_RST();
}

void z80_single_cycle(void)
{
    set_CLK();
    clear_CLK();
}