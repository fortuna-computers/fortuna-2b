#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "addr.h"
#include "random.h"
#include "rtc.h"
#include "uart.h"

int main(void)
{
    uart_init();
    // uart_clrscr();

    random_init();
    addr_init();

    rtc_init();
    /*
    RTC_DateTime dt = { 21, 11, 20, 19, 47, 0 };
    rtc_set_datetime(&dt);
    */
    rtc_print_datetime();

    for (;;);

    return 0;
}

/* catch bad interrupts */
ISR(BADISR_vect)
{
    puts_P(PSTR("!!!"));
    for(;;);
}
