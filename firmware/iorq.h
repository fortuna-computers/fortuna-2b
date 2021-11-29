#ifndef IORQ_H_
#define IORQ_H_

#include <avr/io.h>

void     iorq_init(void);

uint8_t  iorq_post_byte(void);

uint16_t iorq_output(uint8_t cmd);

#endif
