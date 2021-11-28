#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "config.h"

#include "addr.h"
#include "post.h"
#include "random.h"
#include "ram.h"
#include "rtc.h"
#include "sdcard.h"
#include "spi.h"
#include "uart.h"
#include "z80.h"

int main(void)
{
    uart_init();
#if CLEAR_SCR_ON_INIT
    uart_clrscr();
#endif

    random_init();

    z80_init(Z80_SPEED_KHZ);

    addr_init();
    rtc_init();
    ram_init();
    spi_init();
    sdcard_init();

    /*
    RTC_DateTime dt = { 21, 11, 27, 17, 56, 0 };
    rtc_set_datetime(&dt);
    */

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
