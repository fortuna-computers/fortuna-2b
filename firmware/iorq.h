#ifndef IORQ_H_
#define IORQ_H_

#include <avr/io.h>

void iorq_init(void);
void iorq_release_wait(void);

#endif
