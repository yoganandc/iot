#include <stdbool.h>
#include <Arduino.h>
#include "motor.h"
#include "sensor.h"
#include "logger.h"

#define PWM_L 3
#define PWM_R 11
#define DIR_L 2
#define DIR_R 4

#define SPEED      130
#define SPEED_SLOW 100
#define SPEED_CAL  110

#define KP 0.04
#define KD 0.0

#define LEFT_EXTREME 7
#define LEFT_ALMOST 6
#define RIGHT_EXTREME 0
#define RIGHT_ALMOST 1

#define CORRECTION_LIMIT 55

#define LEFT 0
#define RIGHT 1

static void set_dir(int);
static void set_speed(int);
static void finish_left();
static void finish_right();
static bool detect();
static void correct();

void motor_init()
{
  pinMode(PWM_L, OUTPUT);
  pinMode(PWM_R, OUTPUT);
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);

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

  finish_left();
}

bool motor_go()
{
  while(1) {
    sensor_read();
    unsigned int *values = sensor_values();

    if(values[RIGHT_EXTREME] == 1000 || values[LEFT_EXTREME] == 1000) {
//      log_serial("hit intersection\n");
      set_speed(0);
      return detect();
    }

    if(values[RIGHT_ALMOST] > SENSOR_THRESHOLD) {
//      log_serial("hit right sensor, must correct right\n");
      set_speed(SPEED_SLOW);
    }

    if(values[LEFT_ALMOST] > SENSOR_THRESHOLD) {
//      log_serial("hit left sensor, must correct left\n");
      set_speed(SPEED_SLOW);
    }
    
    correct();
  }
}

void motor_left()
{
  set_dir(LEFT);
  set_speed(SPEED);
  finish_left();
}

void motor_right()
{
  set_dir(RIGHT);
  set_speed(SPEED);
  finish_right();
}

void motor_180()
{
  
}

bool motor_has_left()
{
  sensor_read();
  uint8_t *values = sensor_values;
  return sensor[LEFT_EXTREME] > SENSOR_THRESHOLD;
}

bool motor_has_right()
{
  sensor_read();
  uint8_t *values = sensor_values;
  return sensor[RIGHT_EXTREME] > SENSOR_THRESHOLD;
}

static void set_dir(int dir)
{
  digitalWrite(DIR_L, !dir);
  digitalWrite(DIR_R, dir);
}

static void set_speed(int speed)
{
  analogWrite(PWM_L, speed);
  analogWrite(PWM_R, speed);
}

static void finish_left()
{
  sensor_read();
  unsigned int *values = sensor_values();

  // left sensor hitting tape means we are almost
  // done
  while(values[LEFT_EXTREME] < SENSOR_THRESHOLD) {
    sensor_read();
    values = sensor_values();
  }

  set_speed(SPEED_SLOW);

  unsigned int pos = sensor_position();
  while(pos > SENSOR_CENTER_HIGH) {
    sensor_read();
    pos = sensor_position();
  }

  analogWrite(PWM_R, 0); // stop right first
  analogWrite(PWM_L, 0);
}

static void finish_right()
{
  sensor_read();
  unsigned int *values = sensor_values();

  // right sensor hitting tape means we are almost done
  // turning
  while(values[RIGHT_EXTREME] < SENSOR_THRESHOLD) {
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

static void correct() 
{
  static int last_error;
  static bool first_time;
  double correction;
  int error = (int) sensor_position() - 3500;

  if(first_time) {
    first_time = false;
    correction = KP * error;
  }
  else {
    correction = KP * error + KD * (error - last_error);
  }
  last_error = error;
    
  if(correction > CORRECTION_LIMIT) {
    correction = CORRECTION_LIMIT;
  }
  if(correction < -CORRECTION_LIMIT) {
    correction = -CORRECTION_LIMIT;
  }
  
  int m1 = SPEED - correction;
  int m2 = SPEED + correction;

//  if(m1 > m2) {
//    log_serial("correct right (%d), error: %d m_l: %d m_r: %d\n", (int) correction, (int) sensor_position() - 3500, m1, m2);
//  }
//  else {
//    log_serial("correct left (%d), error: %d m_l: %d m_r: %d\n",(int) correction, (int) sensor_position() - 3500, m1, m2);
//  }

  digitalWrite(DIR_L, LOW);
  analogWrite(PWM_L, m1);
  digitalWrite(DIR_R, LOW);
  analogWrite(PWM_R, m2);
}

static bool detect()
{
  unsigned int *values = sensor_values();
  bool is_end = true;

  // all sensors must be not receiving any reflected IR 
  // if it's a dead end
  for(int i = 0; i < SENSOR_COUNT; i++) {
    is_end &= values[i] > SENSOR_THRESHOLD;
  }

  return is_end;
}

