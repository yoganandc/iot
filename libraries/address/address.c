#include <stdint.h>
#include <constants.h>
#include "address.h"

static uint16_t addresses[MAX_ROUTERS] = {
  0x5678,
  0x90AB,
  0xCDEF,
  0x1011,
  0x1213
};

uint16_t id_to_address(uint8_t id)
{
  return addresses[id];
}

uint8_t address_to_id(uint16_t address) 
{
  for(int i = 0; i < MAX_ROUTERS; i++) {
    if(addresses[i] == address) {
      return i;
    }
  }
}


