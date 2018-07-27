#include <stdbool.h>
#include <stdint.h>
#include "pkt.h"
#include "router.h"
#include "rx_tx.h"

#define XBEE_READ_CYCLE 50U
#define LCD_UPDATE_CYCLE 1000U
#define LOG_CYCLE 5000U

unsigned long last_xbee_read = 0;
unsigned long last_lcd_updated = 0;
unsigned long last_logged = 0;

void setup() 
{
  Serial.begin(9600);
  Serial.print(F("Booting... "));
  xbee_init();
  Serial.println(F("Done"));
  
  Serial.print(F("Retrieving node ID... "));
  uint16_t node_address;
  while(!xbee_address(&node_address));
  Serial.println(node_address, HEX);
  
  router_init(node_address);
}

void loop() 
{
//  for(;;);
  if(check(&last_xbee_read, XBEE_READ_CYCLE)) {
    do_xbee_read();
  }
  if(check(&last_lcd_updated, LCD_UPDATE_CYCLE)) {
    do_lcd_update();
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
    struct pkt pkt;
    pkt_init(&pkt, data.buf, data.len);
  
    if(pkt.type == MSG_LINK) {
      Serial.println(F("Recieved LINK message"));
      process_link_update(&pkt);
    }
    else {
      Serial.print(F("Received DV message from "));
      Serial.println(data.address, HEX);
      process_dv_update(data.address, &pkt);
    }
  }
}

void do_lcd_update() 
{
}
