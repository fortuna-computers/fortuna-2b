#ifndef IORQ_H_
#define IORQ_H_

#include <avr/io.h>

typedef enum IORQ_CMD {
    I_LAST_KEYPRESS     = 0x00,
    I_PRINT             = 0x01,
    
    I_SDCARD_RAW_READ   = 0x02,
    I_SDCARD_RAW_WRITE  = 0x03,
    
    I_RANDOM            = 0x04,
    
    I_MEMCPY            = 0x05,
    
    I_RTC_GET           = 0x06,
    I_RTC_SET           = 0x07,
    
    I_POST_TEST         = 0xff,
} IORQ_CMD;

void    iorq_init(void);
uint8_t iorq_post_byte(void);
void    iorq_output(uint8_t cmd);

#endif
