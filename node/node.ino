#include <stdbool.h>
#include <stdint.h>
#include "pkt.h"
#include "router.h"
#include "rx_tx.h"
#include "logger.h"
#include "logger_flash.h"

#define XBEE_READ_CYCLE 50U
#define LCD_UPDATE_CYCLE 1000U
#define LOG_CYCLE 5000U

#define SERIAL_SELECT 4
#define LCD_V0 5
#define LCD_CONTRAST 0.1

unsigned long last_xbee_read = 0;
unsigned long last_lcd_updated = 0;
unsigned long last_logged = 0;

void setup() 
{
  Serial.begin(9600);
  
  pinMode(SERIAL_SELECT, INPUT_PULLUP);
  pinMode(LCD_V0, OUTPUT);
  analogWrite(LCD_V0, LCD_CONTRAST * 255);

  bool xbee_uses_uart = digitalRead(SERIAL_SELECT);
  log_init(!xbee_uses_uart); // if xbee uses uart, then the logger can't
  
  log_serial(F("Booting... "));
  xbee_init(xbee_uses_uart);
  log_serial(F("Done\n"));
  
  log_serial(F("Retrieving node ID... "));
  uint16_t node_address;
  while(!xbee_address(&node_address));
  log_serial(F("0x%X\n"), node_address);
  
  router_init(node_address);
}

void loop() 
{
//  for(;;);
  static unsigned long max_time = 0;
  
  if(check(&last_xbee_read, XBEE_READ_CYCLE)) {
    unsigned long start_time = millis();
    do_xbee_read();
    unsigned long stop_time = millis();
    if(max_time < stop_time - start_time) {
      max_time = stop_time - start_time;
    }
  }
  if(check(&last_lcd_updated, LCD_UPDATE_CYCLE)) {
    do_lcd_update();
  }
  if(check(&last_logged, LOG_CYCLE)) {
    router_log();
    log_serial(F("Max time for xbee handler: %ul\n"), max_time);
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
    pkt_init(&pkt, data.buf, data.len);
  
    if(pkt.type == MSG_LINK) {
      log_serial(F("Recieved LINK message\n"));
      process_link_update(&pkt);
    }
    else {
      log_serial(F("Received DV message from 0x%X\n"), data.address);
      process_dv_update(data.address, &pkt);
    }
  }
}

void do_lcd_update() 
{
}
