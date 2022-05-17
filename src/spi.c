#include "spi.h"

#include <avr/io.h>

void spi_init() {
  // Set MOSI, SCK as output
  DDRB |= _BV(DDB5) | _BV(DDB3);

  // Set clock divider to 2, for 8MHz
  SPSR = _BV(SPI2X);

  // Enable SPI, set MASTER mode
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPI2X);
}

void spi_send_byte(unsigned char data) {
  SPDR = data;

  while (!(SPSR & _BV(SPIF)))
    ;
}
