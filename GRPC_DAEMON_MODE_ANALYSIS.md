# gRPC daemon-mode investigation: RESOLVED ✅

Status: **FIXED** — daemon (client-server) mode now works end-to-end on z/OS.
Verified: `bazel version`, `bazel info`, `bazel shutdown` all rc=0 without
`--batch`, after rebuilding the Bazel client with `-fno-short-enums`.

## Root cause

**`-fno-short-enums` ABI mismatch between the zopen-built gRPC libraries and
the Bazel client code.**

z/OS clang defaults to *short* enums (`-fshort-enums`: enums occupy the
smallest fitting integer, often 1 byte). The zopen build flags for gRPC (and
its bundled protobuf/absl) include `-fno-short-enums`, so those libraries use
4-byte enums. Struct layouts differ between the two ABI variants, and the
mismatch is silent: fields are read at wrong offsets, producing garbage.

Concrete failure chain (diagnosed with instrumented gRPC sources):

1. grpcpp's `CompletionQueue` ctor builds a stack temporary
   `grpc_completion_queue_attributes{version, GRPC_CQ_NEXT, ...}` in code
   compiled **with** short enums → `cq_completion_type` written at a different
   offset than the library expects.
2. `grpc_completion_queue_create_internal` reads `cq_completion_type` from the
   library's (4-byte enum) view → reads stack garbage (0xE72x) instead of 0.
3. `g_cq_vtable[garbage]` → wildly out-of-bounds vtable pointer →
   `vtable->init(...)` branches to NULL/garbage →
   **0C4/0C1 protection/operation exception**.

This explains every observed symptom:
- "cq before channel works, cq after channel crashes": the garbage read
  depends on leftover stack contents; channel creation leaves 0xE72x bytes
  on that stack slot.
- The `__udivmodti4` divide exceptions (0C9) from absl time code: Timestamp /
  Duration values garbled by the same struct-layout mismatch.
- The nondeterminism: uninitizalized-stack-dependent.

## Fixes applied

1. **`tools/cpp/unix_cc_toolchain_config.bzl`** (patch 0010): the z/OS cc
   toolchain now passes `-fno-short-enums` for all C++ compiles, matching the
   zopen library ABI. This is the fix that makes daemon mode work.
2. `src/main/cpp/main.cc` (patch 0004): explicit `grpc_init()` under
   `__MVS__` before any gRPC use (lazy init is unreliable on z/OS).
3. `local_repos/com_github_grpc_grpc/BUILD`: removed duplicate gRPC core
   (`libgrpc.a` + `libgrpc_unsecure.a` both define the full core).

## Rule for z/OS C++ consumers

**Any** code compiled against zopen-built C++ libraries (gRPC, protobuf, absl,
...) must use the zopen compiler flag set, at minimum:

```
-fzos-le-char-mode=ascii -fno-short-enums -m64 -mzos-target=zosv2r5 -march=z13
```

- `-fzos-le-char-mode=ascii` selects the `std::__1_a` libc++ ABI namespace.
- `-fno-short-enums` selects 4-byte enums.
- Omitting either produces silently-corrupted structs.

## Reproducers (`grpc_repro/`)

Kept for regression testing. Build both ways to see failure/success:

```sh
G=/home/haritha/zopen_23may/zopen/usr/local
# FAILS (0C4 after channel creation):
clang++ -std=c++17 -fzos-le-char-mode=ascii grpc_cpptest5.cpp ...
# WORKS:
clang++ -std=c++17 -fzos-le-char-mode=ascii -fno-short-enums grpc_cpptest5.cpp ...
```

`grpc_ctest2.c` (C API with explicit grpc_init) works either way — the C API
passes enums by value in registers, not in structs.

## grpcport build (zopencommunity/grpcport)

While rebuilding gRPC from source to debug this, the port needed these fixes
(all recorded in `/home/haritha/code/bazel-7.2.0/grpcport/patches/`):

- `buildenv`: `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` (CMake 4.x vs bundled
  cares' old `cmake_minimum_required`), clang pinned via `COMPILER_PATH`.
- `patches/src.patch`: OpenSSL const-correctness fixes
  (`grpc_tls_crl_provider.cc`, `ssl_transport_security_utils.cc`) — zopen's
  OpenSSL headers return `const X509_NAME*` / make `ASN1_STRING` opaque;
  replaced direct member access with `ASN1_STRING_length()` /
  `ASN1_STRING_get0_data()`.
- `patches/third_party/abseil-cpp.patch`: `crc32c.h` operator<< fix
  (absl::StreamFormat doesn't compile with IBM Open XL C++).
- `patches/toolchain/libcpp-poll_with_backoff-zos.patch`: z/OS `<poll.h>`
  uses `__poll` as include guard, colliding with a libc++ parameter name;
  renamed in the local OELCPP headers (original saved as `.zos-original`).
