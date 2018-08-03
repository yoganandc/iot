#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#define MOTOR_END 1
#define MOTOR_X   0

void motor_init();
void motor_calibrate();
bool motor_go();
void motor_left();
void motor_right();
void motor_180();

#ifdef __cplusplus
}
#endif

#endif
