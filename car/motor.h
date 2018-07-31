#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#define PWM_A 3
#define PWM_B 11
#define DIR_A 2
#define DIR_B 4

void motor_init();

#ifdef __cplusplus
}
#endif

#endif
