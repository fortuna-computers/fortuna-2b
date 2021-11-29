#include "z80.h"

#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "iorq.h"
#include "uart.h"

#define set_BUSRQ()    PORTB |=  (1 << PB0)
#define clear_BUSRQ()  PORTB &= ~(1 << PB0)
#define set_RST()      PORTB |=  (1 << PB2)
#define clear_RST()    PORTB &= ~(1 << PB2)
#define set_CLK()      PORTD |=  (1 << PD5)
#define clear_CLK()    PORTD &= ~(1 << PD5)
#define set_INT()      PORTC |=  (1 << PC7)
#define clear_INT()    PORTC &= ~(1 << PC7)
#define set_NMI()      PORTC |=  (1 << PC6)
#define clear_NMI()    PORTC &= ~(1 << PC6)
#define set_WAITST()   PORTB |=  (1 << PB3)
#define clear_WAITST() PORTB &= ~(1 << PB3)

#define get_BUSACK()   (PINB & (1 << PINB1))
#define get_IORQ()     (PIND & (1 << PIND3))
#define get_WR()       (PIND & _BV(PD6))
#define get_RD()       (PIND & _BV(PD4))


static volatile uint16_t z80_speed_khz = 0;

static void z80_stop_clock(void)
{
    TCCR1A = 0;
}

static void z80_start_clock(void)
{
    // clock output on OC1A (PD5)
    OCR1A = F_CPU / (2UL * (unsigned long) z80_speed_khz * 1000UL) - 1UL;  // calculate speed
    TCCR1A = (1 << COM1A0);                  // Toggle OC1A/OC1B on compare match
    TCCR1B = (1 << WGM12) | (1 << CS10);     // CTC mode, top OCR1A, no prescaling (clk/1)
}

void z80_init(uint16_t speed_khz)
{
    z80_speed_khz = speed_khz;

    // reset CPU
    DDRB |= _BV(DDB2) | _BV(DDB3);  // RST, WAITST
    DDRD |= _BV(DDD5);  // CLK
    clear_RST();
    for (int i = 0; i < 50; ++i)
        z80_cycle();

    // output: BUSRQ
    DDRB |= _BV(DDB0);
    
    // output: INT, NMI
    DDRC |= _BV(DDC6) | _BV(DDC7);
    
    // input: BUSACK
    DDRB &= ~_BV(DDB1);
    
    // input: IORQ
    DDRD &= ~_BV(DDD3);
    
    // set pins to initial configuration
    set_BUSRQ();
    set_INT();
    set_NMI();
    set_WAITST();
    
    // enable interrupt for IORQ
    cli();
    GICR = (1 << INT1);     // enable interrupt on INT1 (IORQ)
    MCUCR = (1 << ISC11);   //   on falling edge
    sei();
    
    z80_start_clock();
}

void z80_powerdown(void)
{
    clear_RST();
}

void z80_powerup(void)
{
    set_BUSRQ();
    set_INT();
    set_NMI();
    set_RST();
}

void z80_cycle(void)
{
    set_CLK();
    clear_CLK();
}

static void z80_iorq(void)
{
    bool rd = get_RD() == 0;
    bool wr = get_WR() == 0;
    
    if (rd)
        uart_putchar('R');
    if (wr)
        uart_putchar('W');
    
    for(;;);
}

ISR(INT1_vect)   // interrupt: execute on IORQ
{
    z80_iorq();
}
