#include "adc.h"

#include <avr/io.h>

#include "util.h"

void init_adc() {
  // Configure the ADC
  // Set reference to AVcc = 5V
  ADMUX |= BIT(REFS0);

  // Disable the digital input buffer on ADC 0
  DIDR0 |= BIT(ADC0D);

  // Enable the ADC, and set the clock pre-scaler to 128.
  ADCSRA |= BIT(ADEN) | BIT(ADPS2) | BIT(ADPS1) | BIT(ADPS0);

  // Trigger an initial sample, as the first sample takes 25 clock cycles, not 13
  ADCSRA |= BIT(ADSC);

  while (ADCSRA & BIT(ADSC))
    ;
}

int sample_adc(unsigned char channel) {
  if (channel < 0 || channel > 7) {
    return -1;
  }

  // Clear the current channel
  ADMUX &= 0xF0;

  // Set the ADC channel
  ADMUX |= BIT(channel);

  // Trigger the conversion
  ADCSRA |= BIT(ADSC);

  // Wait for the result
  while (ADCSRA & BIT(ADSC))
    ;

  return ADC;
}
