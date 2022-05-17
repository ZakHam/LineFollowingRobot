#include "motor.h"

#include <avr/io.h>

#include "util.h"

#define MOTOR_OUTPUT_PINS 0xfc

#define MOTOR_1_ENABLE DDD2
#define MOTOR_1_FORWARD DDD7
#define MOTOR_1_BACKWARD DDD6

#define MOTOR_2_ENABLE DDD3
#define MOTOR_2_FORWARD DDD5
#define MOTOR_2_BACKWARD DDD4

void init_motors() {
  DDRD |= MOTOR_OUTPUT_PINS;
  enable_motor_1(1);
  enable_motor_2(1);
}

void enable_motor_1(unsigned char enabled) {
  if (enabled) {
    PORTD |= BIT(MOTOR_1_ENABLE);
  } else {
    PORTD &= ~BIT(MOTOR_1_ENABLE);
  }
}

void enable_motor_2(unsigned char enabled) {
  if (enabled) {
    PORTD |= BIT(MOTOR_2_ENABLE);
  } else {
    PORTD &= ~BIT(MOTOR_2_ENABLE);
  }
}

void motor_1_forwards() {
  PORTD &= ~BIT(MOTOR_1_BACKWARD);
  PORTD |= BIT(MOTOR_1_FORWARD);
}

void motor_1_backwards() {
  PORTD &= ~BIT(MOTOR_1_FORWARD);
  PORTD |= BIT(MOTOR_1_BACKWARD);
}

void motor_1_stop() { PORTD &= ~(BIT(MOTOR_1_FORWARD) | BIT(MOTOR_1_BACKWARD)); }

void motor_2_forwards() {
  PORTD &= ~BIT(MOTOR_2_BACKWARD);
  PORTD |= BIT(MOTOR_2_FORWARD);
}

void motor_2_backwards() {
  PORTD &= ~BIT(MOTOR_2_FORWARD);
  PORTD |= BIT(MOTOR_2_BACKWARD);
}

void motor_2_stop() { PORTD &= ~(BIT(MOTOR_2_FORWARD) | BIT(MOTOR_2_BACKWARD)); }
