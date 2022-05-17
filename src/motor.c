#include "motor.h"

#include <avr/io.h>

#define MOTOR_OUTPUT_PINS 0xfc

#define MOTOR_1_ENABLE DDD2
#define MOTOR_1_FORWARD DDD7
#define MOTOR_1_BACKWARD DDD6

#define MOTOR_2_ENABLE DDD3
#define MOTOR_2_FORWARD DDD5
#define MOTOR_2_BACKWARD DDD4

void motors_init() {
  DDRD |= MOTOR_OUTPUT_PINS;
  motor_1_enable(1);
  motor_2_enable(1);
}

void motor_1_enable(unsigned char enabled) {
  if (enabled) {
    PORTD |= _BV(MOTOR_1_ENABLE);
  } else {
    PORTD &= ~_BV(MOTOR_1_ENABLE);
  }
}

void motor_2_enable(unsigned char enabled) {
  if (enabled) {
    PORTD |= _BV(MOTOR_2_ENABLE);
  } else {
    PORTD &= ~_BV(MOTOR_2_ENABLE);
  }
}

void motor_1_forwards() {
  PORTD &= ~_BV(MOTOR_1_BACKWARD);
  PORTD |= _BV(MOTOR_1_FORWARD);
}

void motor_1_backwards() {
  PORTD &= ~_BV(MOTOR_1_FORWARD);
  PORTD |= _BV(MOTOR_1_BACKWARD);
}

void motor_1_stop() { PORTD &= ~(_BV(MOTOR_1_FORWARD) | _BV(MOTOR_1_BACKWARD)); }

void motor_2_forwards() {
  PORTD &= ~_BV(MOTOR_2_BACKWARD);
  PORTD |= _BV(MOTOR_2_FORWARD);
}

void motor_2_backwards() {
  PORTD &= ~_BV(MOTOR_2_FORWARD);
  PORTD |= _BV(MOTOR_2_BACKWARD);
}

void motor_2_stop() { PORTD &= ~(_BV(MOTOR_2_FORWARD) | _BV(MOTOR_2_BACKWARD)); }
