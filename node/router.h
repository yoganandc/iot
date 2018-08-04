#ifndef ROUTER_H
#define ROUTER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "pkt.h"

void router_init(uint16_t node_address);
void process_link_update(struct pkt_iter *iter);
void process_dv_update(uint16_t from_address, struct pkt_iter *iter);
void process_req(struct pkt_req *req);
void router_log();
void router_lcd();

#ifdef __cplusplus
}
#endif

#endif
