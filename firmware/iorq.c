#include "iorq.h"

#include "ram.h"
#include "random.h"
#include "uart.h"

static uint8_t post_byte;   // byte used for testing POST

void iorq_init(void)
{
    post_byte = random_value();
}

uint8_t iorq_input(uint8_t cmd)
{
    return 0xf0;
}

void iorq_output(uint8_t cmd, uint8_t data)
{
}