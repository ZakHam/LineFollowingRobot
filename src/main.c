#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "usart.h"

int main() {
  DDRB = _BV(DDB5);
  usart_init();

  // Global enable interrupts
  sei();

  while (1) {
    int c = usart_read_byte();
    if (c >= 0) {
      usart_write_byte((unsigned char)c);
    }
  }

  return 0;
}
