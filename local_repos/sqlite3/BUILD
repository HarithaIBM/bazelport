# sqlite3 — z/OS system install (zopen), exposed as a Bazel repo.
# Links the zopen-built static library instead of compiling the amalgamation.
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sqlite3",
    hdrs = glob(["include/*.h"]),
    includes = ["include"],
    linkopts = [
        "/home/haritha/zopen_23may/zopen/usr/local/zopen/sqlite/sqlite/lib/libsqlite3.a",
        "-lpthread",
        "-ldl",
    ],
)
