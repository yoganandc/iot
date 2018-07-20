#ifndef PKT_H
#define PKT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define MSG_LINK 1
#define MSG_DV 2

struct pkt {
  uint8_t len;
  uint8_t *buf;
  uint8_t type;
  uint8_t pos;
};

struct pkt_entry {
  uint8_t node;
  uint8_t cost;
};

void pkt_init(struct pkt *pkt, uint8_t *buf, uint8_t len);
bool has_next(struct pkt *pkt);
void next(struct pkt *pkt, struct pkt_entry *entry);

#ifdef __cplusplus
}
#endif

#endif
