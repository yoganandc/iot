#include "address.h"
#include "router.h"

static uint8_t num_routers = 5;
static uint16_t addresses[MAX_ROUTERS] = {
  0x1234,
  0x5678,
  0x90AB,
  0xCDEF,
  0x1011
};

uint16_t id_to_address(uint8_t id)
{
  return addresses[id];
}

uint8_t address_to_id(uint16_t address) 
{
  for(int i = 0; i < num_routers; i++) {
    if(addresses[i] == address) {
      return i;
    }
  }
}


