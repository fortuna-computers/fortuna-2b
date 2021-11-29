#include "iorq.h"

#include "buffer.h"
#include "random.h"

static uint8_t post_byte;   // byte used for testing POST

void iorq_init(void)
{
    post_byte = random_value();
}

uint8_t iorq_post_byte(void)
{
    return post_byte;
}

uint16_t iorq_output(uint8_t cmd)    // returns bytes affected
{
    switch (cmd) {
        case 0xff:
            buffer[0] = post_byte;
            return 1;
    }
    return 0;
}
