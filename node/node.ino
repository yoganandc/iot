#include <stdbool.h>
#include <stdint.h>
#include <logger.h>
#include <rx_tx.h>
#include <constants.h>
#include "pkt.h"
#include "router.h"
#include "lcd.h"

#define XBEE_READ_CYCLE 50U
#define LCD_UPDATE_CYCLE 2000U
#define LOG_CYCLE 5000U

#define SOFT_SERIAL 4

static unsigned long max_time = 0;

void setup() 
{
  Serial.begin(9600);
  
  pinMode(SOFT_SERIAL, INPUT_PULLUP);
  bool dont_use_soft_serial = digitalRead(SOFT_SERIAL);

  if(dont_use_soft_serial) {
    log_init(LOG_NONE);
  }
  else {
    log_init(LOG_SOFT);
  }
  
  lcd_init();
  
  log_serial(F("Booting... "));
  xbee_init();
  log_serial(F("Done\n"));
  
  log_serial(F("Retrieving node ID... "));
  uint16_t node_address;
  while(!xbee_address(&node_address));
  log_serial(F("0x%X\n"), node_address);

  router_init(node_address);
}

void loop() 
{
  static unsigned long last_xbee_read = 0;
  static unsigned long last_lcd_updated = 0;
  static unsigned long last_logged = 0;
  
  if(check(&last_xbee_read, XBEE_READ_CYCLE)) {
    do_xbee_read();
  }
  
  if(check(&last_lcd_updated, LCD_UPDATE_CYCLE)) {
    router_lcd();
  }
  
  if(check(&last_logged, LOG_CYCLE)) {
    router_log();
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
    uint8_t type = pkt_type(data.buf);

    switch(type) {
      case MSG_LINK: {
        struct pkt_iter iter;
        pkt_iterator(data.buf, data.len, &iter);
        
        log_serial(F("Recieved LINK message\n"));
        process_link_update(&iter);
        break;
      }
      case MSG_DV: {
        struct pkt_iter iter;
        pkt_iterator(data.buf, data.len, &iter);
        
        log_serial(F("Received DV message from 0x%X\n"), data.address);
        process_dv_update(data.address, &iter);
        break;
      }
      case MSG_REQ: {
        struct pkt_req req;
        pkt_req_parse(data.buf, &req);
        
        log_serial(F("Received REQ message\n"));
        process_req(&req);
      }
    }
  }
}

