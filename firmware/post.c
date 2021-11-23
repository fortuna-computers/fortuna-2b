#include "post.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "buffer.h"
#include "ram.h"
#include "random.h"
#include "rtc.h"
#include "sdcard.h"
#include "uart.h"
#include "z80.h"

static void print_ok(void) { uart_print_P(PSTR("OK\n")); }
_Noreturn static void print_error_and_halt(void) { uart_print_P(PSTR("ERROR\n")); for(;;); }

// region -> RTC

static void post_rtc(void)
{
    RTC_DateTime dt;
    Response r;
    for (uint8_t i = 0; i < 10; ++i) {
        r = rtc_datetime(&dt);
        if (r == R_OK)
            break;
    }
    try(r);

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

// endregion

// region -> RAM

static bool test_buffer(uint16_t addr)
{
    uint8_t initial_a = random_value(),
            initial_b = random_value();
    
    for (uint16_t i = 0; i < 256; ++i)
        buffer[i] = (initial_a + i) & 0xff;
    for (uint16_t i = 256; i < 512; ++i)
        buffer[i] = (initial_b + i) & 0xff;
    ram_write_buffer(addr, 512);
    
    buffer_clear();
    ram_read_buffer(addr, 512);
    
    int16_t err = -1;
    for (uint16_t i = 0; i < 256; ++i) {
        if (buffer[i] != ((initial_a + i) & 0xff)) {
            err = i;
            goto skip;
        }
    }
    for (uint16_t i = 256; i < 512; ++i) {
        if (buffer[i] != ((initial_b + i) & 0xff)) {
            goto skip;
        }
    }
    
skip:
    if (err != -1) {
        uart_putchar('*');
        uart_puthex16(err + addr);
        return false;
    } else {
        return true;
    }
}

static void post_ram(void)
{
    uart_print_P(PSTR("RAM "));
    if (test_buffer(0) && test_buffer(0x7400) && test_buffer(0x8000) && test_buffer(0xfa00)) {
        print_ok();
    } else {
        print_error_and_halt();
    }
}

// endregion

// region -> SDCARD

static void post_sdcard(void)
{
    uart_print_P(PSTR("SDCARD "));
    
    try(sdcard_initialize());
    try(sdcard_read_page(0));
    
    if (buffer[510] != 0x55 || buffer[511] != 0xaa) {
        uart_print_P(PSTR("NOT BOOTABLE"));
        for(;;);
    }
    
    print_ok();
}

// endregion

// region -> Z80

#include "post1.inc"

static void post_z80(void)
{
    uart_print_P(PSTR("CPU "));
    
    uint8_t expected_byte = random_value();
    
    // load executable code into RAM, and replace expected value
    memcpy_P((void*) buffer, post1_bin, sizeof post1_bin);
    buffer[1] = expected_byte;
    ram_write_buffer(0, sizeof post1_bin);
    
    // run code for a few milliseconds
    z80_powerup();
    _delay_ms(50);
    z80_powerdown();
    
    ram_dump(0, 0x20);
    
    // check if the memory position was set correctly
    ram_read_buffer(0x0, 0x20);
    if (buffer[0x1f] != expected_byte) {
        uart_puthex(buffer[0x1f]);
        print_error_and_halt();
    }
    
    print_ok();
}

// endregion

void post_execute(void)
{
    post_rtc();
    post_sdcard();
    post_ram();
    post_z80();
}
