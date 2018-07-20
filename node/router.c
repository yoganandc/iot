#include <stdbool.h>
#include <stdint.h>
#include "router.h"
#include "pkt.h"
#include "rx_tx.h"
#include "logger.h"

static const uint8_t INF = 255;

static uint8_t self;

static uint8_t next_hop[MAX_ROUTERS];
static uint8_t dv[MAX_ROUTERS][MAX_ROUTERS];
static uint8_t neighbors[MAX_ROUTERS];

static uint8_t num_neighbors; // for dv and next_hop
static uint8_t num_routers; // for neighbors

static bool bf();
static void update_neighbors();

void router_init(uint8_t node_id) {
  for(int i = 0; i < MAX_ROUTERS; i++) {
    for(int j = 0; j < MAX_ROUTERS; j++) {
      if(i != j) {
        dv[i][j] = INF;
      }
    }
  }

  self = node_id;
}

void process_link_update(struct pkt *pkt) 
{
  uint8_t count = 0;
  
  while(has_next(pkt)) {
    struct pkt_entry entry;
    next(pkt, &entry);
    neighbors[count++] = entry.node;

    if(entry.node >= num_routers) {
      num_routers = entry.node;
    }

    dv[entry.node][self] = dv[self][entry.node] = entry.cost;
  }

  num_neighbors = count;
  
  bf();
  update_neighbors();
}

void process_dv_update(uint8_t from, struct pkt *pkt)
{
  bool changed = false;

  while(has_next(pkt)) {
    struct pkt_entry entry;
    next(pkt, &entry);

    if(entry.node >= num_routers) {
      num_routers = entry.node;
      changed = true;
    }

    dv[from][entry.node] = entry.cost;
  }

  if(bf() || changed) {
    update_neighbors();
  }
}

static bool bf() 
{
  bool changed = false;
  
  for(int dst = 0; dst < num_routers; dst++) {
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
    dv[self][dst] = dv[dst][self] = best_cost;
    next_hop[dst] = best_hop;
  }

  return changed;
}

static void update_neighbors()
{
  for(int i = 0; i < num_neighbors; i++) {
    struct xbee_data data;
    data.buf = dv[self];
    data.len = num_routers;
    data.address = id_to_address(neighbors[i]);

    xbee_tx(&data);
  }
}

static void log_dv() 
{
  log_serial("\n");
  log_serial("    ");

  for(int i = 0; i < num_routers; i++) {
    log_serial(" | %2d", i);
  }
  log_serial("\n");
  log_serial("----");

  for(int i = 0; i < num_routers; i++) {
    log_serial("-----", i);
  }
  
  for(int i = 0; i < num_routers; i++) {
    log_serial("%2d ", i);
    for(int j = 0; j < num_routers; j++) {
      log_serial(" | %2d", dv[i][j]);
    }
    log_serial("\n");
  }
  log_serial("\n");
}

