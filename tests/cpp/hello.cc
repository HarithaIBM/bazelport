#include "cpp/greeting.h"
#include <cstdio>
int main() {
  std::printf("%s\n", Greet("world").c_str());
  return 0;
}
