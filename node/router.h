#ifndef ROUTER_H
#define ROUTER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "pkt.h"
#include <stdint.h>

#define MAX_ROUTERS 20

void router_init(uint8_t node_id);
void process_link_update(uint8_t from, struct pkt *pkt);
void process_dv_update(uint8_t from, struct pkt *pkt);

#ifdef __cplusplus
}
#endif

#endif
