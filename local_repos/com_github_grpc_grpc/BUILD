# gRPC C++ (z/OS system install via zopen) exposed as a Bazel repo.
#
# Wraps artifacts already on this system — no downloads, no foreign binaries:
#   - grpc_cpp_plugin : zopen z/OS binary (via wrapper script)
#   - headers/libs    : zopen-built static archives + headers (z/OS object code)
package(default_visibility = ["//visibility:public"])

GRPC_LIBS = "/home/haritha/zopen_23may/zopen/usr/local/zopen/grpc/grpc/lib"

# Headers needed to compile cc_grpc_library-generated code.
# The include root `includes/` contains the real upstream structure
# (grpc/..., grpc++/...) so that #include <grpcpp/...> resolves.
cc_library(
    name = "grpc++_codegen_proto",
    hdrs = glob([
        "includes/grpc/**/*.h",
        "includes/grpc++/**/*.h",
        "includes/grpcpp/**/*.h",
        "includes/grpcpp/**/*.hpp",
    ]),
    includes = ["includes"],
    # grpc++ headers include protobuf headers
    deps = ["@com_google_protobuf//:protoc_lib"],
)

# The linkable grpc++ library: zopen static archives + absl set.
cc_library(
    name = "grpc++_unsecure",
    srcs = [],
    linkstatic = 1,
    linkopts = [
        GRPC_LIBS + "/libgrpc++_unsecure.a",
        GRPC_LIBS + "/libgrpc_unsecure.a",
        GRPC_LIBS + "/libgpr.a",
        GRPC_LIBS + "/libaddress_sorting.a",
        GRPC_LIBS + "/libcares.a",
        # NOTE: do NOT add libgrpc.a here. It contains a second copy of the
        # gRPC core (grpc_completion_queue_create_internal, poller vtables,
        # plugin registries) that is also present in libgrpc_unsecure.a.
        # Linking both gives two copies of the core's global state: init
        # registers engines in one copy, use reads NULL vtables from the
        # other -> 0C4 protection exception at the first completion-queue
        # creation. Bazel uses insecure channels only, so the unsecure core
        # is sufficient.
        "-lpthread",
        "-ldl",
    ],
    deps = [":grpc++_codegen_proto"],
)

alias(
    name = "grpc++",
    actual = ":grpc++_unsecure",
)
