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

// z/OS (UNIX System Services) implementation of the portable JNI interface
// declared in unix_jni.h. Modeled on unix_jni_linux.cc; no fstatat, no xattrs,
// no power/thermal monitoring on z/OS.

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

#include <string>

#include "src/main/native/unix_jni.h"

namespace blaze_jni {

std::string ErrorMessage(int error_number) {
  char buf[4096];
  // z/OS libc provides the SUSv3-conformant strerror_r.
  if (strerror_r(error_number, buf, sizeof buf) == -1) {
    return std::string("");
  } else {
    return std::string(buf);
  }
}

// z/OS does not provide fstatat(2). Fall back to stat()/lstat() when the
// path is absolute or relative to the current working directory; report
// ENOSYS otherwise.
int portable_fstatat(
    int dirfd, char *name, portable_stat_struct *statbuf, int flags) {
  if (name != nullptr && name[0] == '/') {
    if (flags & AT_SYMLINK_NOFOLLOW) {
      return lstat(name, statbuf);
    }
    return stat(name, statbuf);
  }
  if (dirfd == AT_FDCWD) {
    if (flags & AT_SYMLINK_NOFOLLOW) {
      return lstat(name, statbuf);
    }
    return stat(name, statbuf);
  }
  errno = ENOSYS;
  return -1;
}

int StatSeconds(const portable_stat_struct &statbuf, StatTimes t) {
  switch (t) {
    case STAT_ATIME:
      return statbuf.st_atime;
    case STAT_CTIME:
      return statbuf.st_ctime;
    case STAT_MTIME:
      return statbuf.st_mtime;
  }
  return 0;
}

// z/OS struct stat does not carry nanosecond resolution.
int StatNanoSeconds(const portable_stat_struct &statbuf, StatTimes t) {
  return 0;
}

// z/OS does not support extended attributes.
ssize_t portable_getxattr(const char *path, const char *name, void *value,
                          size_t size, bool *attr_not_found) {
  *attr_not_found = true;
  errno = ENOSYS;
  return -1;
}

ssize_t portable_lgetxattr(const char *path, const char *name, void *value,
                           size_t size, bool *attr_not_found) {
  *attr_not_found = true;
  errno = ENOSYS;
  return -1;
}

int portable_push_disable_sleep() {
  // Currently not supported.
  return -1;
}

int portable_pop_disable_sleep() {
  // Currently not supported.
  return -1;
}

void portable_start_suspend_monitoring() {
  // Currently not implemented.
}

void portable_start_thermal_monitoring() {
  // Currently not implemented.
}

int portable_thermal_load() {
  // Currently not implemented.
  return 0;
}

void portable_start_system_load_advisory_monitoring() {
  // Currently not implemented.
}

int portable_system_load_advisory() {
  // Currently not implemented.
  return 0;
}

void portable_start_memory_pressure_monitoring() {
  // Currently not implemented.
}

MemoryPressureLevel portable_memory_pressure() {
  // Currently not implemented.
  return MemoryPressureLevelNormal;
}

void portable_start_disk_space_monitoring() {
  // Currently not implemented.
}

void portable_start_cpu_speed_monitoring() {
  // Currently not implemented.
}

int portable_cpu_speed() {
  // Currently not implemented.
  return -1;
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_devtools_build_lib_profiler_SystemNetworkStats_getNetIoCountersNative(
    JNIEnv *env, jclass clazz, jobject counters_list) {
  // Currently not implemented.
}

}  // namespace blaze_jni
