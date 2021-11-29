#include "ram.h"

#include <stdbool.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/delay.h>

#include "addr.h"
#include "buffer.h"
#include "config.h"
#include "uart.h"

#define set_MREQ()    PORTD |= (1 << PD2)
#define clear_MREQ()  PORTD &= ~(1 << PD2)
#define set_WR()      PORTD |= (1 << PD6)
#define clear_WR()    PORTD &= ~(1 << PD6)
#define set_RD()      PORTD |= (1 << PD4)
#define clear_RD()    PORTD &= ~(1 << PD4)
#define set_DATA(n)   PORTA = (n)
#define get_DATA()    PINA
#define WAIT()        { _NOP(); _NOP(); }

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
    DDRA = 0x0;                                 // data
    DDRD &= ~(_BV(PD2) | _BV(PD4) | _BV(PD6));  // MREQ, RD, WR
    
    set_MREQ();   // all control pins high, data = 0x0
    set_WR();
    set_RD();
    set_DATA(0x0);
}

void ram_init(void)
{
    ram_bus_release();
    ram_read_buffer(0, 16);
    ram_write_buffer(0, 16);
}

void ram_write_buffer(uint16_t addr, uint16_t count)
{
    ram_bus_takeover(true);

#if RAM_DEBUG
    uart_putchar('\n');
    uart_puthex16(addr);
    uart_putchar('\n');
#endif
    
    for (uint16_t i = 0; i < count; ++i) {
#if RAM_DEBUG
        uart_puthex_green(buffer[i]);
#endif
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
    
#if RAM_DEBUG
    uart_putchar('\n');
    uart_puthex16(addr);
    uart_putchar('\n');
#endif
    
    for (uint16_t i = 0; i < count; ++i) {
        addr_set(addr + i);
        clear_MREQ();
        clear_RD();
        WAIT();
        buffer[i] = get_DATA();
#if RAM_DEBUG
        uart_puthex_red(buffer[i]);
#endif
        set_MREQ();
        set_RD();
        WAIT();
    }
    
    ram_bus_release();
}

void ram_dump(uint16_t addr, uint16_t until)
{
    ram_read_buffer(addr, until);
    
    for (uint16_t i = 0; i < until; ++i)
        uart_puthex(buffer[i]);
    uart_putchar('\n');
}
