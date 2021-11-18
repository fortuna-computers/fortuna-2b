#ifndef RTC_H_
#define RTC_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct RTC_DateTime {
    uint8_t yy, mm, dd, hh, nn, ss;
} RTC_DateTime;

bool rtc_init();
bool rtc_datetime(RTC_DateTime* dt);

#endif
