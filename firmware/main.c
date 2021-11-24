#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "config.h"

#include "addr.h"
#include "post.h"
#include "random.h"
#include "ram.h"
#include "rtc.h"
#include "spi.h"
#include "uart.h"
#include "z80.h"

int main(void)
{
    uart_init();
#if CLEAR_SCR_ON_INIT
    uart_clrscr();
#endif

    z80_init(Z80_SPEED_KHZ);
    random_init();
    addr_init();
    
    addr_set(0b11001100);
    addr_set(0b11001100);
    for (;;); 

    rtc_init();
    ram_init();
    spi_init();

#if POST_EXECUTE
    post_execute();
#endif
    
    // z80_powerup();
    
    for (;;);
}

// catch bad interrupts
ISR(BADISR_vect)
{
    puts_P(PSTR("!!!"));
    for(;;);
}
