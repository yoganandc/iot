#include <stdbool.h>
#include <stdint.h>
#include "router.h"
#include "pkt.h"
#include "rx_tx.h"
#include "logger.h"
#include "router_util.h"
#include "lcd.h"

static const uint8_t INF = 255;

static uint8_t self;
static uint8_t buf[MAX_ROUTERS*2 + 1];

static uint8_t next_hop[MAX_ROUTERS];
static uint8_t dv[MAX_ROUTERS][MAX_ROUTERS];
static uint8_t neighbors[MAX_NEIGHBORS]; 
static uint8_t neighbor_dirs[MAX_NEIGHBORS];

static uint8_t num_neighbors;
static uint8_t num_routers;

static bool bf();
static void update_neighbors();

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
    
    neighbors[count] = entry.node;
    neighbor_dirs[count++] = entry.dir;

    if(entry.node >= num_routers) {
      num_routers = entry.node + 1;
    }

    dv[self][entry.node] = dv[entry.node][self] = entry.cost;
  }

  num_neighbors = count;

  bf();
  update_neighbors();
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

    if(data.address) {
      xbee_tx(&data);
    }
  }
}

void router_log() 
{
  static char *dirs[] = {"NORTH", "EAST", "SOUTH", "WEST"};

  if(num_routers < 1) {
    return;
  }
  
  log_serial("\nNeighbors\n");
  for(int i = 0; i < num_neighbors; i++) {
    log_serial("id: %2d, cost: %2d, dir: %s\n", neighbors[i], dv[self][neighbors[i]], dirs[neighbor_dirs[i]]);
  }

  log_serial("\nNext Hop\n");
  log_serial("%d: %d", 0, next_hop[0]);
  for(int i = 1; i < num_routers; i++) {
    log_serial(", %d: %d", i, next_hop[i]);
  }
  log_serial("\n");
  
  log_serial("\nRouting Table\n");
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

void router_lcd()
{
  lcd_tabulate(dv[self], next_hop, num_routers, self);
}

