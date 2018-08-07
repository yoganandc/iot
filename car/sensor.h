#ifndef SENSOR_H
#define SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#define SENSOR_COUNT   8

#define SENSOR_CENTER      3500
#define SENSOR_CENTER_LOW  3000
#define SENSOR_CENTER_HIGH 4000
#define SENSOR_THRESHOLD   400

void sensor_init();
void sensor_calibrate();
void sensor_read();
unsigned int sensor_position();
unsigned int *sensor_values();
void sensor_log();

#ifdef __cplusplus
}
#endif

#endif
