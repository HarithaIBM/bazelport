// Copyright 2026 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// z/OS (UNIX System Services) platform implementation of the blaze_util
// interface. Modeled on blaze_util_bsd.cc; no /proc filesystem statistics,
// no statfs magic numbers, no ioprio.

#if defined(__MVS__)

#define STANDARD_JAVABASE "/home/haritha/dhjava/J21.0_64"

#if !defined(DEFAULT_SYSTEM_JAVABASE)
# define DEFAULT_SYSTEM_JAVABASE STANDARD_JAVABASE
#endif

#include <errno.h>  // errno
#include <limits.h>  // PATH_MAX
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>  // strerror
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <memory>
#include <string>

#include "src/main/cpp/blaze_util.h"
#include "src/main/cpp/blaze_util_platform.h"
#include "src/main/cpp/startup_options.h"
#include "src/main/cpp/util/errors.h"
#include "src/main/cpp/util/exit_code.h"
#include "src/main/cpp/util/logging.h"
#include "src/main/cpp/util/path.h"
#include "src/main/cpp/util/port.h"
#include "src/main/cpp/util/strings.h"

namespace blaze {

using blaze_util::GetLastErrorString;
using blaze_util::Path;
using std::string;

// ${XDG_CACHE_HOME}/bazel, a.k.a. ~/.cache/bazel by default (which is the
// fallback when XDG_CACHE_HOME is not set)
string GetOutputRoot() {
  string xdg_cache_home = GetPathEnv("XDG_CACHE_HOME");
  if (xdg_cache_home.empty()) {
    char buf[2048];
    struct passwd pwbuf;
    struct passwd *pw = nullptr;
    int uid = getuid();
    int r = getpwuid_r(uid, &pwbuf, buf, 2048, &pw);
    if (r == 0 && pw != nullptr) {
      xdg_cache_home = blaze_util::JoinPath(pw->pw_dir, ".cache");
    } else {
      return "/tmp";
    }
  }

  return blaze_util::JoinPath(xdg_cache_home, "bazel");
}

// z/OS does not expose filesystem type magic numbers.
void WarnFilesystemType(const blaze_util::Path &output_base) {}

string GetSelfPath(const char* argv0) {
  if (argv0 == nullptr) {
    return "";
  }
  string selfpath = argv0;
  if (!selfpath.empty() && selfpath[0] != '/') {
    // Resolve a relative argv0 against the current working directory.
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof cwd) != nullptr) {
      selfpath = blaze_util::JoinPath(cwd, selfpath);
    }
  }
  return selfpath;
}

uint64_t GetMillisecondsMonotonic() {
  struct timespec ts = {};
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

// z/OS does not support ioprio or batch CPU scheduling hints.
void SetScheduling(bool batch_cpu_scheduling, int io_nice_level) {}

std::unique_ptr<Path> GetProcessCWD(int pid) {
  // z/OS does not expose per-process working directories.
  return nullptr;
}

bool IsSharedLibrary(const string &filename) {
  return blaze_util::ends_with(filename, ".so");
}

string GetSystemJavabase() {
  // If JAVA_HOME is defined, then use it as default.
  string javahome = GetPathEnv("JAVA_HOME");

  if (!javahome.empty()) {
    string javac = blaze_util::JoinPath(javahome, "bin/javac");
    if (access(javac.c_str(), X_OK) == 0) {
      return javahome;
    }
    BAZEL_LOG(WARNING)
        << "Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.";
  }

  return DEFAULT_SYSTEM_JAVABASE;
}

void WriteSystemSpecificProcessIdentifier(const blaze_util::Path &server_dir,
                                          pid_t server_pid) {}

bool VerifyServerProcess(int pid, const blaze_util::Path &output_base) {
  // TODO(lberki): This only checks for the process's existence, not whether
  // its start time matches. Therefore this might accidentally kill an
  // unrelated process if the server died and the PID got reused.
  return kill(pid, 0) == 0;
}

// Not supported on z/OS.
void ExcludePathFromBackup(const blaze_util::Path &path) {}

int32_t GetExplicitSystemLimit(const int resource) {
  return -1;
}

}  // namespace blaze

#endif  // __MVS__
