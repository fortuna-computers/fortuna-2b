/*
ds1307 lib 0x01

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.

References: parts of the code taken from https://github.com/adafruit/RTClib
*/


#ifndef RTC_H
#define RTC_H

//definitions
#define DS1307_ADDR (0x68<<1) //device address

//path to i2c fleury lib
#define DS1307_I2CFLEURYPATH "i2cmaster.h" //define the path to i2c fleury lib
#define DS1307_I2CINIT 1 //init i2c

//functions
void    rtc_init(void);
uint8_t rtc_setdate(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
void    rtc_getdate(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second);
void    rtc_print_datetime(void);

#endif

