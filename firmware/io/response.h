#ifndef RESPONSE_H_
#define RESPONSE_H_

typedef enum Response {
    R_OK                = 0x00,
    R_RTC_SLA_FAIL      = 0x10,
    R_RTC_WRITE_FAIL    = 0x11,
    R_RTC_READ_FAIL     = 0x12,
    R_SDCARD_FAIL       = 0x13,
} Response;

Response try(Response response);
Response try_continue(Response response);

#endif
