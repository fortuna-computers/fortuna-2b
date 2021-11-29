#ifndef Z80_H
#define Z80_H

#include <stdint.h>

#define DATA_EXCHANGE_AREA 0xfe00

void z80_init(uint16_t speed_khz);

void z80_powerup(void);
void z80_powerdown(void);

void z80_cycle(void);
void z80_request_bus(void);

#endif
