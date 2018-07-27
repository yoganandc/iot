#ifndef ROUTER_UTIL_H
#define ROUTER_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define MAX_ROUTERS 20
#define MAX_NEIGHBORS 4

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define STRAIGHT 0
#define RIGHT 1
#define TURNAROUND 2
#define LEFT 3 

uint16_t id_to_address(uint8_t id);
uint8_t address_to_id(uint16_t address);

#ifdef __cplusplus
}
#endif

#endif
