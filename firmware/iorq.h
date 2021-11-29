#ifndef IORQ_H_
#define IORQ_H_

#include <avr/io.h>

void    iorq_init(void);

uint8_t iorq_input(uint8_t cmd);
void    iorq_output(uint8_t cmd, uint8_t data);

#endif
