#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "config.h"

#include "addr.h"
#include "post.h"
#include "random.h"
#include "rtc.h"
#include "uart.h"

uint8_t buffer[512] = { 0 };

int main(void)
{
    uart_init();
#if CLEAR_SCR_ON_INIT
    uart_clrscr();
#endif

    random_init();
    addr_init();
    rtc_init();

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
