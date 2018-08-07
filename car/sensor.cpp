#include <QTRSensors.h> 
#include "sensor.h"
#include "logger.h"

#define SENSOR_TIMEOUT 2500

static QTRSensorsRC *qtrrc;
static unsigned int values[SENSOR_COUNT];
static unsigned int position;

void sensor_init()
{
  static unsigned char pins[] = {5, 6, 7, 10, 12, 13, 14, 15};
  static QTRSensorsRC qtrrc0(pins, SENSOR_COUNT, SENSOR_TIMEOUT, QTR_NO_EMITTER_PIN);
  qtrrc = &qtrrc0;
}

void sensor_calibrate()
{
  qtrrc->calibrate();
}

void sensor_read()
{
  position = (unsigned int) qtrrc->readLine(values);
}

unsigned int sensor_position()
{
  return position;
}

unsigned int *sensor_values()
{
  return values;
}

void sensor_log()
{
  sensor_read();
  unsigned int pos = sensor_position();
  unsigned int *values = sensor_values();

  log_serial("pos = %d, values = [%d", pos, values[0]);
  for(int i = 1; i < SENSOR_COUNT; i++) {
    log_serial(", %d", values[i]);
  }
  log_serial("]");
}

