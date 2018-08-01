#include "pkt.h"

void pkt_parse(struct pkt *pkt, uint8_t *buf)
{
  pkt->type = *(buf++);
  pkt->data = *buf;
}

