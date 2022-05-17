#include "spi.h"

#include <avr/io.h>

#include "util.h"

void init_spi() {
  // Set MOSI, SCK as output
  DDRB |= BIT(DDB5) | BIT(DDB3);

  // Set clock divider to 2, for 8MHz
  SPSR = BIT(SPI2X);

  // Enable SPI, set MASTER mode
  SPCR = BIT(SPE) | BIT(MSTR) | BIT(SPI2X);
}

void send_data(unsigned char data) {
  SPDR = data;

  while (!(SPSR & BIT(SPIF)))
    ;
}
