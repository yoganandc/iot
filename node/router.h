#ifndef ROUTER_H
#define ROUTER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "pkt.h"

void router_init(uint16_t node_address);
void process_link_update(struct pkt *pkt);
void process_dv_update(uint16_t from_address, struct pkt *pkt);
void router_log();

#ifdef __cplusplus
}
#endif

#endif
