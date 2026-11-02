#define _ALL_SOURCE 1
#include <grpc/grpc.h>
#include <stdio.h>
int main() {
  grpc_init();
  grpc_completion_queue* cq1 = grpc_completion_queue_create_for_next(NULL);
  printf("cq1=%p\n", (void*)cq1); fflush(stdout);
  grpc_channel* ch = grpc_channel_create("localhost:12345", NULL, NULL);
  printf("ch=%p\n", (void*)ch); fflush(stdout);
  grpc_completion_queue* cq2 = grpc_completion_queue_create_for_next(NULL);
  printf("cq2=%p\n", (void*)cq2); fflush(stdout);
  grpc_completion_queue* cq3 = grpc_completion_queue_create_for_callback(NULL, NULL);
  printf("cq3(callback)=%p\n", (void*)cq3); fflush(stdout);
  printf("ALL OK\n");
  return 0;
}
