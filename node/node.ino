#include <XBee.h>
#include <SoftwareSerial.h>
#include "pkt.h"
#include "address.h"
#include "router.h"

#define XBEE_READ_CYCLE 200U
#define LCD_UPDATE_CYCLE 1000U

unsigned long last_xbee_read = 0;
unsigned long last_lcd_updated = 0;

SoftwareSerial xbee_serial(2, 3);
XBee xbee;

void setup() 
{
  Serial.begin(9600);
  xbee_serial.begin(9600);
  xbee.setSerial(xbee_serial);
}

void loop() 
{
  if(check(&last_xbee_read, XBEE_READ_CYCLE)) {
    do_xbee_read();
  }
  if(check(&last_lcd_updated, LCD_UPDATE_CYCLE)) {
    do_lcd_update();
  }
}

boolean check(unsigned long *last_millis, unsigned int cycle)
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
  Rx16Response rx16 = Rx16Response();
  xbee.readPacket();
  
  if(xbee.getResponse().isAvailable()) {
    if(xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      xbee.getResponse().getRx16Response(rx16);

      uint8_t *buf = rx16.getData();
      uint8_t len = rx16.getDataLength();
      uint8_t from = address_to_id(rx16.getRemoteAddress16());

      struct pkt pkt;
      pkt_init(&pkt, buf, len);

      if(pkt.type == MSG_LINK) {
        process_link_update(from, &pkt);
      }
      else {
        process_dv_update(from, &pkt);
      }
    }
  }
}

//void do_xbee_read() {
//  Rx16Response rx16 = Rx16Response();
//  xbee.readPacket();
//  
//  if(xbee.getResponse().isAvailable()) {
//    if(xbee.getResponse().getApiId() == RX_16_RESPONSE) {
//      xbee.getResponse().getRx16Response(rx16);
//
//      Serial.print("Received a packet from ");
//      Serial.print(rx16.getRemoteAddress16(), HEX);
//      Serial.print(": ");
//    
//      uint8_t *rx = rx16.getData();
//      
//      for(int i = 0; i < rx16.getDataLength(); i++) {
//        Serial.print(rx[i], HEX);
//        Serial.print(" ");
//      }
//    
//      Serial.println();
//    }
//  }
//  else {
//    Serial.println("Received no packet");
//  }
//}

void do_lcd_update() 
{
}

