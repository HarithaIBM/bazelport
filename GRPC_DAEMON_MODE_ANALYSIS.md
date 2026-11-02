# gRPC daemon-mode investigation: leveraging zopen gRPC on z/OS

Status: **diagnosed — blocked by a bug in the zopen gRPC port itself**
(zopencommunity/grpcport, gRPC 1.73.0, release 20250807_221145).
Batch mode (`bazel --batch`) remains fully functional.

## Background

The Bazel C++ client (`blaze`) talks to the Bazel server over gRPC
(localhost, insecure channel, grpcpp sync stubs). The port already links
zopen's static gRPC libraries via `local_repos/com_github_grpc_grpc`
(there is no working network fetch, and building gRPC from source was not
attempted).

Running `output/bazel version` (daemon mode) crashed with:

```
CEE3204S System Completion Code=0C4 (protection exception)
From entry point grpc_completion_queue_create_internal ... offset +0003BC
```

## What was verified

### 1. zopen gRPC C core basically works (with explicit grpc_init)

`grpc_repro/grpc_ctest2.c`: `grpc_init()` + C API
(`grpc_completion_queue_create_for_next`, `grpc_channel_create`,
`grpc_completion_queue_create_for_callback`) — all succeed, 3/3 runs.

**Without** an explicit `grpc_init()` first, the same program can crash
inside `grpc_pollset_size` (0C4). Lazy core initialization is unreliable
on z/OS.

=> Fix applied: `src/main/cpp/main.cc` now calls `grpc_init()` under
`__MVS__` before any gRPC use.

### 2. The C++ layer (grpcpp) breaks after a channel exists

`grpc_repro/grpc_cpptest3.cpp` / `grpc_cpptest5.cpp`, linked against the
zopen static libs exactly like the Bazel client:

- `CompletionQueue` created **before** any channel: works.
- `grpc::CreateChannel(...)`: works.
- Any `grpc::CompletionQueue` created **after** a channel exists:
  deterministic crash — **indirect branch to address 0** inside
  `grpc_completion_queue_create_internal` ("Possible Bad Branch", CEE3201S
  0C1, or 0C4 at entry offset +0x3BC in the Bazel binary).

This is precisely Bazel's client flow: `Connect()` creates the channel,
then the sync `Ping` unary call constructs a temporary callback
`CompletionQueue` → crash.

The grpcpp default ctor creates its CQ via
`grpc_completion_queue_create(NULL, attributes{version=2, GRPC_CQ_NEXT,
GRPC_CQ_DEFAULT_POLLING}, NULL)` — the attributes-based path with
DEFAULT_POLLING. That path reads a polling-engine vtable that is only
resolved after full core init (triggered by channel creation) and ends up
NULL on z/OS.

### 3. Background event-engine threads also crash

Several runs died with 0C9 (fixed-point divide exception) in
`__udivmodti4` reached from absl `KernelTime`/`IDivDuration` on a
`WorkStealingThreadPool` thread. Plain 128-bit division works fine in
isolation (`udivtest2.cpp`), so the arguments are garbage — consistent
with corrupted core state rather than a compiler-rt bug.

### 4. Duplicate gRPC core in the binary (fixed)

The port's `local_repos/com_github_grpc_grpc/BUILD` used to link BOTH
`libgrpc_unsecure.a` and `libgrpc.a`. Both archives define the whole gRPC
core (`grpc_completion_queue_create_internal`, poller vtables, plugin
registries), so the binary could end up with two copies of the core's
global state. `libgrpc.a` has been removed from the linkopts (Bazel only
needs insecure channels). Verified: no `grpc_ssl_credentials_create`
symbols in the rebuilt binary. (The crash in (2) persists, so this was a
real but not sufficient fix.)

### 5. zopen's own gRPC port tests were skipped

```
$ cat .../zopen/grpc/grpc/test.status
Skipped (tests skipped)
```

The zopencommunity gRPC port never validated its runtime on z/OS. The
crashes above reproduce with *only* zopen-built libraries, so this is an
upstream zopen port bug, not a Bazel-port issue.

### 6. Compile-time fix needed for zopen absl header

`absl/crc/crc32c.h` uses `absl::StreamFormat`, which fails to compile
with IBM Open XL C++ ("FormatSpecTemplate<void> is unavailable"). We
patched the installed header in place (same fix as
`patches/10-fix-absl-crc32c-zos.patch`). This should be upstreamed to the
zopen absl/grpc ports.

## Reproducing

See the programs in `grpc_repro/`. Build (ASCII mode, zopen static libs):

```sh
G=/home/haritha/zopen_23may/zopen/usr/local
clang++ -std=c++17 -fzos-le-char-mode=ascii -c grpc_cpptest5.cpp -o t.o \
    -I$G/include -I$G/include/grpc
ABSL=""; for a in $G/lib/libabsl_*.a; do absl="${a##*/lib}"; absl="${absl%.a}"; ABSL="$ABSL -l$absl"; done
clang++ -std=c++17 -fzos-le-char-mode=ascii t.o -o t -L$G/lib \
    -lgrpc++_unsecure -lgrpc_unsecure -lgpr -laddress_sorting -lcares \
    -lupb_textformat_lib -lupb_json_lib -lupb_reflection_lib -lupb_wire_lib \
    -lupb_message_lib -lutf8_range_lib -lupb_mini_descriptor_lib \
    -lupb_mini_table_lib -lupb_hash_lib -lupb_mem_lib -lupb_base_lib \
    -lupb_lex_lib -lprotobuf -lzoslib -lre2 -lz $ABSL
LIBPATH=$G/lib ./t   # expect: channel OK, then 0C1 branch-to-NULL
```

Note `-fzos-le-char-mode=ascii` is required: it selects the
`std::__1_a` (ASCII + BFP) libc++ ABI namespace that the zopen libraries
were built with (see `__config_site` in the OELCPP headers).

## Recommended next steps

1. Upstream the two zopen bugs (NULL polling-engine vtable after channel
   creation; event-engine thread crash in absl time code) to
   zopencommunity/grpcport, with the reproducers above.
2. Once fixed, daemon mode should work with no further Bazel-port changes
   (the `grpc_init()` call and single-core link are already in place).
3. Until then, use `bazel --batch` (fully working, verified on user
   workspaces).
