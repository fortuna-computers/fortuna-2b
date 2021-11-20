/*
ds1307 lib 0x01

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "rtc.h"
#include "uart.h"

//path to i2c fleury lib
#include DS1307_I2CFLEURYPATH

/*
 * days per month
 */
const uint8_t rtc_daysinmonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

/*
 * initialize the accellerometer
 */
void rtc_init(void) {
	#if DS1307_I2CINIT == 1
	//init i2c
	i2c_init();
	_delay_us(10);
	#endif
}

/*
 * transform decimal value to bcd
 */
uint8_t rtc_dec2bcd(uint8_t val) {
	return val + 6 * (val / 10);
}

/*
 * transform bcd value to deciaml
 */
static uint8_t rtc_bcd2dec(uint8_t val) {
	return val - 6 * (val >> 4);
}

/*
 * get number of days since 2000/01/01 (valid for 2001..2099)
 */
static uint16_t rtc_date2days(uint8_t y, uint8_t m, uint8_t d) {
	uint16_t days = d;
	for (uint8_t i = 1; i < m; ++i)
		days += pgm_read_byte(rtc_daysinmonth + i - 1);
	if (m > 2 && y % 4 == 0)
		++days;
	return days + 365 * y + (y + 3) / 4 - 1;
}

/*
 * get day of a week
 */
uint8_t rtc_getdayofweek(uint8_t y, uint8_t m, uint8_t d) {
	uint16_t day = rtc_date2days(y, m, d);
	return (day + 6) % 7;
}

/*
 * set date
 */
uint8_t rtc_setdate(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	//sanitize data
	if (second > 59 ||
		minute > 59 ||
		hour > 23 ||
		day < 1 || day > 31 ||
		month < 1 || month > 12 ||
		year > 99)
		return 8;

	//sanitize day based on month
	if(day > pgm_read_byte(rtc_daysinmonth + month - 1))
		return 0;

	//get day of week
	uint8_t dayofweek = rtc_getdayofweek(year, month, day);

	//write date
	i2c_start_wait(DS1307_ADDR | I2C_WRITE);
	i2c_write(0x00);//stop oscillator
	i2c_write(rtc_dec2bcd(second));
	i2c_write(rtc_dec2bcd(minute));
	i2c_write(rtc_dec2bcd(hour));
	i2c_write(rtc_dec2bcd(dayofweek));
	i2c_write(rtc_dec2bcd(day));
	i2c_write(rtc_dec2bcd(month));
	i2c_write(rtc_dec2bcd(year));
	i2c_write(0x00); //start oscillator
	i2c_stop();

	return 1;
}

/*
 * get date
 */
void rtc_getdate(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second) {
	i2c_start_wait(DS1307_ADDR | I2C_WRITE);
	i2c_write(0x00);//stop oscillator
	i2c_stop();

	i2c_rep_start(DS1307_ADDR | I2C_READ);
	*second = rtc_bcd2dec(i2c_readAck() & 0x7F);
	*minute = rtc_bcd2dec(i2c_readAck());
	*hour = rtc_bcd2dec(i2c_readAck());
	i2c_readAck();
	*day = rtc_bcd2dec(i2c_readAck());
	*month = rtc_bcd2dec(i2c_readAck());
	*year = rtc_bcd2dec(i2c_readNak());
	i2c_stop();
}

void rtc_print_datetime(void)
{
    uint8_t year, month, day, hour, minute, second;
    rtc_getdate(&year, &month, &day, &hour, &minute, &second);

    uart_print_P(PSTR("RTC "));
    uart_putdec(hour, 2);
    uart_putchar(':');
    uart_putdec(minute, 2);
    uart_putchar(':');
    uart_putdec(second, 2);
    uart_putchar(' ');
    uart_putdec(month, 1);
    uart_putchar('/');
    uart_putdec(day, 1);
    uart_putchar('/');
    uart_putdec(year, 2);
    uart_putchar('\n');
}

