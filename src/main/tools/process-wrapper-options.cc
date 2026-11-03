// Copyright 2017 The Bazel Authors. All rights reserved.
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

#include "src/main/tools/process-wrapper-options.h"

#ifdef __MVS__
// z/OS <getopt.h> is EBCDIC-tagged with trigraph directives that clang cannot
// process; getopt_long itself exists in the libc. Declare it directly.
#define no_argument 0
#define required_argument 1
#define optional_argument 2
struct option {
  const char* name;
  int has_arg;
  int* flag;
  int val;
};
extern "C" int getopt_long(int argc, char* const argv[], const char* optstring,
                           const struct option* longopts, int* longindex);
#else
#include <getopt.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "src/main/tools/logging.h"

struct Options opt;

// Print out a usage error. argc and argv are the argument counter and vector,
// fmt is a format, string for the error message to print.
static void Usage(char *program_name, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  fprintf(stderr, "\nUsage: %s -- command arg1 @args\n", program_name);
  fprintf(
      stderr,
      "\nPossible arguments:\n"
      "  -g/--graceful_sigterm  propagate SIGTERM to the subprocess and delay "
      "the corresponding SIGKILL until --kill_delay has passed\n"
      "  -t/--timeout <timeout>  timeout after which the child process will be "
      "terminated with SIGTERM\n"
      "  -k/--kill_delay <timeout>  in case timeout occurs, how long to wait "
      "before killing the child with SIGKILL\n"
      "  -o/--stdout <file>  redirect stdout to a file\n"
      "  -e/--stderr <file>  redirect stderr to a file\n"
      "  -s/--stats <file>  if set, write stats in protobuf format to a file\n"
      "  -d/--debug  if set, debug info will be printed\n"
      "  --  command to run inside sandbox, followed by arguments\n");
  exit(EXIT_FAILURE);
}

// Parses command line flags from an argv array and puts the results into the
// global `opt` struct.
#ifdef __MVS__
// z/OS: the libc getopt_long() compares option characters as EBCDIC, while
// this program is compiled with -fzos-le-char-mode=ascii and receives ASCII
// argv strings -- so getopt_long never matches any option and silently
// treats flags like "--" as the command (execvp("--") fails). Parse the
// options by hand instead.
static void ParseCommandLine(const std::vector<char *> &args) {
  size_t i = 1;  // skip argv[0]
  while (i < args.size()) {
    char *arg = args[i];
    if (strcmp(arg, "--") == 0) {
      i++;  // explicit command separator
      break;
    }
    if (arg[0] != '-' || arg[1] == '\0') {
      break;  // first non-option ("+" semantics): rest is the command
    }
    const char *name = arg + 1;
    if (name[0] == '-') {
      name++;
    }
    // Long options may be given as --name=value.
    const char *eq = strchr(name, '=');
    size_t nlen = (eq != nullptr) ? (size_t)(eq - name) : strlen(name);
    const char *value = (eq != nullptr) ? eq + 1 : nullptr;

    int c;
    if ((nlen == 1 && name[0] == 'g') ||
        (nlen == 16 && strncmp(name, "graceful_sigterm", nlen) == 0)) {
      c = 'g';
    } else if ((nlen == 1 && name[0] == 't') ||
               (nlen == 7 && strncmp(name, "timeout", nlen) == 0)) {
      c = 't';
    } else if ((nlen == 1 && name[0] == 'k') ||
               (nlen == 10 && strncmp(name, "kill_delay", nlen) == 0)) {
      c = 'k';
    } else if ((nlen == 1 && name[0] == 'o') ||
               (nlen == 6 && strncmp(name, "stdout", nlen) == 0)) {
      c = 'o';
    } else if ((nlen == 1 && name[0] == 'e') ||
               (nlen == 6 && strncmp(name, "stderr", nlen) == 0)) {
      c = 'e';
    } else if ((nlen == 1 && name[0] == 's') ||
               (nlen == 5 && strncmp(name, "stats", nlen) == 0)) {
      c = 's';
    } else if ((nlen == 1 && name[0] == 'd') ||
               (nlen == 5 && strncmp(name, "debug", nlen) == 0)) {
      c = 'd';
    } else {
      Usage(args.front(), "Unrecognized argument: %s", arg);
      return;  // Usage exits
    }

    if (value == nullptr &&
        (c == 't' || c == 'k' || c == 'o' || c == 'e' || c == 's')) {
      if (i + 1 >= args.size()) {
        Usage(args.front(), "Flag requires an argument: %s", arg);
        return;
      }
      value = args[++i];
    }

    switch (c) {
      case 'g':
        opt.graceful_sigterm = true;
        break;
      case 't':
        if (sscanf(value, "%lf", &opt.timeout_secs) != 1) {
          Usage(args.front(), "Invalid timeout (-t) value: %s", value);
        }
        break;
      case 'k':
        if (sscanf(value, "%lf", &opt.kill_delay_secs) != 1) {
          Usage(args.front(), "Invalid kill delay (-k) value: %s", value);
        }
        break;
      case 'o':
        if (opt.stdout_path.empty()) {
          opt.stdout_path.assign(value);
        } else {
          Usage(args.front(),
                "Cannot redirect stdout (-o) to more than one destination.");
        }
        break;
      case 'e':
        if (opt.stderr_path.empty()) {
          opt.stderr_path.assign(value);
        } else {
          Usage(args.front(),
                "Cannot redirect stderr (-e) to more than one destination.");
        }
        break;
      case 's':
        if (opt.stats_path.empty()) {
          opt.stats_path.assign(value);
        } else {
          Usage(args.front(),
                "Cannot write stats (-s) to more than one destination.");
        }
        break;
      case 'd':
        opt.debug = true;
        break;
    }
    i++;
  }
  if (i < args.size()) {
    opt.args.assign(args.begin() + i, args.end());
  }
}
#else
static void ParseCommandLine(const std::vector<char *> &args) {
  static struct option long_options[] = {
      {"graceful_sigterm", no_argument, 0, 'g'},
      {"timeout", required_argument, 0, 't'},
      {"kill_delay", required_argument, 0, 'k'},
      {"stdout", required_argument, 0, 'o'},
      {"stderr", required_argument, 0, 'e'},
      {"stats", required_argument, 0, 's'},
      {"debug", no_argument, 0, 'd'},
      {0, 0, 0, 0}};
  extern char *optarg;
  extern int optind, optopt;
  int c;

  while ((c = getopt_long(args.size(), args.data(), "+:gt:k:o:e:s:d",
                          long_options, nullptr)) != -1) {
    switch (c) {
      case 'g':
        opt.graceful_sigterm = true;
        break;
      case 't':
        if (sscanf(optarg, "%lf", &opt.timeout_secs) != 1) {
          Usage(args.front(), "Invalid timeout (-t) value: %s", optarg);
        }
        break;
      case 'k':
        if (sscanf(optarg, "%lf", &opt.kill_delay_secs) != 1) {
          Usage(args.front(), "Invalid kill delay (-k) value: %s", optarg);
        }
        break;
      case 'o':
        if (opt.stdout_path.empty()) {
          opt.stdout_path.assign(optarg);
        } else {
          Usage(args.front(),
                "Cannot redirect stdout (-o) to more than one destination.");
        }
        break;
      case 'e':
        if (opt.stderr_path.empty()) {
          opt.stderr_path.assign(optarg);
        } else {
          Usage(args.front(),
                "Cannot redirect stderr (-e) to more than one destination.");
        }
        break;
      case 's':
        if (opt.stats_path.empty()) {
          opt.stats_path.assign(optarg);
        } else {
          Usage(args.front(),
                "Cannot write stats (-s) to more than one destination.");
        }
        break;
      case 'd':
        opt.debug = true;
        break;
      case '?':
        Usage(args.front(), "Unrecognized argument: -%c (%d)", optopt, optind);
        break;
      case ':':
        Usage(args.front(), "Flag -%c requires an argument", optopt);
        break;
    }
  }

  if (optind < static_cast<int>(args.size())) {
    opt.args.assign(args.begin() + optind, args.end());
  }
}
#endif  // __MVS__

void ParseOptions(int argc, char *argv[]) {
  std::vector<char *> args(argv, argv + argc);

  ParseCommandLine(args);

  if (opt.args.empty()) {
    Usage(args.front(), "No command specified.");
  }

  // argv[] passed to execve() must be a null-terminated array.
  opt.args.push_back(nullptr);
}
