#include <stdint.h>
#include "pkt.h"

void pkt_init(struct pkt *pkt, uint8_t *buf, uint8_t len) 
{
  pkt->type = *buf++;
  pkt->buf = buf;
  pkt->len = len - 1;
  pkt->pos = 0;
}

bool has_next(struct pkt *pkt)
{
  return pkt->pos != pkt->len;
}

void next(struct pkt *pkt, struct pkt_entry *entry)
{
  if(pkt->type == MSG_LINK) {
    entry->node = *(pkt->buf++);
    entry->cost = *(pkt->buf++);
    entry->dir = *(pkt->buf++);
    pkt->pos += 3;
  }
  else {
    entry->node = *(pkt->buf++);
    entry->cost = *(pkt->buf++);
    pkt->pos += 2;
  }
}

