#include "rtc.h"

#include <util/twi.h>

#include "config.h"
#include "response.h"

// region -> Low level stuff
// {{{

#define TW_READY  (TWCR & 0x80) // ready when TWINT returns to logic 1.

#define DS1307  0xD0 // I2C bus address of DS1307 RTC

#define SECONDS_REG 0x00
#define MINUTES_REG 0x01
#define HOURS_REG   0x02
#define DAYOFWK_REG 0x03
#define DAYS_REG    0x04
#define MONTHS_REG  0x05
#define YEARS_REG   0x06

static inline void rtc_wait_until_ready(void)
{
    while (!(TWCR & _BV(TWINT)));
}

static inline uint8_t rtc_status(void)
{
#if RTC_DEBUG
    uart_puthex(TW_STATUS);
#endif
    return TW_STATUS;
}

static uint8_t rtc_write(uint8_t value)
{
    TWDR = value;
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
#if RTC_DEBUG
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
#if RTC_DEBUG
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

#define RTC_START(rw, expected) { if (rtc_start(rw) != (expected)) return R_RTC_SLA_FAIL; }
#define RTC_WRITE(byte) { if (rtc_write(byte) != TW_MT_DATA_ACK) return R_RTC_WRITE_FAIL; }
#define RTC_READ(ack, ptr) { if (rtc_read(ack, ptr) != ((ack) ? TW_MR_DATA_ACK : TW_MR_DATA_NACK)) return R_RTC_READ_FAIL; }

// }}}
// endregion

void rtc_init(void)
{
    TWSR = 0;                             // clear status register
    TWBR = (F_CPU / I2C_SPEED_KHZ - 16) / 2;  // set I²C speed
}

Response rtc_datetime(RTC_DateTime* dt)
{
    uint8_t data[7];
    
    // choose initial register
    RTC_START(TW_WRITE, TW_MT_SLA_ACK)
    RTC_WRITE(0x0)
    
    // read date
    RTC_START(TW_READ, TW_MR_SLA_ACK)
    for (int i = 0; i < 6; ++i)
        RTC_READ(true, &data[i])
    RTC_READ(false, &data[6])

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
    // choose initial register
    RTC_START(TW_WRITE, TW_MT_SLA_ACK)
    RTC_WRITE(0x0)

    // send date
    RTC_WRITE(to_bcd(dt->ss))
    RTC_WRITE(to_bcd(dt->nn))
    RTC_WRITE(to_bcd(dt->hh))
    RTC_WRITE(1)
    RTC_WRITE(to_bcd(dt->dd))
    RTC_WRITE(to_bcd(dt->mm))
    RTC_WRITE(to_bcd(dt->yy))
    
    // stop communcation
    rtc_stop();
    
    return R_OK;
}