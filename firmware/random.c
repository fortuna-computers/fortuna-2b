#include "random.h"

#include <avr/eeprom.h>

#define EEPROM_SEED ((uint32_t *) 127)

static uint32_t seed;

void random_init(void)
{
    seed = eeprom_read_dword(EEPROM_SEED);
    eeprom_update_dword(EEPROM_SEED, seed + 1);
}

uint8_t random_value(void)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return (uint8_t) seed;
}
