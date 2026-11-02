# z/OS WORKSPACE — system-installed dependencies only, zero network.
#
# Repositories come from:
#   1. zopen system installs (grpc, zlib, sqlite)
#   2. the wrapper repo for com_google_protobuf (zopen protoc + dist jars + zopen lib)
#   3. rule repos extracted from the dist-bundled derived/repository_cache (pure Starlark)
#   4. the system JDK via local_java_repository (JAVA_HOME)
#
# @bazel_tools needs no entry: the embedded tools.WORKSPACE prefix (compiled into
# the jar) auto-defines it -> $install_base/embedded_tools.
workspace(name = "io_bazel")

# ---------------------------------------------------------------------------
# 1. zopen system-installed libraries
# ---------------------------------------------------------------------------
# zopen gRPC — exposed through a local wrapper repo (system z/OS binary +
# static libs + headers), same pattern as com_google_protobuf.
local_repository(
    name = "com_github_grpc_grpc",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/com_github_grpc_grpc",
)

local_repository(
    name = "zlib",
    path = "/home/haritha/zopen_23may/zopen/usr/local/zopen/zlib",
)

local_repository(
    name = "sqlite",
    path = "/home/haritha/zopen_23may/zopen/usr/local/zopen/sqlite",
)

# ---------------------------------------------------------------------------
# 2. protobuf wrapper: system zopen protoc + dist-shipped java jars + zopen C++ lib
# ---------------------------------------------------------------------------
local_repository(
    name = "com_google_protobuf",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/com_google_protobuf",
)

# ---------------------------------------------------------------------------
# 3. rule repos (locally extracted, pure Starlark — see local_repos/)
# ---------------------------------------------------------------------------
local_repository(
    name = "rules_java",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_java",
)

# @bazel_tools maps @rules_java -> @rules_java_builtin (repo_mapping in the
# embedded tools.WORKSPACE), so the builtin repo must exist as well.
local_repository(
    name = "rules_java_builtin",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_java",
)

local_repository(
    name = "bazel_skylib",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/bazel_skylib",
)

local_repository(
    name = "rules_cc",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_cc",
)

local_repository(
    name = "rules_python",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_python",
)

# rules_python's internal config repo — normally created by its own WORKSPACE /
# module extension, which are not evaluated for external repos in WORKSPACE mode.
load("@rules_python//python/private:internal_config_repo.bzl", "internal_config_repo")

internal_config_repo(name = "rules_python_internal")

local_repository(
    name = "rules_proto",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_proto",
)

local_repository(
    name = "rules_pkg",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_pkg",
)

# buildozer: needed only as a FILE (buildozer.bzl) loaded by
# //src/tools/bzlmod:utils.bzl from the root BUILD. The buildozer BINARY is
# never executed in WORKSPACE mode (only bzlmod extensions/dist rules use it).
local_repository(
    name = "buildozer",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/buildozer",
)

# rules_license: loaded by the root BUILD (license rule); local copy avoids
# the suffix http_archive path entirely.
local_repository(
    name = "rules_license",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_license",
)

# stardoc (io_bazel_skydoc): needed only as a FILE load from
# tools/build_defs/repo/BUILD (docs rule, never executed in our graph).
local_repository(
    name = "io_bazel_skydoc",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/stardoc",
)

# The pre-resolved maven repository shipped in the dist (Stage-1 classpath),
# exposed under the apparent name the bazel sources use (@maven//:com_google_guava_guava ...).
local_repository(
    name = "maven",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/.bazelci/derived/maven",
)

# Required by derived/maven/BUILD (jvm_import rule).
local_repository(
    name = "rules_jvm_external",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/rules_jvm_external",
)

# Vendored API proto repos — COPIES of third_party/{remoteapis,googleapis} so that
# the external repos (@remoteapis//:) and the main-repo path labels
# (//third_party/remoteapis:...) do not cross a repository boundary.
local_repository(
    name = "remoteapis",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/remoteapis",
)

local_repository(
    name = "googleapis",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/googleapis",
)

# BLAKE3 (C library, portable build) — needed by //src/main/native:blake3_jni.
# Source from the dist repository cache; BUILD is bazel's own third_party/blake3
# template whose default branch compiles portable C only (no SIMD) — correct for s390x.
local_repository(
    name = "blake3",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/blake3",
)

# zstd-jni — prebuilt jar shipped in the dist (derived/jars).
local_repository(
    name = "zstd-jni",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/zstd-jni",
)

# sqlite3 — system zopen install (headers + static lib), linked not compiled.
local_repository(
    name = "sqlite3",
    path = "/home/haritha/code/bazel-7.2.0/bazelport/local_repos/sqlite3",
)

# platforms 0.0.9 (from dist repository_cache; includes cpu:s390x)
# NOTE: @platforms / @internal_platforms_do_not_use / @local_config_platform /
# @host_platform are now provided by the local_config_platform.WORKSPACE
# suffix (default for every workspace), pointing at the platforms copy in the
# install base -- no need to redefine them here (redefinition conflicts).

# ---------------------------------------------------------------------------
# 4. system JDK as @local_jdk (IBM Semeru z/OS via JAVA_HOME)
#    Generates a proper java_runtime(name="jdk") + toolchain definitions.
# ---------------------------------------------------------------------------
load("@rules_java//toolchains:local_java_repository.bzl", "local_java_repository")
load("@rules_java//toolchains:jdk_build_file.bzl", "JDK_BUILD_TEMPLATE")

local_java_repository(
    name = "local_jdk",
    java_home = "/home/haritha/dhjava/J21.0_64",
    build_file_content = JDK_BUILD_TEMPLATE,
)

# Register the toolchains generated in @local_jdk (in bzlmod, upstream's module
# extension registers them automatically; WORKSPACE mode requires this).
register_toolchains("@local_jdk//:runtime_toolchain_definition")
register_toolchains("@local_jdk//:bootstrap_runtime_toolchain_definition")

# NO http_archive = NO downloads = NO network!
