# z/OS C++ Toolchain Configuration for Bazel
# This file defines a minimal C++ toolchain for IBM z/OS using clang

load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "action_config",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
    "tool_path",
    "with_feature_set",
)
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

all_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.clif_match,
    ACTION_NAMES.lto_backend,
]

all_cpp_compile_actions = [
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.clif_match,
]

all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]

def _impl(ctx):
    toolchain_identifier = "zos-clang-toolchain"
    host_system_name = "zos"
    target_system_name = "zos"
    target_cpu = "s390x"
    target_libc = "zos"
    compiler = "clang"
    abi_version = "local"
    abi_libc_version = "local"

    # Path to IBM Open XL C/C++ compiler
    clang_bin = "/home/haritha/install_test/usr/lpp/IBM/oelcpp/v3r0/bin"
    # Path to protobuf/grpc (from zopen)
    zopen_root = "/home/haritha/zopen_23may/zopen/usr/local"

    tool_paths = [
        tool_path(name = "gcc", path = clang_bin + "/clang"),
        tool_path(name = "g++", path = clang_bin + "/clang++"),
        tool_path(name = "cpp", path = clang_bin + "/clang"),
        tool_path(name = "ar", path = "/bin/ar"),
        tool_path(name = "ld", path = clang_bin + "/clang++"),  # Use clang++ for linking
        tool_path(name = "nm", path = "/bin/nm"),
        tool_path(name = "objdump", path = "/bin/objdump"),
        tool_path(name = "strip", path = "/bin/strip"),
        tool_path(name = "gcov", path = "/bin/false"),  # Not available
        tool_path(name = "dwp", path = "/bin/false"),   # Not available
    ]

    # Default compile flags for z/OS
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-I" + zopen_root + "/include",  # protobuf/grpc headers
                            "-U_FORTIFY_SOURCE",
                            "-D_FORTIFY_SOURCE=1",
                            "-fstack-protector",
                            "-Wall",
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = ["-g"])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O2",
                            "-DNDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    # C++ specific flags
    default_cxx_flags_feature = feature(
        name = "default_cxx_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_cpp_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-std=c++14",  # Bazel requires at least C++14
                        ],
                    ),
                ],
            ),
        ],
    )

    # Link flags for z/OS
    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-L" + zopen_root + "/lib",  # protobuf/grpc libraries
                            "-lgrpc++",
                            "-lgrpc",
                            "-lprotobuf",
                            "-lstdc++",
                            "-lm",  # Math library
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-Wl,--gc-sections",  # Remove unused sections
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    # Optimization features
    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")
    fastbuild_feature = feature(name = "fastbuild")

    # PIC support
    supports_pic_feature = feature(name = "supports_pic", enabled = True)

    # Dynamic linking support
    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)

    features = [
        default_compile_flags_feature,
        default_cxx_flags_feature,
        default_link_flags_feature,
        opt_feature,
        dbg_feature,
        fastbuild_feature,
        supports_pic_feature,
        supports_dynamic_linker_feature,
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = toolchain_identifier,
        host_system_name = host_system_name,
        target_system_name = target_system_name,
        target_cpu = target_cpu,
        target_libc = target_libc,
        compiler = compiler,
        abi_version = abi_version,
        abi_libc_version = abi_libc_version,
        tool_paths = tool_paths,
        features = features,
    )

zos_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)
