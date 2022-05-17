#ifndef __MOTOR_H__
#define __MOTOR_H__

void motors_init();

void motor_1_enable(unsigned char enabled);
void motor_2_enable(unsigned char enabled);

void motor_1_forwards();
void motor_1_backwards();
void motor_1_stop();

void motor_2_forwards();
void motor_2_backwards();
void motor_2_stop();

#endif // __MOTOR_H__
