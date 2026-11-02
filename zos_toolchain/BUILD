# z/OS toolchain definitions

load("@bazel_tools//tools/jdk:default_java_toolchain.bzl", "default_java_toolchain")
load(":constraints.bzl", "is_zos")

package(default_visibility = ["//visibility:public"])

# z/OS Java runtime
java_runtime(
    name = "zos_java_runtime",
    java_home = "/usr/lpp/java/J21.0_64",
    visibility = ["//visibility:public"],
)

# z/OS Java runtime toolchain
toolchain(
    name = "zos_java_runtime_toolchain",
    exec_compatible_with = [
        ":is_zos",
        "@platforms//cpu:s390x",
    ],
    target_compatible_with = [
        ":is_zos",
        "@platforms//cpu:s390x",
    ],
    toolchain = ":zos_java_runtime",
    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",
)

# Bootstrap runtime (same as main runtime for z/OS)
toolchain(
    name = "zos_java_bootstrap_runtime_toolchain",
    exec_compatible_with = [
        ":is_zos",
        "@platforms//cpu:s390x",
    ],
    target_compatible_with = [
        ":is_zos",
        "@platforms//cpu:s390x",
    ],
    toolchain = ":zos_java_runtime",
    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",
)

# z/OS platform definition
platform(
    name = "zos_platform",
    constraint_values = [
        ":is_zos",
        "@platforms//cpu:s390x",
    ],
)
