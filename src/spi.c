#include "spi.h"

#include <avr/io.h>
#include <util/atomic.h>

#define SCK PB5
#define MISO PB4
#define MOSI PB3
#define SS PB2

void spi_init() {
  // Wrap the init in a no interrupt block
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Write the slave select high
    PORTB |= _BV(SS);

    // Set SS as output
    DDRB |= _BV(SS);

    // Set clock divider to 2, for 8MHz
    SPSR = _BV(SPI2X);

    // Enable SPI, set MASTER mode
    SPCR = _BV(SPE) | _BV(MSTR);

    // Set SCK, MOSI to output after enabling to avoid reading a bit erroneously
    DDRB |= _BV(SCK) | _BV(MOSI);
  }
}

uint8_t spi_send_byte(uint8_t data) {
  // Set the data to write
  SPDR = data;

  // A single nop may prevent the while loop forming if speed is high
  asm volatile("nop");
  while (!(SPSR & _BV(SPIF)))
    ;

  // Return the data received
  return SPDR;
}

uint16_t spi_send_double_byte(uint16_t data) {
  return (spi_send_byte(data >> 8) << 8) | spi_send_byte(data);
}
