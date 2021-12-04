#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

extern volatile uint8_t buffer[];

void buffer_clear(void);

#endif
