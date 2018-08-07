#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

void motor_init();
void motor_calibrate();
void motor_go();
void motor_left();
void motor_right();
void motor_180();

#ifdef __cplusplus
}
#endif

#endif
