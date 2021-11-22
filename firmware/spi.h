#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void spi_init(void);

void spi_activate(void);
void spi_deactivate(void);

uint8_t spi_send(uint8_t byte);
uint8_t spi_recv(void);
uint8_t spi_recv_ignore_ff(void);

#endif
