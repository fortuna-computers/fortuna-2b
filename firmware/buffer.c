#include "buffer.h"

#include <string.h>

volatile uint8_t buffer[512] = { 0 };

void buffer_clear(void)
{
    memset((void *) buffer, 0, 512);
}
