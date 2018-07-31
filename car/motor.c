#include "Arduino.h"
#include "motor.h"

void motor_init()
{
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);

  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

