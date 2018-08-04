#ifndef PKT_H
#define PKT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define MSG_LINK 1
#define MSG_DV   2
#define MSG_REQ  3
#define MSG_RES  4

struct pkt_iter {
  uint8_t *buf;
  uint8_t  len;
  uint8_t  pos;
};

struct pkt_link {
  uint8_t node;
  uint8_t cost;
  uint8_t dir;
};

struct pkt_dv {
  uint8_t node;
  uint8_t cost;
};

struct pkt_req {
  uint8_t dst;
  uint8_t prev;
};

struct pkt_res {
  uint8_t next;
  uint8_t dir;
};

uint8_t pkt_type(uint8_t *buf);
void pkt_req_parse(uint8_t *buf, struct pkt_req *req);
void pkt_iterator(uint8_t *buf, uint8_t len, struct pkt_iter *iter);
bool pkt_has_next(struct pkt_iter *iter);
void pkt_link_next(struct pkt_iter *iter, struct pkt_link *entry);
void pkt_dv_next(struct pkt_iter *iter, struct pkt_dv *entry);

#ifdef __cplusplus
}
#endif

#endif
