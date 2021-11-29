#include "iorq.h"

#include "ram.h"
#include "random.h"
#include "uart.h"

static uint8_t post_byte;   // byte used for testing POST

void iorq_init(void)
{
    post_byte = random_value();
}