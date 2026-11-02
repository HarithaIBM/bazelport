#include <grpcpp/grpcpp.h>
#include <cstdio>
int main() {
  grpc_init();
  printf("channel first...\n"); fflush(stdout);
  auto ch = grpc::CreateChannel("localhost:12345", grpc::InsecureChannelCredentials());
  printf("channel OK; now cq\n"); fflush(stdout);
  grpc::CompletionQueue cq;
  printf("cq1 OK after channel\n"); fflush(stdout);
  return 0;
}
