#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

void spi_init();

uint8_t spi_send_byte(uint8_t data);

uint16_t spi_send_double_byte(uint16_t data);

#endif // __SPI_H__
