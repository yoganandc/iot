#ifndef XBEE_TX_H
#define XBEE_TX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

struct xbee_data {
  uint8_t *buf;
  uint8_t len;
  uint16_t address;
};

void xbee_init();
bool xbee_address(uint16_t *address);
bool xbee_rx(struct xbee_data *);
void xbee_tx(struct xbee_data *);

#ifdef __cplusplus
}
#endif
#endif
