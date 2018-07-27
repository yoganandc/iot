#include <stdbool.h>
#include <stdint.h>
#include "router.h"
#include "pkt.h"
#include "rx_tx.h"
#include "logger.h"
#include "router_util.h"

static const uint8_t INF = 255;

static uint8_t self;
static uint8_t buf[MAX_ROUTERS*2 + 1];

static uint8_t next_hop[MAX_ROUTERS];
static uint8_t dv[MAX_ROUTERS][MAX_ROUTERS];
static uint8_t neighbors[MAX_ROUTERS];

static uint8_t num_neighbors; // for dv and next_hop
static uint8_t num_routers; // for neighbors

static bool bf();
static void update_neighbors();
static void log_dv();

void router_init(uint16_t node_address) 
{
  for(int i = 0; i < MAX_ROUTERS; i++) {
    for(int j = 0; j < MAX_ROUTERS; j++) {
      if(i != j) {
        dv[i][j] = INF;
      }
    }
  }

  self = address_to_id(node_address);
}

void process_link_update(struct pkt *pkt) 
{
  uint8_t count = 0;
  
  while(has_next(pkt)) {
    struct pkt_entry entry;
    next(pkt, &entry);
    
    neighbors[count++] = entry.node;

    if(entry.node >= num_routers) {
      num_routers = entry.node + 1;
    }

    dv[self][entry.node] = entry.cost;
  }

  num_neighbors = count;

  bf();
  update_neighbors();
  log_dv();
}

void process_dv_update(uint16_t from_address, struct pkt *pkt)
{
  bool changed = false;
  uint8_t from = address_to_id(from_address);

  while(has_next(pkt)) {
    struct pkt_entry entry;
    next(pkt, &entry);

    if(entry.node >= num_routers) {
      num_routers = entry.node + 1;
      changed = true;
    }

    dv[from][entry.node] = entry.cost;
  }

  if(bf() || changed) {
    update_neighbors();
    log_dv();
  }
}

static bool bf() 
{
  bool changed = false;
  
  for(int dst = 0; dst < num_routers; dst++) {
    if(dst == self) {
      continue;
    }
    
    uint8_t best_cost = INF;
    uint8_t best_hop;

    for(int j = 0; j < num_neighbors; j++) {
      int neighbor = neighbors[j];
      int curr = dv[self][neighbor] + dv[neighbor][dst];

      if(curr < best_cost) {
        best_cost = curr;
        best_hop = neighbor;
      }
    }

    changed = dv[self][dst] != best_cost;
    dv[self][dst] = best_cost;
    next_hop[dst] = best_hop;
  }

  return changed;
}

static void update_neighbors()
{
  int idx = 0;
  
  buf[idx++] = num_routers;
  for(int i = 0; i < num_routers; i++) {
    buf[idx++] = i;
    buf[idx++] = dv[self][i];
  }
  
  for(int i = 0; i < num_neighbors; i++) {
    struct xbee_data data;
    data.buf = buf;
    data.len = 2*num_routers + 1;
    data.address = id_to_address(neighbors[i]);

    xbee_tx(&data);
  }
}

static void log_dv() 
{
  log_serial("\n");
  log_serial("   ");

  for(int i = 0; i < num_routers; i++) {
    log_serial(" | %3d", i);
  }
  log_serial("\n");
  log_serial("---");

  for(int i = 0; i < num_routers; i++) {
    log_serial("------", i);
  }
  log_serial("\n");
  
  for(int i = 0; i < num_routers; i++) {
    log_serial("%3d", i);
    for(int j = 0; j < num_routers; j++) {
      log_serial(" | %3d", dv[i][j]);
    }
    log_serial("\n");
  }
  log_serial("\n");
}

