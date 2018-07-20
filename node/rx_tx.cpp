#include <XBee.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <stdbool.h>
#include "rx_tx.h"

static SoftwareSerial xbee_serial(2, 3);
static XBee xbee;

void xbee_init() 
{
  xbee_serial.begin(9600);
  xbee.setSerial(xbee_serial);
  delay(5000);
}

bool xbee_address(uint16_t *address) 
{
  uint8_t command[] = {'M', 'Y'};
  AtCommandRequest req(command);
  xbee.send(req);

  if(xbee.readPacket(5000)) {
    if(xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      
      AtCommandResponse res;
      xbee.getResponse().getAtCommandResponse(res);

      if(res.isOk()) {
        if(res.getValueLength() == 2) {
          uint8_t *data = res.getValue();
          *address = ((uint16_t) data[0] << 8) | data[1];
          return true;
        }
      }
    }
  }

  return false;
}

bool xbee_rx(struct xbee_data *data) 
{
  Rx16Response rx16 = Rx16Response();
  xbee.readPacket();
  
  if(xbee.getResponse().isAvailable()) {
    if(xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      xbee.getResponse().getRx16Response(rx16);
      data->buf = rx16.getData();
      data->len = rx16.getDataLength();
      data->address = rx16.getRemoteAddress16();
      return true;
    }
  }

  return false;
}

void xbee_tx(struct xbee_data *data) 
{
  Tx16Request tx16 = Tx16Request(data->address, data->buf, data->len);
  xbee.send(tx16);
}

