#include <stdbool.h>
#include <logger.h>
#include <rx_tx.h>
#include "motor.h"

#define XBEE_READ_CYCLE 50U

void setup() 
{
  Serial.begin(9600);

  motor_init();
  log_init(LOG_SOFT);
  
  log_serial(F("Booting... "));
  xbee_init(true);
  log_serial(F("Done\n"));
  
  log_serial(F("Retrieving node ID... "));
  uint16_t node_address;
  while(!xbee_address(&node_address));
  log_serial(F("0x%X\n"), node_address);
}

void loop() 
{
  for(;;);
  static unsigned long last_xbee_read = 0;

  if(check(&last_xbee_read, XBEE_READ_CYCLE)) {
    do_xbee_read();
  }
}

bool check(unsigned long *last_millis, unsigned int cycle)
{
  unsigned long current_millis = millis();
  if(current_millis - *last_millis >= cycle) {
    *last_millis = current_millis;
    return true;
  }

  return false;
}

void do_xbee_read() 
{
  
}

