#include "Arduino.h"
#include "motor.h"
#include "sensor.h"
#include "logger.h"

#define PWM_A 3
#define PWM_B 11
#define DIR_A 2
#define DIR_B 4

#define SPEED_TURN 200
#define SPEED_SLOW 100
#define SPEED_CAL  140

#define LEFT 0
#define RIGHT 1

static void set_dir(int);
static void set_speed(int);
static void stop_left();
static void stop_right();

void motor_init()
{
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);

  set_speed(0);
  sensor_init();
}

void motor_calibrate()
{ 
  set_speed(SPEED_CAL);
  
  for(int i = 0; i <= 100; i++) { // timeout*100 = 2.5s
    if(i == 0 || i == 60) {
      set_dir(RIGHT);
    }
    else if(i == 20 || i == 100) {
      set_dir(LEFT);
    }
    sensor_calibrate();
  }

  stop_left();
}

void motor_go()
{
  
}

void motor_left()
{
  set_dir(LEFT);
  set_speed(SPEED_TURN);
}

void motor_right()
{
  set_dir(RIGHT);
  set_speed(SPEED_TURN);
}

void motor_180()
{
  
}

static void set_dir(int dir)
{
  digitalWrite(DIR_A, !dir);
  digitalWrite(DIR_B, dir);
}

static void set_speed(int speed)
{
  analogWrite(PWM_A, speed);
  analogWrite(PWM_B, speed);
}

static void stop_left()
{
  sensor_read();
  unsigned int *values = sensor_values();
  
  while(values[6] < SENSOR_THRESHOLD) {
    sensor_read();
    values = sensor_values();
  }

  set_speed(SPEED_SLOW);

  unsigned int pos = sensor_position();
  while(pos > SENSOR_CENTER_HIGH) {
    sensor_read();
    pos = sensor_position();
  }

  analogWrite(PWM_B, 0); // stop right first
  analogWrite(PWM_A, 0);
}

static void stop_right()
{
  sensor_read();
  unsigned int *values = sensor_values();
  
  while(values[1] < SENSOR_THRESHOLD) {
    sensor_read();
    values = sensor_values();
  }

  set_speed(SPEED_SLOW);

  unsigned int pos = sensor_position();
  while(pos < SENSOR_CENTER_LOW) {
    sensor_read();
    pos = sensor_position();
  }

  set_speed(0);
}

