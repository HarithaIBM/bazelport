#include "cpp/greeting.h"
#include <string>
std::string Greet(const std::string& name) { return "Hello, " + name + ", from Bazel on z/OS!"; }
