#include "ram.h"

#include <stdbool.h>

#include <avr/cpufunc.h>
#include <avr/io.h>

#include "addr.h"
#include "buffer.h"

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
    // control port
    DDRD |= _BV(PD2) | _BV(PD4) | _BV(PD6);
    
    // data port
    if (for_writing) {
        DDRA = 0xff;
    } else {
        DDRA = 0x0;
        set_DATA(0x0);
    }
}

static void ram_bus_release(void)
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

void ram_write_buffer(uint16_t addr, uint16_t count)
{
    ram_bus_takeover(true);
    
    for (uint16_t i = 0; i < count; ++i) {
        set_DATA(buffer[i]);
        addr_set(addr + i);
        clear_MREQ();
        clear_WR();
        WAIT();
        set_WR();
        set_MREQ();
        WAIT();
    }
    
    ram_bus_release();
}

void ram_read_buffer(uint16_t addr, uint16_t count)
{
    ram_bus_takeover(false);
    
    for (uint16_t i = 0; i < count; ++i) {
        addr_set(addr + i);
        clear_MREQ();
        clear_RD();
        WAIT();
        buffer[i] = get_DATA();
        set_MREQ();
        set_RD();
        WAIT();
    }
    
    ram_bus_release();
}
