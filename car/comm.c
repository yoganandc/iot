#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <rx_tx.h>
#include <logger.h>
#include <constants.h>
#include "comm.h"
#include "address.h"

struct pkt {
  uint8_t type;
  uint8_t *buf;
};

static uint8_t *recv_pkt(uint8_t type);
static void recv(struct pkt *pkt);
static void pkt_parse(struct pkt *pkt, uint8_t *buf);

int comm_init()
{
  xbee_init();
  
  uint16_t node_address;
  while(!xbee_address(&node_address));
  return (int) node_address;
}

void comm_send_req(int to, struct comm_req *req)
{
  struct xbee_data data;
  uint8_t buf[3];
  
  buf[0] = MSG_REQ;
  buf[1] = (uint8_t) req->dst_node;
  buf[2] = (uint8_t) req->prev_node;

  data.address = id_to_address(to);
  data.buf = buf;
  data.len = 3;

  xbee_tx(&data);
}

void comm_recv_res(struct comm_res *res)
{
  uint8_t *buf = recv_pkt(MSG_RES);
  res->next_node = buf[0];
  res->next_dir = buf[1];
}

void comm_recv_stx(struct comm_stx *stx)
{
  uint8_t *buf = recv_pkt(MSG_STX);
  stx->src_node = buf[0];
  stx->next_node = buf[1];
  stx->dst_node = buf[2];
}

static uint8_t *recv_pkt(uint8_t type) {
  struct pkt ret;
  
  while(true) {
    recv(&ret);

    if(ret.type == type) {
      return ret.buf;
    }
  }
}

static void recv(struct pkt *pkt) {
  struct xbee_data data;

  while(true) {
    if(xbee_rx(&data)) {
      pkt_parse(pkt, data.buf);
      return;
    }

    delay(10);
  }
}

static void pkt_parse(struct pkt *pkt, uint8_t *buf)
{
  pkt->type = *(buf++);
  pkt->buf = buf;
}

