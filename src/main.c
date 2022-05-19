#include <avr/interrupt.h>
#include <avr/io.h>

#include <stdint.h>

#include "screen.h"
#include "usart.h"

int main() {
  DDRB = _BV(DDB5);
  usart_init();

  screen_init();

  // Global enable interrupts
  sei();

  usart_write_string("Starting write\n");
  screen_start_write();

  usart_write_string("Writing pixels\n");
  screen_write_pixel(10, 10, 0xFFFF);
  screen_write_pixel(11, 11, 0xFFFF);
  screen_write_pixel(20, 20, 0x3F2F);
  screen_write_pixel(30, 30, 0x0000);

  usart_write_string("Ending write\n");
  screen_end_write();

  while (1) {
    int c = usart_read_byte();
    if (c >= 0) {
      usart_write_byte((uint8_t)c);
    }
  }

  return 0;
}
