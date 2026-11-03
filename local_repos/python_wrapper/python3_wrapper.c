/* z/OS python3 wrapper (program object).
 *
 * The IBM Open Enterprise SDK for Python interpreter is a z/OS program
 * object that needs libpython3.14.so resolvable via LIBPATH. Bazel action
 * spawns do not inherit the server's LIBPATH, and shell-script wrappers
 * hit EBCDIC/ASCII tagging issues in the action environment. A compiled
 * wrapper sidesteps both problems: set LIBPATH from PYTHON_HOME (default
 * below) and exec the real interpreter.
 */
#include <stdlib.h>
#define _XOPEN_SOURCE_EXTENDED 1
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <spawn.h>

extern int setenv(const char *, const char *, int);
int main(int argc, char **argv) {
  const char *home = getenv("PYTHON_HOME");
  if (home == NULL || home[0] == '\0') {
    home = "/home/haritha/bazelDepTools/python";
  }
  char libpath[4096];
  const char *old = getenv("LIBPATH");
  if (old != NULL && old[0] != '\0') {
    snprintf(libpath, sizeof(libpath), "%s/lib:%s", home, old);
  } else {
    snprintf(libpath, sizeof(libpath), "%s/lib", home);
  }
  setenv("LIBPATH", libpath, 1);

  char interp[4096];
  snprintf(interp, sizeof(interp), "%s/bin/python3", home);

  argv[0] = interp;
  execv(interp, argv);
  perror("python3_wrapper: execv failed");
  return 127;
}
