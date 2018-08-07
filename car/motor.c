#include <stdbool.h>
#include <Arduino.h>
#include "motor.h"
#include "sensor.h"
#include "logger.h"

#define btoa(x) ((x) ? "true" : "false")

#define PWM_L 3
#define PWM_R 11
#define DIR_L 2
#define DIR_R 4

#define SPEED            130
#define SPEED_SLOW       100
#define SPEED_SUPER_SLOW  80
#define SPEED_CAL        110

#define KP 0.02
#define KD 0.0

#define LEFT_EXTREME  7
#define LEFT_ALMOST   6
#define LEFT_CENTER   4
#define LEFT_X        5 // I have no idea why it's called this
#define RIGHT_EXTREME 0
#define RIGHT_ALMOST  1
#define RIGHT_CENTER  3
#define RIGHT_X       2

#define CORRECTION_LIMIT 55

#define LEFT 0
#define RIGHT 1

static void set_dir(int);
static void set_speed(int);
static void finish_left();
static void finish_right();
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

void motor_go()
{
  while(1) {
    sensor_read();
    unsigned int *values = sensor_values();

    bool has_right = values[RIGHT_EXTREME] == 1000 && values[RIGHT_ALMOST] == 1000 && values[RIGHT_X] == 1000;
    bool has_left = values[LEFT_EXTREME] == 1000 && values[LEFT_ALMOST] == 1000 && values[LEFT_X] == 1000;

    if(has_left || has_right) {
      set_speed(SPEED_SUPER_SLOW);
      
      while(values[RIGHT_ALMOST] > 100 || values[LEFT_ALMOST] > 100) {
        sensor_read();
        values = sensor_values();
      }

      set_speed(0);

      if(has_left && has_right) {
        return;
      }
      
      if(values[RIGHT_CENTER] > SENSOR_THRESHOLD || values[LEFT_CENTER] > SENSOR_THRESHOLD) {
        log_serial("has_straight = true\n");
        sensor_log();
        return;
      }
      
      if(has_left) {
        log_serial("going left\n");
        sensor_log();
        motor_left();
      }
      else {
        log_serial("going right\n");
        sensor_log();
        motor_right();
      }
    }

    else if(values[RIGHT_ALMOST] > SENSOR_THRESHOLD) {
      set_speed(SPEED_SLOW);
    }

    else if(values[LEFT_ALMOST] > SENSOR_THRESHOLD) {
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

  while(values[LEFT_CENTER] < SENSOR_THRESHOLD) {
    sensor_read();
    values = sensor_values();
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

  while(values[RIGHT_CENTER] < SENSOR_THRESHOLD) {
    sensor_read();
    values = sensor_values();
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

