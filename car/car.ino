#include <stdbool.h>
#include <logger.h>
#include <rx_tx.h>
#include <constants.h>
#include "motor.h"
#include "comm.h"

#define XBEE_READ_CYCLE 50U
#define LOG_CYCLE 5000U

void setup() 
{
  Serial.begin(9600);
  log_init(LOG_SOFT);
  
  motor_init();

  log_serial(F("Booting... "));
  int node_address = comm_init();
  log_serial(F("Done. Node ID is 0x%X\n"), node_address);
  
  motor_calibrate();
}

void loop() 
{ 
  log_serial(F("Waiting for dst\n"));
  // 1. wait for dst
  struct comm_stx data;
  comm_recv_stx(&data);
  log_serial(F("Got dst: src = %d nxt = %d dst = %d\n"), data.src_node, data.next_node, data.dst_node);
  
  int prev = data.src_node;
  int next = data.next_node;
  
  while(motor_go() != MOTOR_END) {
    unsigned long start = millis();
    
    // we reached a intersection
    struct comm_req req;
    req.dst_node = data.dst_node;
    req.prev_node = prev;
    
    comm_send_req(next, &req);
    
    // wait for response
    struct comm_res res;
    comm_recv_res(&res);

    unsigned long diff = millis() - start;

    if(diff < 100) {
      delay(100 - diff);
    }
    
    prev = next;
    next = res.next_node;
  
    switch(res.next_dir) {
      case LEFT: {
        motor_left();
        break;
      }
      case RIGHT: {
        motor_right();
        break;
      }
    }
  }
}

