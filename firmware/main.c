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

int main(void)
{
    uart_init();
#if CLEAR_SCR_ON_INIT
    uart_clrscr();
#endif

    random_init();
    addr_init();
    rtc_init();
    ram_init();
    spi_init();
    
    for (uint8_t i = 0; i < 200; ++i)
        uart_puthex(random_value());
    uart_putchar('\n');

#if POST_EXECUTE
    post_execute();
#endif
    
    for (;;);
}

// catch bad interrupts
ISR(BADISR_vect)
{
    puts_P(PSTR("!!!"));
    for(;;);
}
