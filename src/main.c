#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>

#include "screen.h"
#include "spi.h"
#include "usart.h"

int main() {
  usart_init();

  // Global enable interrupts
  sei();

  screen_init();

  screen_start_write();
  for (int i = 0; i < 128; i++) {
    for (int j = 0; j < 128; j++) {
      screen_write_pixel(i, j, 0xffff);
    }
  }
  for (int i = 50; i < 60; i++) {
    for (int j = 60; j < 90; j++) {
      screen_write_pixel(i, j, 0xa932);
    }
  }
  screen_end_write();

  while (1) {
    _delay_ms(1000);
  }

  return 0;
}
