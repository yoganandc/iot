#include <stdint.h>
#include <stdbool.h>
#include "pkt.h"

uint8_t pkt_type(uint8_t *buf)
{
  return buf[0];
}

void pkt_req_parse(uint8_t *buf, struct pkt_req *req)
{
  req->dst = buf[1];
  req->prev = buf[2];
}

void pkt_iterator(uint8_t *buf, uint8_t len, struct pkt_iter *iter)
{
  iter->buf = buf + 1;
  iter->len = len - 1;
  iter->pos = 0;
}

bool pkt_has_next(struct pkt_iter *iter)
{
  return iter->pos != iter->len;
}

void pkt_link_next(struct pkt_iter *iter, struct pkt_link *entry)
{
  entry->node = *(iter->buf++);
  entry->cost = *(iter->buf++);
  entry->dir = *(iter->buf++);
  iter->pos += 3;
}

void pkt_dv_next(struct pkt_iter *iter, struct pkt_dv *entry)
{
  entry->node = *(iter->buf++);
  entry->cost = *(iter->buf++);
  iter->pos += 2;
}


