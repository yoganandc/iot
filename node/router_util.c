#include <stdint.h>
#include "router_util.h"

static uint16_t addresses[MAX_ROUTERS] = {
  0x5678,
  0x90AB,
  0xCDEF,
  0x1011,
  0x1213
};

uint8_t turn_table[MAX_NEIGHBORS][MAX_NEIGHBORS] = {
  {STRAIGHT, RIGHT, TURNAROUND, LEFT}, // HEADING NORTH
  {LEFT, STRAIGHT, RIGHT, TURNAROUND}, // EAST
  {TURNAROUND, LEFT, STRAIGHT, RIGHT}, // SOUTH
  {RIGHT, TURNAROUND, LEFT, STRAIGHT}  // WEST
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


