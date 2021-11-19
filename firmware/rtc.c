#include "rtc.h"

#include <util/twi.h>

#include "uart.h"

#define F_SCL 100000L // I2C clock speed 100 kHz

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

static void rtc_wait_until_ready()
{
    while (!(TWCR & _BV(TWINT)));
}

static uint8_t rtc_read_reg(uint8_t reg)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);   // clear interrupt, set as master, enable I²C
    rtc_wait_until_ready();
    
    // send device ID, prepare for writing
    TWDR = DS1307 | TW_WRITE;
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
    rtc_wait_until_ready();
    if (TW_STATUS != TW_MT_SLA_ACK)
        return R_RTC_SLA_FAIL;
    
    // write device to be read from
    TWDR = reg;
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
    if (TW_STATUS != TW_MT_DATA_ACK)
        return R_RTC_WRITE_FAIL;
    
    // send device ID, prepare for reading
    TWDR = DS1307 | TW_READ;
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
    rtc_wait_until_ready();
    if (TW_STATUS != TW_MR_SLA_ACK)
        return R_RTC_SLA_FAIL;
    
    // read register
    TWCR = _BV(TWINT) | _BV(TWEN);  // communicate
    rtc_wait_until_ready();
    uint8_t data = TWDR;
    if (TW_STATUS != TW_ST_DATA_ACK)
        return R_RTC_READ_FAIL;
    
    // stop communcation
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    return R_OK;
}


#if 0

#define TW_NACK 0x84 // read data with NACK (last byte)
#define TW_SEND 0x84 // send data (TWINT,TWEN)
#define TW_STOP 0x94 // send stop condition (TWINT,TWSTO,TWEN)

// #define RTC_DEBUG 1

static Response i2c_start(uint8_t addr)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while (!TW_READY);
    TWDR = addr;
#ifdef RTC_DEBUG
    uart_puthex_green(addr);
#endif
    TWCR = TW_SEND;
    while (!TW_READY);
#ifdef RTC_DEBUG
    uart_puthex_red(TW_STATUS);
#endif
    return (TW_STATUS == 0x18) ? R_OK : R_RTC_SLA_FAIL;
}

static Response i2c_write(uint8_t data)
{
    TWDR = data;
#ifdef RTC_DEBUG
    uart_puthex_green(data);
#endif
    TWCR = TW_SEND;
    while (!TW_READY);
#ifdef RTC_DEBUG
    uart_puthex_red(TW_STATUS);
#endif
    return (TW_STATUS == 0x28) ? R_OK : R_RTC_WRITE_FAIL;
}

static Response i2c_read_nack(uint8_t* data)
{
    TWCR = TW_NACK;
    while (!TW_READY);
#ifdef RTC_DEBUG
    uart_puthex_red(TW_STATUS);
#endif
    *data = TWDR;
    return (TW_STATUS == TW_MR_DATA_ACK) ? R_OK : R_RTC_READ_FAIL;
}

static void i2c_stop(void)
{
    TWCR = TW_STOP;
}

static void i2c_write_reg(uint8_t device_reg, uint8_t data)
{
    try(i2c_start(DS1307 | TW_WRITE));
    try(i2c_write(device_reg));
    try(i2c_write(data));
    i2c_stop();
}

static uint8_t i2c_read_reg(uint8_t device_reg)
{
    uint8_t data = 0;
    try(i2c_start(DS1307 | TW_WRITE));
    try(i2c_write(device_reg));
    try(i2c_start(DS1307 | TW_READ));
    try(i2c_read_nack(&data));
    i2c_stop();
    return data;
}

Response rtc_init(void)
{
    TWSR = 0;
    TWBR = (F_CPU / F_SCL - 16) / 2;
    return R_OK;
}
#endif

Response rtc_datetime(RTC_DateTime* dt)
{
    dt->yy = rtc_read_reg(YEARS_REG);
    /*
    dt->mm = i2c_read_reg(MONTHS_REG);
    dt->dd = i2c_read_reg(DAYS_REG);
    dt->hh = i2c_read_reg(HOURS_REG);
    dt->nn = i2c_read_reg(MINUTES_REG);
    dt->ss = i2c_read_reg(SECONDS_REG);

    // select between 12h and 24h
    if (dt->hh & 0x40)
        dt->hh &= 0x1f;
    else
        dt->hh &= 0x3f;
    */

    return R_OK;  // TODO - check errors
}

void rtc_print_datetime(void)
{
    RTC_DateTime dt;
    try(rtc_datetime(&dt));
    uart_print_P(PSTR("RTC "));
    uart_putdec(dt.hh);
    uart_putchar(':');
    uart_putdec(dt.nn);
    uart_putchar(':');
    uart_putdec(dt.ss);
    uart_putchar(' ');
    uart_putdec(dt.mm);
    uart_putchar('/');
    uart_putdec(dt.dd);
    uart_putchar('/');
    uart_putdec(dt.yy);
    uart_putchar('\n');
}

// http://w8bh.net/avr/AvrDS1307.pdf
