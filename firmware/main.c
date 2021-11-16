#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "random.h"
#include "uart.h"

int main()
{
    uart_init();
    uart_clrscr();

    random_init();

    return 0;
}

/* catch bad interrupts */
ISR(BADISR_vect)
{
    puts_P(PSTR("!!!"));
    for(;;);
}

