#include "ram.h"

#include <stdbool.h>

#include <avr/io.h>

#include "addr.h"

extern volatile uint8_t* buffer;

#define set_MREQ()    PORTD |= (1 << PD2)
#define clear_MREQ()  PORTD &= ~(1 << PD2)
#define set_WR()      PORTD |= (1 << PD6)
#define clear_WR()    PORTD &= ~(1 << PD6)
#define set_RD()      PORTD |= (1 << PD4)
#define clear_RD()    PORTD &= ~(1 << PD4)
#define set_DATA(n)   PORTA = (n)
#define get_DATA()    PINA
#define WAIT()        _NOP()

static void ram_bus_takeover(bool for_writing)
{
}

static void ram_bus_release()
{
    addr_disable();
    DDRA =  0x0;                                // data
    DDRD &= ~(_BV(PD2) | _BV(PD4) | _BV(PD6));  // MREQ, RD, WR
    
    set_MREQ();   // all control pins high, data = 0x0
    set_WR();
    set_RD();
    set_DATA(0x0);
}

void ram_init(void)
{
    ram_bus_release();
}