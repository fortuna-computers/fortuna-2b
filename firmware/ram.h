#ifndef RAM_H_
#define RAM_H_

#include <stdint.h>

void ram_init(void);

void ram_write_buffer(uint16_t addr, uint16_t count);
void ram_read_buffer(uint16_t addr, uint16_t count);

uint8_t ram_get_byte(uint16_t addr);

void ram_dump(uint16_t addr, uint16_t until);

#endif
