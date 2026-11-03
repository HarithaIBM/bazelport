load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_java//java:defs.bzl", "java_library")
load("@rules_python//python:proto.bzl", "py_proto_library")
load("//third_party/grpc:build_defs.bzl", "java_grpc_library")
load("//third_party/grpc/bazel:cc_grpc_library.bzl", "cc_grpc_library")

package(default_visibility = ["//visibility:public"])

FILES = [
    "action_cache",
    "android_deploy_info",
    "bazel_flags",
    "build",
    "builtin",
    "crosstool_config",
    "deps",
    "desugar_deps",
    "execution_statistics",
    "extra_actions_base",
    "invocation_policy",
    "java_compilation",
    "memory_pressure",
    "test_status",
    "worker_protocol",
    "execution_graph",
]

# z/OS: Use pre-generated Java proto sources from the dist archive
# (derived/src/java/) instead of regenerating with protoc 31, which
# produces code incompatible with the protobuf 21 runtime.
# All proto *_java_proto targets compile the same pre-generated sources.
java_library(
    name = "all_java_protos",
    srcs = ["//:pre_generated_protos"],
    deps = [
        "@com_google_protobuf//:protobuf_java",
        "@com_google_protobuf//:protobuf_java_util",
        "@maven//:io_grpc_grpc_stub",
        "@maven//:io_grpc_grpc_protobuf",
        "@maven//:io_grpc_grpc_api",
        "@maven//:io_grpc_grpc_context",
        "@maven//:javax_annotation_javax_annotation_api",
    ],
)

[proto_library(
    name = s + "_proto",
    srcs = [s + ".proto"],
) for s in FILES]

[alias(
    name = s + "_java_proto",
    actual = ":all_java_protos",
) for s in FILES]

[filegroup(
    name = s + "_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
) for s in FILES]

# Additional proto targets
proto_library(
    name = "analysis_v2_proto",
    srcs = ["analysis_v2.proto"],
    deps = [":build_proto"],
)

py_proto_library(
    name = "analysis_v2_py_proto",
    deps = [":analysis_v2_proto"],
)

alias(
    name = "analysis_v2_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "analysis_v2_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

alias(
    name = "any_java_proto",
    actual = ":all_java_protos",
)

alias(
    name = "wrappers_java_proto",
    actual = ":all_java_protos",
)

proto_library(
    name = "command_server_proto",
    srcs = ["command_server.proto"],
    deps = [
        ":failure_details_proto",
        "@com_google_protobuf//:any_proto",
    ],
)

alias(
    name = "command_server_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "command_server_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "failure_details_proto",
    srcs = ["failure_details.proto"],
    deps = ["@com_google_protobuf//:descriptor_proto"],
)

alias(
    name = "failure_details_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "failure_details_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "option_filters_proto",
    srcs = ["option_filters.proto"],
)

alias(
    name = "option_filters_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "option_filters_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "command_line_proto",
    srcs = ["command_line.proto"],
    deps = [":option_filters_proto"],
)

alias(
    name = "command_line_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "command_line_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

genrule(
    name = "desugar_deps_zip",
    srcs = ["desugar_deps.proto"],
    outs = ["desugar_deps.zip"],
    cmd = "zip -q $@ $<",
    visibility = ["//src/tools/singlejar:__pkg__"],
)

filegroup(
    name = "desugar_deps_filegroup",
    srcs = ["desugar_deps.proto"],
    visibility = ["//src/tools/singlejar:__pkg__"],
)

cc_proto_library(
    name = "desugar_deps_cc_proto",
    deps = [":desugar_deps_proto"],
)

cc_proto_library(
    name = "worker_protocol_cc_proto",
    deps = [":worker_protocol_proto"],
)

alias(
    name = "command_server_java_grpc",
    actual = ":all_java_protos",
)

cc_proto_library(
    name = "command_server_cc_proto",
    deps = [":command_server_proto"],
)

cc_grpc_library(
    name = "command_server_cc_grpc",
    srcs = [":command_server_proto"],
    extra_cc_library_kwargs = {"defines": ["GRPC_USE_PROTO_LITE=ON"]},
    deps = [
        ":command_server_cc_proto",
        "//third_party/grpc:grpc++_unsecure",
    ],
)

py_proto_library(
    name = "build_pb_py",
    deps = [":build_proto"],
)

filegroup(
    name = "profile_java_proto_srcs",
    visibility = ["//visibility:private"],
    srcs = ["//:pre_generated_protos"],
)

cc_proto_library(
    name = "execution_statistics_cc_proto",
    deps = [":execution_statistics_proto"],
)

proto_library(
    name = "remote_execution_log_proto",
    srcs = ["remote_execution_log.proto"],
    deps = [
        "@com_google_protobuf//:timestamp_proto",
        "@googleapis//:google_bytestream_bytestream_proto",
        "@googleapis//:google_longrunning_operations_proto",
        "@googleapis//:google_rpc_status_proto",
        "@remoteapis//:build_bazel_remote_execution_v2_remote_execution_proto",
    ],
)

alias(
    name = "remote_execution_log_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "remote_execution_log_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "cache_salt_proto",
    srcs = ["cache_salt.proto"],
)

alias(
    name = "cache_salt_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "cache_salt_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "remote_scrubbing_proto",
    srcs = ["remote_scrubbing.proto"],
)

alias(
    name = "remote_scrubbing_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "remote_scrubbing_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "bazel_output_service_proto",
    srcs = ["bazel_output_service.proto"],
    deps = [
        "@com_google_protobuf//:any_proto",
        "@googleapis//:google_rpc_status_proto",
    ],
)

alias(
    name = "bazel_output_service_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "bazel_output_service_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "bazel_output_service_rev2_proto",
    srcs = ["bazel_output_service_rev2.proto"],
    deps = [
        "@remoteapis//:build_bazel_remote_execution_v2_remote_execution_proto",
    ],
)

alias(
    name = "bazel_output_service_rev2_java_proto",
    actual = ":all_java_protos",
)

alias(
    name = "bazel_output_service_java_grpc",
    actual = ":all_java_protos",
)

filegroup(
    name = "bazel_output_service_rev2_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "spawn_proto",
    srcs = ["spawn.proto"],
    deps = [
        "@com_google_protobuf//:duration_proto",
        "@com_google_protobuf//:timestamp_proto",
    ],
)

alias(
    name = "spawn_java_proto",
    actual = ":all_java_protos",
)

filegroup(
    name = "spawn_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

proto_library(
    name = "xcode_proto",
    srcs = ["xcode_config.proto"],
)

alias(
    name = "xcode_java_proto",
    actual = ":all_java_protos",
)

cc_proto_library(
    name = "xcode_cc_proto",
    deps = [":xcode_proto"],
)

filegroup(
    name = "xcode_java_proto_srcs",
    srcs = ["//:pre_generated_protos"],
)

filegroup(
    name = "srcs",
    srcs = glob(["**"]),
)

filegroup(
    name = "dist_jars",
    srcs = [s + "_java_proto_srcs" for s in FILES] + [
        ":analysis_v2_java_proto_srcs",
        ":bazel_output_service_java_grpc_srcs",
        ":bazel_output_service_java_proto_srcs",
        ":bazel_output_service_rev2_java_proto_srcs",
        ":cache_salt_java_proto_srcs",
        ":command_line_java_proto_srcs",
        ":command_server_java_grpc_srcs",
        ":command_server_java_proto_srcs",
        ":failure_details_java_proto_srcs",
        ":option_filters_java_proto_srcs",
        ":profile_java_proto_srcs",
        ":remote_execution_log_java_proto_srcs",
        ":remote_scrubbing_java_proto_srcs",
        ":spawn_java_proto_srcs",
        ":xcode_java_proto_srcs",
    ],
)
