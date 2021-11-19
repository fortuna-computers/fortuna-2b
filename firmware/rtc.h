#ifndef RTC_H_
#define RTC_H_

#include <stdbool.h>
#include <stdint.h>

#include "response.h"

typedef struct RTC_DateTime {
    uint8_t yy, mm, dd, hh, nn, ss;
} RTC_DateTime;

Response rtc_init(void);
Response rtc_datetime(RTC_DateTime* dt);
void     rtc_print_datetime(void);

#endif
