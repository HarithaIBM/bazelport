# Bazel 7.2.0 z/OS Port — Stage 2 Build Patches

These 12 patches contain **all changes** required to build Bazel 7.2.0 on
z/OS with Bazel itself (Stage 2 bootstrap), taking the pristine upstream
Bazel 7.2.0 distribution source tree to a state where
`KEEP_TMP=1 bash compile.sh` produces a working `output/bazel` binary
(`bazel 7.2.0-2026-10-28`, nojdk variant, IBM Semeru J21 + IBM Open Enterprise
LLVM/clang in ASCII mode + zoslib 4.0.5).

## Applying

Apply in order, from the root of an unpacked Bazel 7.2.0 dist archive:

```bash
for p in stable-patches/*.patch; do git apply "$p"; done
```

(Also works with `patch -p1 < $p`.)

## Patch contents

| # | Patch | Area |
|---|-------|------|
| 1 | `0001-bootstrap-build-scripts.patch` | `compile.sh` (nojdk target, EBCDIC-safe binary lookup), `scripts/bootstrap/*` (z/OS Java detection, javac opts), `create_rules_java_builtin.sh` |
| 2 | `0002-workspace-config.patch` | `.bazelrc`(+`.local`), `BUILD`, `WORKSPACE`, `MODULE.bazel` — local repo wiring, pre-generated proto filegroup |
| 3 | `0003-platform-recognition.patch` | `src/conditions`, `OS.java` (ZOS enum), platform WORKSPACE templates |
| 4 | `0004-cpp-client.patch` | C++ client: `blaze.cc` (`__MVS__` locale/gRPC guards), **new `blaze_util_zos.cc`**, `file_posix.cc` (O_BINARY), md5 |
| 5 | `0005-unix-jni.patch` | `unix_jni.*` + **new `unix_jni_zos.cc`** |
| 6 | `0006-java-runtime.patch` | BlazeRuntime debug/module setup, FileWriteStrategy (chtag), JniLoader, OpenJ9 GC metrics, Skyframe/Workspace loading fixes |
| 7 | `0007-native-tools.patch` | process-wrapper, daemonize, process-tools, `package-bazel.sh`, `zip_builtins.sh` |
| 8 | `0008-singlejar-ijar.patch` | singlejar binary-mode `fopen("rb")` params fix (the key Stage 2 breakthrough), ijar z/OS fixes |
| 9 | `0009-grpc-java-plugin.patch` | grpc-java protoc plugin (protobuf 31 feature set) |
| 10 | `0010-cc-toolchain.patch` | `tools/cpp` configure/toolchain config, **new `zos_cc_toolchain_config.bzl`, `BUILD.zos`, `zos_toolchain/`** |
| 11 | `0011-python-jdk-toolchain.patch` | `tools/python` (interpreter paths), `tools/jdk` |
| 12 | `0012-proto-pregeneration.patch` | proto BUILD files (pre-generated sources), `pre_generated_protos/`, `derived/maven/BUILD` |

## Notes

- Run the bootstrap with `KEEP_TMP=1` so the build output tree (which
  contains the built binary) is not deleted by the temp-dir cleanup.
- The `nojdk` variant is used because the default `bazel-bin` target embeds a
  prebuilt Linux OpenJDK, which cannot be fetched/used on z/OS.
- Remaining known issue: bazel server/daemon mode fails in
  `blaze_util::MakeCanonical` (`realpath`, `EDC5121I`); use `--batch` mode.
