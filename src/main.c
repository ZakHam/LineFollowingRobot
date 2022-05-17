#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>

#include "uart.h"
#include "util.h"

int main() {
  DDRB = BIT(DDB5);
  init_serial_uart();

  // Global enable interrupts
  sei();

  while (1) {
    int c = read_byte();
    if (c >= 0) {
      write_byte((unsigned char)c);
    }
  }

  return 0;
}
