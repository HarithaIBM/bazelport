## Building Bazel on z/OS

### 1. Clone and tag the buildenv

git cannot store z/OS file tags; the zopen-build harness reads `buildenv`
as EBCDIC when it is untagged, so one command is required after cloning:

```bash
git clone https://github.com/HarithaIBM/bazelport.git
cd bazelport
/bin/chtag -tc ISO8859-1 buildenv
```

(Everything else — patches, deps, the source tree — is tagged
automatically during the build.)

### 2. Prerequisites

All toolchain locations are environment variables with auto-detection
(see the header of `buildenv`): `JAVA_HOME`, `PYTHON_HOME`,
`COMPILER_PATH`, `ZOPEN_ROOTFS`, `ZOSLIB_HOME`, `GRPC_HOME`,
`SQLITE_HOME`, `ZLIB_HOME`, `OPENSSL_HOME`.

### 3. Build

```bash
zopen build -vv
```

On first use the build applies `stable-patches/` to the dist extract,
provisions `local_repos/` from `deps/` + zopen installs, tags the source
tree, then runs Stage 1 (bootstrap) and Stage 2 (bazel builds itself).
A clean build takes ~1.5–2 h; the log is intentionally quiet.

The installed product is always the Stage 2 binary (`output/bazel`) —
the bootstrap jar is an intermediate artifact only.
