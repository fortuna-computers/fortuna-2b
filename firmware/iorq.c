#include "iorq.h"

#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#include "ram.h"
#include "random.h"
#include "uart.h"
#include "z80.h"

static uint8_t post_byte;   // byte used for testing POST

#define get_WR()       (PIND & _BV(PD6))
#define get_RD()       (PIND & _BV(PD4))
#define set_WAITST()   PORTB |=  (1 << PB3)
#define clear_WAITST() PORTB &= ~(1 << PB3)

typedef enum IO_Dir { I_IN, I_OUT, I_NONE } IO_Dir;

void iorq_init(void)
{
    DDRB |= _BV(DDB3);
    clear_WAITST();

    post_byte = random_value();

    // enable interrupt for IORQ
    cli();
    GICR = (1 << INT1);     // enable interrupt on INT1 (IORQ)  
    MCUCR = (1 << ISC11);   //   on falling edge
    sei();
}

void iorq_release_wait(void)
{
    set_WAITST();
    clear_WAITST();
}

static IO_Dir iorq_current_dir(void)
{
    IO_Dir dir = I_NONE;
    if (get_RD() == 0)
        dir = I_IN;
    else if (get_WR() == 0)
        dir = I_OUT;
    return dir;
}

static void iorq_execute_in(uint8_t cmd)
{
    uart_puthex(cmd);
    /*
    DDRA = 0xff;
    PORTA = 0x43;
    */
}

static void iorq_execute_out(uint8_t cmd)
{
}

ISR(INT1_vect)   // interrupt: execute on IORQ
{
    IO_Dir dir = iorq_current_dir();

    z80_request_bus();  // this will also release the WAIT line

    uint8_t cmd = ram_get_byte(0xfdff);
    if (dir == I_IN) {
        iorq_execute_in(cmd);
    } else {
        iorq_execute_out(cmd);
    }

    z80_release_bus();
}
