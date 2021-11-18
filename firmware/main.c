#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "addr.h"
#include "random.h"
#include "rtc.h"
#include "uart.h"

int main()
{
    uart_init();
    // uart_clrscr();

    random_init();
    addr_init();
    if (!rtc_init())
        uart_print_P(PSTR("RTC FAIL\n"));

    uart_print_P(PSTR("Hello world!\n"));

    for (;;);

    return 0;
}

/* catch bad interrupts */
ISR(BADISR_vect)
{
    puts_P(PSTR("!!!"));
    for(;;);
}

