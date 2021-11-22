#include "response.h"

#include <avr/pgmspace.h>

#include "sdcard.h"
#include "uart.h"

Response try(Response response)
{
    Response r = try_continue(response);
    if (r != R_OK)
        for (;;);
    return R_OK;
}

Response try_continue(Response response)
{
    if (response != R_OK) {
        uart_print_P(PSTR("\033[0;31mERROR: "));
        switch (response) {
            case R_OK: break;
            case R_RTC_SLA_FAIL:   uart_print_P(PSTR("RTC start")); break;
            case R_RTC_READ_FAIL:  uart_print_P(PSTR("RTC read"));  break;
            case R_RTC_WRITE_FAIL: uart_print_P(PSTR("RTC write")); break;
            case R_SDCARD_FAIL:
                uart_print_P(PSTR("SDCARD failed "));
                uart_puthex(sdcard_last_stage());
                uart_puthex(sdcard_last_response());
                break;
        }
        uart_print_P(PSTR("\033[0m\n"));
    }
    return response;
}
