#include "iorq.h"

#include <string.h>

#include "buffer.h"
#include "ram.h"
#include "random.h"
#include "rtc.h"
#include "sdcard.h"
#include "uart.h"
#include "z80.h"

static uint8_t post_byte;   // byte used for testing POST

typedef struct __attribute__((packed)) IO_Registers {
    uint16_t I_ORIG;
    uint16_t I_DEST;
    uint32_t I_SD_BLOCK;
    uint16_t I_SZ;
} IO_Registers;

#define IO_REGISTERS_ADDR 0x100

void iorq_init(void)
{
    post_byte = random_value();
}

uint8_t iorq_post_byte(void)
{
    return post_byte;
}

void iorq_output(uint8_t cmd)    // returns bytes affected
{
    IO_Registers reg;
    
    ram_read_buffer(IO_REGISTERS_ADDR, sizeof(IO_Registers));
    memcpy(&reg, (const void *) buffer, sizeof(IO_Registers));
    
    switch (cmd) {
        case I_LAST_KEYPRESS:
            buffer[0] = uart_last_pressed_key();
            ram_write_buffer(reg.I_DEST, 1);
            break;
        case I_PRINT:
            ram_read_buffer(reg.I_ORIG, 1);
            uart_putchar((char) buffer[0]);
            break;
            
        case I_SDCARD_RAW_READ:
            sdcard_read_page(reg.I_SD_BLOCK);
            ram_write_buffer(reg.I_DEST, 512);
            break;
        case I_SDCARD_RAW_WRITE:
            ram_read_buffer(reg.I_ORIG, 512);
            sdcard_write_page(reg.I_SD_BLOCK);
            break;
            
        case I_RANDOM: {
            uint32_t r = random_value();
            memcpy((void *) buffer, &r, reg.I_SZ);
            break;
        }
    
        case I_MEMCPY:
            break;  // TODO
            
        case I_RTC_GET: {
            RTC_DateTime dt;
            rtc_datetime(&dt);
            memcpy((void *) buffer, &dt, sizeof(RTC_DateTime));
            ram_write_buffer(reg.I_DEST, sizeof(RTC_DateTime));
            break;
        }
        
        case I_RTC_SET: {
            RTC_DateTime dt;
            ram_read_buffer(reg.I_ORIG, sizeof(RTC_DateTime));
            memcpy(&dt, (const void *) buffer, sizeof(RTC_DateTime));
            rtc_set_datetime(&dt);
            break;
        }
        
        case I_POST_TEST:
            buffer[0] = post_byte;
            ram_write_buffer(reg.I_DEST, 1);
            break;
    }
}
