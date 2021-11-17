#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "addr.h"
#include "random.h"
#include "uart.h"

int main()
{
    uart_init();
    uart_clrscr();

    random_init();
    addr_init();

    addr_set(0xff00);
    addr_set(0b1010001100000000);

    for (;;);

    return 0;
}

/* catch bad interrupts */
ISR(BADISR_vect)
{
    puts_P(PSTR("!!!"));
    for(;;);
}

