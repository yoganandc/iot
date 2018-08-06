#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#define MOTOR_END 1
#define MOTOR_X   0

void motor_init();
void motor_calibrate();
bool motor_go();
void motor_left();
void motor_right();
void motor_180();
bool motor_has_left();
bool motor_has_right();

#ifdef __cplusplus
}
#endif

#endif
