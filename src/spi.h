#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

void spi_init();

unsigned char spi_send_byte(uint8_t data);

#endif // __SPI_H__
