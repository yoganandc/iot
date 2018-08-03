#include <stdbool.h>
#include <logger.h>
#include <rx_tx.h>
#include "motor.h"
#include "comm.h"

#define XBEE_READ_CYCLE 50U
#define LOG_CYCLE 5000U

void setup() 
{
  Serial.begin(57600);
  log_init(LOG_UART);
  
  motor_init();
  delay(5000);

  log_serial(F("Begin"));
  motor_calibrate();
  log_serial("Done");
  
  log_serial(F("Booting... "));
  int node_address = comm_init();
  log_serial(F("Done. Node ID is 0x%X\n"), node_address);
}

void loop() 
{
  do {
    // 1. wait for dst
    struct comm_stx data;
    comm_recv_stx(&data);

    int prev = data.src_node;
    int next = data.next_node;

    while(motor_go() != MOTOR_END) {
      // we reached a intersection
      struct comm_req req;
      req.dst_node = data.dst_node;
      req.prev_node = prev;

      comm_send_req(next, &req);

      // wait for response
      struct comm_res res;
      comm_recv_res(&res);

      prev = next;
      next = res.next_node;

      
    }
    
  } while(1);
}

