#include "rtc.h"

#include <util/twi.h>

#define F_SCL 100000L // I2C clock speed 100 kHz

#define TW_READY  (TWCR & 0x80) // ready when TWINT returns to logic 1.

#define DS1307  0xD0 // I2C bus address of DS1307 RTC
#define TW_NACK 0x84 // read data with NACK (last byte)
#define TW_SEND 0x84 // send data (TWINT,TWEN)
#define TW_STOP 0x94 // send stop condition (TWINT,TWSTO,TWEN)

#define SECONDS_REG 0x00
#define MINUTES_REG 0x01
#define HOURS_REG   0x02
#define DAYOFWK_REG 0x03
#define DAYS_REG    0x04
#define MONTHS_REG  0x05
#define YEARS_REG   0x06

#define READ 1

static uint8_t i2c_start()
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while (!TW_READY);
    return TW_STATUS == 0x08;
}

static uint8_t i2c_send_addr(uint8_t addr)
{
    TWDR = addr;
    TWCR = TW_SEND;
    while (!TW_READY);
    return TW_STATUS == 0x18;
}

static uint8_t i2c_write(uint8_t data)
{
    TWCR = data;
    TWCR = TW_SEND;
    while (!TW_READY);
    return TW_STATUS == 0x28;
}

static uint8_t i2c_read_nack()
{
    TWCR = TW_NACK;
    while (!TW_READY);
    return TWDR;
}

static void i2c_stop()
{
    TWCR = TW_STOP;
}

static void i2c_write_reg(uint8_t device_reg, uint8_t data)
{
    i2c_start();
    i2c_send_addr(DS1307);
    i2c_write(device_reg);
    i2c_write(data);
    i2c_stop();
}

static uint8_t i2c_read_reg(uint8_t device_reg)
{
    uint8_t data = 0;
    i2c_start();
    i2c_send_addr(DS1307);
    i2c_write(device_reg);
    i2c_start();
    i2c_send_addr(DS1307 + READ);
    data = i2c_read_nack();
    i2c_stop();
    return data;
}

bool rtc_init()
{
    TWSR = 0;
    TWBR = (F_CPU / F_SCL - 16) / 2;
    if (i2c_start() != 1)
        return false;

    return true;
}

bool rtc_datetime(RTC_DateTime* dt)
{
    dt->yy = i2c_read_reg(DS1307, YEARS_REG);
    dt->mm = i2c_read_reg(DS1307, MONTHS_REG);
    dt->dd = i2c_read_reg(DS1307, DAYS_REG);
    dt->hh = i2c_read_reg(DS1307, HOURS_REG);
    dt->nn = i2c_read_reg(DS1307, MINUTES_REG);
    dt->ss = i2c_read_reg(DS1307, SECONDS_REG);

    // select between 12h and 24h
    if (dt->hh & 0x40)
        dt->hh &= 0x1f;
    else
        dt->hh &= 0x3f;

    return true;  // TODO - check errors
}

// http://w8bh.net/avr/AvrDS1307.pdf
