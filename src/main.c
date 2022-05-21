#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdint.h>

#include "screen/screen.h"
#include "usart.h"

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

int main() {
  usart_init();

  // Global enable interrupts
  sei();

  screen_init();

  uint16_t colours[] = {BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
  screen_start_write();
  int col_index = 0;
  for (int i = 0; i < 128; i += 32) {
    for (int j = 0; j < 128; j += 32) {
      screen_write_rect(i, j, 32, 32, colours[col_index]);
      col_index++;
      if (col_index == 8) {
        col_index = 0;
      }
    }
  }
  screen_end_write();

  while (1) {
    _delay_ms(1000);
  }

  return 0;
}
