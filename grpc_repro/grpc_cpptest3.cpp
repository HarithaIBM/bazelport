#include <grpcpp/grpcpp.h>
#include <cstdio>
#include <unistd.h>
int main() {
  grpc_init();
  printf("step1: cq\n"); fflush(stdout);
  grpc::CompletionQueue cq;
  printf("step2: channel\n"); fflush(stdout);
  auto ch = grpc::CreateChannel("localhost:12345", grpc::InsecureChannelCredentials());
  printf("step3: sleeping 5s (background threads run)\n"); fflush(stdout);
  sleep(5);
  printf("step4: cq2\n"); fflush(stdout);
  grpc::CompletionQueue cq2;
  printf("step5: all OK\n"); fflush(stdout);
  return 0;
}
