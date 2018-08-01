#include <stdbool.h>
#include <logger.h>
#include <rx_tx.h>
#include "motor.h"
#include "pkt.h"

#define XBEE_READ_CYCLE 50U
#define LOG_CYCLE 5000U

void setup() 
{
  Serial.begin(57600);
  log_init(LOG_UART);
  
  motor_init();
  delay(5000);
  motor_calibrate();
  
//  log_init(LOG_SOFT);
//  
//  log_serial(F("Booting... "));
//  xbee_init();
//  log_serial(F("Done\n"));
//  
//  log_serial(F("Retrieving node ID... "));
//  uint16_t node_address;
//  while(!xbee_address(&node_address));
//  log_serial(F("0x%X\n"), node_address);
}

void loop() 
{
  for(;;);
  static unsigned long last_xbee_read = 0;
  static unsigned long last_logged = 0;

  if(check(&last_xbee_read, XBEE_READ_CYCLE)) {
    do_xbee_read();
  }
  
  if(check(&last_logged, LOG_CYCLE)) {
    do_log();
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
  struct xbee_data data;

  if(xbee_rx(&data)) {
    struct pkt pkt;
    pkt_parse(&pkt, data.buf);

    if(pkt.type == MSG_DST) {
      log_serial(F("Received MSG_DST packet: %d\n"), pkt.data); 
    }
    else if(pkt.type == MSG_RES) {
      log_serial(F("Received MSG_RES packet: %d\n"), pkt.data); 
    }
  }
}

void do_log()
{
  static unsigned long ctr = 0;
  log_serial(F("Still Alive! %lu\n"), ++ctr);
}

