#include "random.h"

#include <stdlib.h>

#include <avr/eeprom.h>

#define EEPROM_BYTE ((uint8_t *) 511)

static uint8_t seed;

void random_init(void)
{
    uint8_t r = eeprom_read_byte(EEPROM_BYTE);
    seed = r;
    eeprom_update_byte(EEPROM_BYTE, r + 1);
}

uint8_t random_value(void)
{
    seed = (uint8_t) (((uint32_t) seed) * 89 + 207);
    return seed;
}
