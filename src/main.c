#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdio.h>

#include "adc.h"
#include "motor.h"
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
  adc_init();

  // Global enable interrupts
  sei();

  screen_init();

  uint16_t colours[] = {BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
  screen_start_write();
  screen_write_rect(0, 0, 129, 128, BLACK);
  screen_end_write();

  char buf[21];
  while (1) {
    const int val = adc_sample(0);
    sprintf(buf, "ADC: %d", val);
    screen_start_write();
    screen_write_rect(60, 32, 60, 8, BLACK);
    screen_write_string(60, 32, buf, WHITE, false);
    screen_end_write();
    _delay_ms(1000);
  }

  return 0;
}
