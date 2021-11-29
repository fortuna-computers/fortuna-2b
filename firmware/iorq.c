#include "iorq.h"

#include "buffer.h"
#include "ram.h"
#include "random.h"
#include "rtc.h"
#include "sdcard.h"
#include "uart.h"
#include "z80.h"

static uint8_t post_byte;   // byte used for testing POST
static uint32_t sdcard_wpage;

void iorq_init(void)
{
    post_byte = random_value();
}

uint8_t iorq_post_byte(void)
{
    return post_byte;
}

uint16_t iorq_output(uint8_t cmd)    // returns bytes affected
{
    switch (cmd) {
        case I_UART_RECV:
            buffer[0] = uart_last_pressed_key();
            return 1;
        case I_UART_SEND:
            ram_read_buffer(DATA_EXCHANGE_AREA, 1);
            uart_putchar((char) buffer[0]);
            return 0;
        
        case I_SDCARD_RAW_READ:
            ram_read_buffer(DATA_EXCHANGE_AREA, 4);
            sdcard_read_page(*(uint32_t *) &buffer[0]);
            return 512;
        case I_SDCARD_SET_WPAGE:
            ram_read_buffer(DATA_EXCHANGE_AREA, 4);
            sdcard_wpage = *(uint32_t *) &buffer[0];
            return 0;
        case I_SDCARD_RAW_WRITE:
            sdcard_write_page(sdcard_wpage);
            return 0;
        
        case I_SDCARD_RANDOM:
            *(uint32_t *) buffer = random_value();
            return 4;
        
        case I_SDCARD_MEMCPY:
            break;  // TODO
        
        case I_SDCARD_RTC_GET: {
            RTC_DateTime dt;
            rtc_datetime(&dt);
            buffer[0] = dt.yy;
            buffer[1] = dt.mm;
            buffer[2] = dt.dd;
            buffer[3] = dt.hh;
            buffer[4] = dt.nn;
            buffer[5] = dt.ss;
            return 6;
        }
        case I_SDCARD_RTC_SET: {
            RTC_DateTime dt = { buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5] };
            rtc_set_datetime(&dt);
            return 0;
        }
        
        case I_POST_TEST:
            buffer[0] = post_byte;
            return 1;
    }
    return 0;
}
