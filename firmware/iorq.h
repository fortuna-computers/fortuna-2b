#ifndef IORQ_H_
#define IORQ_H_

#include <avr/io.h>

typedef enum IORQ_CMD {
    I_UART_RECV         = 0x00,
    I_UART_SEND         = 0x01,
    
    I_SDCARD_RAW_READ   = 0x02,
    I_SDCARD_SET_WPAGE  = 0x03,
    I_SDCARD_RAW_WRITE  = 0x04,
    
    I_RANDOM            = 0x05,
    
    I_SDCARD_MEMCPY     = 0x06,
    
    I_SDCARD_RTC_GET    = 0x07,
    I_SDCARD_RTC_SET    = 0x08,
    
    I_POST_TEST         = 0xff,
} IORQ_CMD;

void     iorq_init(void);

uint8_t  iorq_post_byte(void);

uint16_t iorq_output(uint8_t cmd);

#endif
