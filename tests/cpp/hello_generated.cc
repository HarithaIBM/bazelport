#include <cstdio>
const char* BuildStamp();
int main() {
  std::printf("built with a generated source: %s\n", BuildStamp());
  return 0;
}
