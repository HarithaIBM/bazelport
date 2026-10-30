# Bazel 7.2.0 z/OS Port — Build Patches (Stage 2 + user-workspace support)

These 12 patches contain **all changes** required to build Bazel 7.2.0 on
z/OS with Bazel itself (Stage 2 bootstrap) **and** to use the resulting
binary in user workspaces (genrules and `cc_binary`/`cc_library` builds with
the z/OS clang toolchain).

Verified end state:
- `KEEP_TMP=1 bash compile.sh` → `output/bazel` (`bazel 7.2.0-...-z/OS`, nojdk)
- In a user workspace: `bazel --batch build --noenable_bzlmod //:hello //:hello_cc`
  → rc=0; the C++ binary runs.
- Prerequisites: IBM Semeru J21 (`JAVA_HOME`), IBM Open Enterprise LLVM/clang
  (ASCII mode) + zoslib 4.0.5, zopen tools, the `local_repos/` directory of
  this repository, and the environment shown in `run_stage2.sh`
  (incl. `_TAG_REDIR_IN=txt=ISO8859-1`, `_TAG_REDIR_OUT=bin`).

## Applying

Apply in order, from the root of an unpacked Bazel 7.2.0 dist archive
(`bazel-7.2.0-dist.zip`):

```bash
for p in stable-patches/*.patch; do git apply "$p"; done
```

(Also works with `patch -p1 < $p`.)

## Patch contents

| # | Patch | Area |
|---|-------|------|
| 1 | `0001-bootstrap-build-scripts.patch` | `compile.sh` (nojdk target, EBCDIC-safe binary lookup, rules_java+platforms in bootstrap install base), `scripts/bootstrap/*`, `create_rules_java_builtin.sh` |
| 2 | `0002-workspace-config.patch` | `.bazelrc`(+`.local`), `BUILD`, `WORKSPACE`, `MODULE.bazel` — local repo wiring, pre-generated proto filegroup |
| 3 | `0003-platform-recognition.patch` | `src/conditions`, `OS.java` (ZOS), restored `rules_java_builtin.WORKSPACE` prefix + `local_config_platform.WORKSPACE` suffix, platform templates |
| 4 | `0004-cpp-client.patch` | C++ client: `blaze.cc` (argv[0]=java path for the J9 launcher, `__MVS__` locale/gRPC guards), **new `blaze_util_zos.cc`**, `file_posix.cc` (realpath buffer fix, O_BINARY) |
| 5 | `0005-unix-jni.patch` | `unix_jni.*` + **new `unix_jni_zos.cc`** |
| 6 | `0006-java-runtime.patch` | BlazeRuntime, FileWriteStrategy (chtag), JniLoader, OpenJ9 GC metrics, Skyframe/Workspace fixes, **BazelStrategyModule (no sandboxed default on z/OS)** |
| 7 | `0007-native-tools.patch` | **process-wrapper-options.cc: hand-rolled ASCII option parser** (libc getopt_long is EBCDIC), process-wrapper, daemonize, package-bazel.sh |
| 8 | `0008-singlejar-ijar.patch` | singlejar binary-mode `fopen("rb")` params fix, ijar z/OS fixes |
| 9 | `0009-grpc-java-plugin.patch` | grpc-java protoc plugin (protobuf 31 feature set) |
| 10 | `0010-cc-toolchain.patch` | `tools/cpp` configure (z/OS default target_libc, skip module map) & toolchain config, **new `zos_cc_toolchain_config.bzl`, `BUILD.zos`, `zos_toolchain/`** |
| 11 | `0011-python-jdk-toolchain.patch` | `tools/python` (interpreter paths), `tools/jdk` |
| 12 | `0012-proto-pregeneration.patch` | proto BUILD files (pre-generated sources), `pre_generated_protos/`, `derived/maven/BUILD` |

## Key z/OS-specific fixes (selected)

- **realpath()**: z/OS doesn't support the glibc `realpath(path, NULL)`
  extension (EINVAL) — `MakeCanonical` passes a `PATH_MAX` buffer.
- **Server JVM argv[0]**: the IBM J9 launcher derives JAVA_HOME from argv[0];
  bazel's `bazel(workspace)` argv[0] kills it (`libjli.so` CEE3501S) — the
  OpenBSD workaround is applied for `__MVS__` too.
- **process-wrapper options**: z/OS libc `getopt_long` compares EBCDIC
  characters while ASCII-mode binaries pass ASCII argv — options are parsed
  by hand (`--`, `--name=value`, short opts).
- **rules_java_builtin / local_config_platform WORKSPACE prefix/suffix**:
  restored so user workspaces resolve `@rules_java_builtin`,
  `@internal_platforms_do_not_use`, `@platforms`, `@local_config_platform`,
  `@host_platform` (no `load()` in the prefix — it causes circular
  `//external` resolution).
- **No sandboxed execution by default** (processwrapper-sandbox stats.out
  I/O fails); `--spawn_strategy=standalone`-equivalent local execution.
- **No `-Wl,-S`** on z/OS (the binder reads it as a SYSLIB DD override);
  `target_libc` defaults to `zos` on z/OS systems.
- Module map generation skipped on z/OS (script is read as EBCDIC by
  `/bin/sh`; clang modules unused anyway).

## Known remaining issue

**Client-server (daemon) mode**: the C++ client crashes (0C4/0C9) inside
zopen's gRPC 1.73 static library when creating the channel to the server
(`grpc_completion_queue_create_internal`). Batch mode (`--batch`) is fully
functional and is the recommended mode until this is resolved.
