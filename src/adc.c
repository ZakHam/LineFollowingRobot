#include "adc.h"

#include <avr/io.h>

void adc_init() {
  // Configure the ADC
  // Set reference to AVcc = 5V
  ADMUX |= _BV(REFS0);

  // Disable the digital input buffer on ADC 0
  DIDR0 |= _BV(ADC0D);

  // Enable the ADC, and set the clock pre-scaler to 128.
  ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

  // Trigger an initial sample, as the first sample takes 25 clock cycles, not 13
  ADCSRA |= _BV(ADSC);

  while (ADCSRA & _BV(ADSC))
    ;
}

int16_t adc_sample(uint8_t channel) {
  if (channel < 0 || channel > 7) {
    return -1;
  }

  // Clear the current channel
  ADMUX &= 0xF0;

  // Set the ADC channel
  ADMUX |= _BV(channel);

  // Trigger the conversion
  ADCSRA |= _BV(ADSC);

  // Wait for the result
  while (ADCSRA & _BV(ADSC))
    ;

  return ADC;
}
