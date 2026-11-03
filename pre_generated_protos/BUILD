# Pre-generated protobuf Java sources (compatible with the protobuf-java
# 3.21.7 runtime from the dist archive). Used instead of regenerating
# with protoc 31, which produces code incompatible with protobuf 21.

load("@rules_java//java:defs.bzl", "java_library")

package(default_visibility = ["//visibility:public"])

java_library(
    name = "pre_generated_protos",
    srcs = glob(
        ["//derived/src/java/**/*.java"],
        allow_empty = True,
    ),
    deps = [
        "@com_google_protobuf//:protobuf_java",
        "@com_google_protobuf//:protobuf_java_util",
    ],
)

filegroup(
    name = "pre_generated_proto_files",
    srcs = glob(
        ["//derived/src/java/**/*.java"],
        allow_empty = True,
    ),
)
