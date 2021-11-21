#include "post.h"

#include <avr/pgmspace.h>

#include "ram.h"
#include "rtc.h"
#include "uart.h"

static void post_rtc(void)
{
    RTC_DateTime dt;
    try(rtc_datetime(&dt));
    uart_print_P(PSTR("RTC "));
    uart_putdec(dt.hh, 2);
    uart_putchar(':');
    uart_putdec(dt.nn, 2);
    uart_putchar(':');
    uart_putdec(dt.ss, 2);
    uart_putchar(' ');
    uart_putdec(dt.mm, 1);
    uart_putchar('/');
    uart_putdec(dt.dd, 1);
    uart_putchar('/');
    uart_putdec(dt.yy, 2);
    uart_putchar('\n');
}

static void post_ram(void)
{
}

void post_execute(void)
{
    post_rtc();
    post_ram();
}
