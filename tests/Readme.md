# z/OS Bazel sample project

A small project that builds with the z/OS Bazel port
(`bazelport`, https://github.com/HarithaIBM/bazelport).

## Targets

| Target | What it demonstrates |
|---|---|
| `//:hello_cc` | `cc_binary` + `cc_library` (clang, ASCII mode, C++17) |
| `//:hello_generated` | `genrule` generating C++ source at build time |
| `//:hello_sh` | `sh_binary` (runs under zopen bash) |

## Building

Use the bazel built by the port (`.bazelci/output/bazel`, or the one
installed by `zopen build`). Start the server from a shell that has the
port's environment (`source bazelport/buildenv` gives you the right PATH
with zopen tools, clang, python3):

```sh
cd ~/bazel-sample
~/code/bazel-7.2.0/bazelport/.bazelci/output/bazel build --noenable_bzlmod \
    //:hello_cc //:hello_generated //:hello_sh
```

Notes:
- `--noenable_bzlmod` — the port runs in WORKSPACE mode.
- Daemon (client-server) mode works; `--batch` is a fallback.
- All source files must carry an ASCII text tag (`chtag -tc ISO8859-1`)
  or a binary tag — untagged files are read as EBCDIC by compiler
  action spawns.

## Running

```sh
./bazel-bin/hello_cc
./bazel-bin/hello_generated
./bazel-bin/hello_sh
```
