#ifndef SDCARD_H_
#define SDCARD_H_

#include <stdbool.h>
#include <stdint.h>

#include "response.h"

typedef enum {
    SD_RESET    = 0x1, SD_GO_IDLE = 0x2, SD_IF_COND = 0x3, SD_INIT = 0x4,
    SD_GET_OCR  = 0x10,
    SD_READ_OK  = 0x20, SD_READ_REJECTED = 0x21, SD_READ_TIMEOUT = 0x22, SD_READ_CRC_FAILED = 0x23,
    SD_WRITE_OK = 0x30, SD_WRITE_REJECTED = 0x31, SD_WRITE_TIMEOUT = 0x32, SD_WRITE_DATA_REJECTED = 0x33, SD_WRITE_DATA_TIMEOUT = 0x34,
    SD_NOT_INITIALIZED = 0xff,
} SDCardStage;

void     sdcard_init(void);
Response sdcard_initialize(void);
Response sdcard_read_page(uint32_t block);
Response sdcard_write_page(uint32_t block);

SDCardStage sdcard_last_stage(void);
uint8_t     sdcard_last_response(void);

#endif
