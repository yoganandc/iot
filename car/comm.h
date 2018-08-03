#ifndef COMM_H
#define COMM_H

#ifdef __cplusplus
extern "C"
{
#endif

struct comm_stx {
  int dst_node;
  int src_node;
  int next_node;
};

struct comm_req {
  int dst_node;
  int prev_node;
};

struct comm_res {
  int next_node;
};

int comm_init();
void comm_send_req(int, struct comm_req *);
void comm_recv_res(struct comm_res *);
void comm_recv_stx(struct comm_stx *);

#ifdef __cplusplus
}
#endif

#endif
