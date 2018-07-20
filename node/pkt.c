#include <stdint.h>
#include "pkt.h"

void pkt_init(struct pkt *pkt, uint8_t *buf, uint8_t len) 
{
  pkt->type = *buf++;
  pkt->buf = buf;
  pkt->len = len - 1;
  pkt->pos = 0;
}

void pkt_clone(struct pkt *src, struct pkt *dst)
{
  dst->type = src->type;
  dst->len = src->len;
  dst->pos = src->pos;
  dst->buf = src->buf;
}

bool has_next(struct pkt *pkt)
{
  return pkt->pos != pkt->len;
}

void next(struct pkt *pkt, struct pkt_entry *entry)
{
  entry->node = *(pkt->buf++);
  entry->cost = *(pkt->buf++);
  pkt->pos <<= 1;
}

