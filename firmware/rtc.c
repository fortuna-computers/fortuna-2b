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

static uint8_t rtc_status(void)
{
#ifdef RTC_DEBUG
    uart_puthex(TW_STATUS);
#endif
    return TW_STATUS;
}

static uint8_t rtc_write(uint8_t value)
{
    TWDR = value;
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
#ifdef RTC_DEBUG
    uart_puthex_green(value);
#endif
    rtc_wait_until_ready();
    return rtc_status();
}

static uint8_t rtc_read(bool ack, uint8_t* data)
{
    TWCR = _BV(TWINT) | _BV(TWEN) | (ack ? _BV(TWEA) : 0);  // communicate
    rtc_wait_until_ready();
    *data = TWDR;
#ifdef RTC_DEBUG
    uart_puthex_red(*data);
#endif
    return rtc_status();
}

static uint8_t rtc_start(uint8_t rw)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);   // clear interrupt, set as master, enable I²C
    rtc_wait_until_ready();

    return rtc_write(DS1307 | rw);
}

static void rtc_stop(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

static uint8_t bcd(uint8_t hexvalue)
{
    return (hexvalue & 0xf) + ((hexvalue >> 4) * 10);
}

static uint8_t to_bcd(uint8_t decvalue)
{
    return ((decvalue / 10) << 4) | (decvalue % 10);
}

Response rtc_datetime(RTC_DateTime* dt)
{
    // send device ID, prepare for writing
    if (rtc_start(TW_WRITE) != TW_MT_SLA_ACK)
        return R_RTC_SLA_FAIL;
    
    // starting register
    if (rtc_write(0x0) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    
    // send device ID, prepare for reading
    if (rtc_start(TW_READ) != TW_MR_SLA_ACK)
        return R_RTC_SLA_FAIL;
        
    // read date
    uint8_t data[7];
    for (int i = 0; i < 6; ++i) {
        if (rtc_read(true, &data[i]) != TW_MR_DATA_ACK)
            return R_RTC_READ_FAIL;
    }

    if (rtc_read(false, &data[6]) != TW_MR_DATA_NACK)
        return R_RTC_READ_FAIL;

    // stop communcation
    rtc_stop();

    // translate
    dt->ss = bcd(data[0]);
    dt->nn = bcd(data[1]);
    dt->hh = bcd(data[2]);
    dt->dd = bcd(data[4]);
    dt->mm = bcd(data[5]);
    dt->yy = bcd(data[6]);

    return R_OK;
}

Response rtc_set_datetime(RTC_DateTime* dt)
{
    // send device ID, prepare for writing
    if (rtc_start(TW_WRITE) != TW_MT_SLA_ACK)
        return R_RTC_SLA_FAIL;
    
    // starting register
    if (rtc_write(0x0) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;

    // send date
    if (rtc_write(to_bcd(dt->ss)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    if (rtc_write(to_bcd(dt->nn)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    if (rtc_write(to_bcd(dt->hh)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    if (rtc_write(to_bcd(1)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    if (rtc_write(to_bcd(dt->dd)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    if (rtc_write(to_bcd(dt->mm)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    if (rtc_write(to_bcd(dt->yy)) != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;

    // stop communcation
    rtc_stop();
    
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
