#define _ALL_SOURCE 1
#include <string.h>
#include <stdlib.h>
#include <grpc/grpc.h>
#include <stdio.h>
int main(int argc, char** argv) {
  int ver = argc > 1 ? atoi(argv[1]) : 2;
  grpc_init();
  grpc_channel* ch = grpc_channel_create("localhost:12345", NULL, NULL);
  printf("ch=%p\n", (void*)ch); fflush(stdout);
  grpc_completion_queue_attributes attrs;
  memset(&attrs, 0, sizeof(attrs));
  attrs.version = ver;
  attrs.cq_completion_type = GRPC_CQ_NEXT;
  attrs.cq_polling_type = GRPC_CQ_DEFAULT_POLLING;
  grpc_completion_queue* cq = grpc_completion_queue_create(NULL, &attrs, NULL);
  printf("cq=%p (ver=%d)\n", (void*)cq, ver); fflush(stdout);
  printf("OK\n");
  return 0;
}
