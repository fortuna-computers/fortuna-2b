#include "response.h"

#include "uart.h"

Response try(Response response)
{
    if (response != R_OK) {
        uart_print_P(PSTR("ERROR "));
        uart_puthex(response);
        uart_putchar('\n');
        for (;;);
    }
    return response;
}
