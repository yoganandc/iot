#ifndef PKT_H
#define PKT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define MSG_DST 3
#define MSG_RES 5

struct pkt {
  uint8_t data;
  uint8_t type;
};

void pkt_parse(struct pkt *pkt, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif
