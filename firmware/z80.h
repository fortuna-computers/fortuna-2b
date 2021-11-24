#ifndef Z80_H
#define Z80_H

#include <stdbool.h>
#include <stdint.h>

void z80_init(uint16_t speed_khz);

void z80_powerup(bool start_clock);
void z80_powerdown(void);

void z80_single_cycle(void);

#endif
