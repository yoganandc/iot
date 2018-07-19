#ifndef ADDRESS_H
#define ADDRESS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

uint16_t id_to_address(uint8_t id);
uint8_t address_to_id(uint16_t address);

#ifdef __cplusplus
}
#endif

#endif
