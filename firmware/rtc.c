#include "rtc.h"

#include <util/twi.h>

#include "config.h"
#include "uart.h"
#include "response.h"

#define TW_READY  (TWCR & 0x80) // ready when TWINT returns to logic 1.

#define DS1307  0xD0 // I2C bus address of DS1307 RTC

#define SECONDS_REG 0x00
#define MINUTES_REG 0x01
#define HOURS_REG   0x02
#define DAYOFWK_REG 0x03
#define DAYS_REG    0x04
#define MONTHS_REG  0x05
#define YEARS_REG   0x06

void rtc_init(void)
{
    TWSR = 0;                         // clear status register
    TWBR = (F_CPU / F_SCL - 16) / 2;  // set I²C speed
}

static void rtc_wait_until_ready(void)
{
    while (!(TWCR & _BV(TWINT)));
}

static void rtc_start(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);   // clear interrupt, set as master, enable I²C
    rtc_wait_until_ready();
}

static void rtc_write(uint8_t value)
{
    TWDR = value;
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
#ifdef RTC_DEBUG
    uart_puthex_green(value);
#endif
    rtc_wait_until_ready();
}

static uint8_t rtc_read(void)
{
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
    rtc_wait_until_ready();
    uint8_t data = TWDR;
#ifdef RTC_DEBUG
    uart_puthex_red(data);
#endif
    return data;
}

static uint8_t rtc_status(void)
{
#ifdef RTC_DEBUG
    uart_puthex(TW_STATUS);
#endif
    return TW_STATUS;
}

static void rtc_stop(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

static Response rtc_read_reg(uint8_t reg, uint8_t* data)
{
    // send device ID, prepare for writing
    rtc_start();
    rtc_write(DS1307 | TW_WRITE);
    if (rtc_status() != TW_MT_SLA_ACK)
        return R_RTC_SLA_FAIL;
    
    // write device to be read from
    rtc_write(reg);
    if (rtc_status() != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    
    // send device ID, prepare for reading
    rtc_start();
    rtc_write(DS1307 | TW_READ);
    if (rtc_status() != TW_MR_SLA_ACK)
        return R_RTC_SLA_FAIL;
    
    // read register
    *data = rtc_read();
    if (rtc_status() != TW_MR_DATA_NACK)
        return R_RTC_READ_FAIL;

    // stop communcation
    rtc_stop();
    return R_OK;
}

static uint8_t bcd(uint8_t hexvalue)
{
    return (hexvalue & 0xf) + ((hexvalue >> 4) * 10);
}

Response rtc_datetime(RTC_DateTime* dt)
{
    /*
    try(rtc_read_reg(YEARS_REG,   &dt->yy));
    try(rtc_read_reg(MONTHS_REG,  &dt->mm));
    try(rtc_read_reg(DAYS_REG,    &dt->dd));
    try(rtc_read_reg(HOURS_REG,   &dt->hh));
    try(rtc_read_reg(MINUTES_REG, &dt->nn));
    */
    try(rtc_read_reg(SECONDS_REG, &dt->ss));

    // select between 12h and 24h
    if (dt->hh & 0x40)
        dt->hh &= 0x1f;
    else
        dt->hh &= 0x3f;

    dt->yy = bcd(dt->yy);
    dt->mm = bcd(dt->mm);
    dt->dd = bcd(dt->dd);
    dt->hh = bcd(dt->hh);
    dt->nn = bcd(dt->nn);
    dt->ss = bcd(dt->ss);

    return R_OK;
}

void rtc_print_datetime(void)
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

// http://w8bh.net/avr/AvrDS1307.pdf
