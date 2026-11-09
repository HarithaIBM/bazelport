# com_google_protobuf — z/OS system-installed protobuf, exposed as a Bazel repo.
#
# Everything here wraps artifacts already on this system:
#   - protoc      : zopen z/OS protoc 31.0 binary (/home/haritha/zopen_23may/zopen/usr/local/bin/protoc)
#   - java jars   : protobuf-java 3.21.7 shipped in the bazel dist (derived/jars, platform-neutral bytecode)
#   - C++ lib/hdrs: zopen-built libprotobuf.a + headers (z/OS object code, z/OS-patched abseil)
#   - well-known type .protos: from the zopen install's include tree
#
# Layout note: the C++ include root is `includes/` — it must contain the real
# upstream directory structure (google/protobuf/..., absl/...) so that
# `#include "google/protobuf/x.h"` resolves with -Iincludes.
package(default_visibility = ["//visibility:public"])

ZLIB_DIR = "/home/haritha/zopen_23may/zopen/usr/local/zopen/grpc/grpc/lib"

# All abseil static archives from the zopen install (protobuf dependencies).
ABSL_LIBS = [
    ZLIB_DIR + "/libabsl_bad_any_cast_impl.a",
    ZLIB_DIR + "/libabsl_bad_optional_access.a",
    ZLIB_DIR + "/libabsl_bad_variant_access.a",
    ZLIB_DIR + "/libabsl_base.a",
    ZLIB_DIR + "/libabsl_city.a",
    ZLIB_DIR + "/libabsl_civil_time.a",
    ZLIB_DIR + "/libabsl_cord.a",
    ZLIB_DIR + "/libabsl_cord_internal.a",
    ZLIB_DIR + "/libabsl_cordz_functions.a",
    ZLIB_DIR + "/libabsl_cordz_handle.a",
    ZLIB_DIR + "/libabsl_cordz_info.a",
    ZLIB_DIR + "/libabsl_cordz_sample_token.a",
    ZLIB_DIR + "/libabsl_crc32c.a",
    ZLIB_DIR + "/libabsl_crc_cord_state.a",
    ZLIB_DIR + "/libabsl_crc_cpu_detect.a",
    ZLIB_DIR + "/libabsl_crc_internal.a",
    ZLIB_DIR + "/libabsl_debugging_internal.a",
    ZLIB_DIR + "/libabsl_decode_rust_punycode.a",
    ZLIB_DIR + "/libabsl_demangle_internal.a",
    ZLIB_DIR + "/libabsl_demangle_rust.a",
    ZLIB_DIR + "/libabsl_die_if_null.a",
    ZLIB_DIR + "/libabsl_examine_stack.a",
    ZLIB_DIR + "/libabsl_exponential_biased.a",
    ZLIB_DIR + "/libabsl_failure_signal_handler.a",
    ZLIB_DIR + "/libabsl_flags_commandlineflag.a",
    ZLIB_DIR + "/libabsl_flags_commandlineflag_internal.a",
    ZLIB_DIR + "/libabsl_flags_config.a",
    ZLIB_DIR + "/libabsl_flags_internal.a",
    ZLIB_DIR + "/libabsl_flags_marshalling.a",
    ZLIB_DIR + "/libabsl_flags_parse.a",
    ZLIB_DIR + "/libabsl_flags_private_handle_accessor.a",
    ZLIB_DIR + "/libabsl_flags_program_name.a",
    ZLIB_DIR + "/libabsl_flags_reflection.a",
    ZLIB_DIR + "/libabsl_flags_usage.a",
    ZLIB_DIR + "/libabsl_flags_usage_internal.a",
    ZLIB_DIR + "/libabsl_graphcycles_internal.a",
    ZLIB_DIR + "/libabsl_hash.a",
    ZLIB_DIR + "/libabsl_hashtablez_sampler.a",
    ZLIB_DIR + "/libabsl_int128.a",
    ZLIB_DIR + "/libabsl_kernel_timeout_internal.a",
    ZLIB_DIR + "/libabsl_leak_check.a",
    ZLIB_DIR + "/libabsl_log_entry.a",
    ZLIB_DIR + "/libabsl_log_flags.a",
    ZLIB_DIR + "/libabsl_log_globals.a",
    ZLIB_DIR + "/libabsl_log_initialize.a",
    ZLIB_DIR + "/libabsl_log_internal_check_op.a",
    ZLIB_DIR + "/libabsl_log_internal_conditions.a",
    ZLIB_DIR + "/libabsl_log_internal_fnmatch.a",
    ZLIB_DIR + "/libabsl_log_internal_format.a",
    ZLIB_DIR + "/libabsl_log_internal_globals.a",
    ZLIB_DIR + "/libabsl_log_internal_log_sink_set.a",
    ZLIB_DIR + "/libabsl_log_internal_message.a",
    ZLIB_DIR + "/libabsl_log_internal_nullguard.a",
    ZLIB_DIR + "/libabsl_log_internal_proto.a",
    ZLIB_DIR + "/libabsl_log_internal_structured_proto.a",
    ZLIB_DIR + "/libabsl_log_severity.a",
    ZLIB_DIR + "/libabsl_log_sink.a",
    ZLIB_DIR + "/libabsl_low_level_hash.a",
    ZLIB_DIR + "/libabsl_malloc_internal.a",
    ZLIB_DIR + "/libabsl_periodic_sampler.a",
    ZLIB_DIR + "/libabsl_poison.a",
    ZLIB_DIR + "/libabsl_random_distributions.a",
    ZLIB_DIR + "/libabsl_random_internal_distribution_test_util.a",
    ZLIB_DIR + "/libabsl_random_internal_platform.a",
    ZLIB_DIR + "/libabsl_random_internal_pool_urbg.a",
    ZLIB_DIR + "/libabsl_random_internal_randen.a",
    ZLIB_DIR + "/libabsl_random_internal_randen_hwaes.a",
    ZLIB_DIR + "/libabsl_random_internal_randen_hwaes_impl.a",
    ZLIB_DIR + "/libabsl_random_internal_randen_slow.a",
    ZLIB_DIR + "/libabsl_random_internal_seed_material.a",
    ZLIB_DIR + "/libabsl_random_seed_gen_exception.a",
    ZLIB_DIR + "/libabsl_random_seed_sequences.a",
    ZLIB_DIR + "/libabsl_raw_hash_set.a",
    ZLIB_DIR + "/libabsl_raw_logging_internal.a",
    ZLIB_DIR + "/libabsl_scoped_set_env.a",
    ZLIB_DIR + "/libabsl_spinlock_wait.a",
    ZLIB_DIR + "/libabsl_stacktrace.a",
    ZLIB_DIR + "/libabsl_status.a",
    ZLIB_DIR + "/libabsl_statusor.a",
    ZLIB_DIR + "/libabsl_str_format_internal.a",
    ZLIB_DIR + "/libabsl_strerror.a",
    ZLIB_DIR + "/libabsl_string_view.a",
    ZLIB_DIR + "/libabsl_strings.a",
    ZLIB_DIR + "/libabsl_strings_internal.a",
    ZLIB_DIR + "/libabsl_symbolize.a",
    ZLIB_DIR + "/libabsl_synchronization.a",
    ZLIB_DIR + "/libabsl_throw_delegate.a",
    ZLIB_DIR + "/libabsl_time.a",
    ZLIB_DIR + "/libabsl_time_zone.a",
    ZLIB_DIR + "/libabsl_tracing_internal.a",
    ZLIB_DIR + "/libabsl_utf8_for_code_point.a",
    ZLIB_DIR + "/libabsl_vlog_config_internal.a",
    ZLIB_DIR + "/libcares.a",
    ZLIB_DIR + "/libupb.a",
    ZLIB_DIR + "/libupb_base_lib.a",
    ZLIB_DIR + "/libupb_hash_lib.a",
    ZLIB_DIR + "/libutf8_range.a",
    ZLIB_DIR + "/libutf8_range_lib.a",
    ZLIB_DIR + "/libutf8_validity.a",
]

# ---- protoc (system z/OS binary via wrapper script that re-tags JAR
#      outputs as binary; genrule with executable=True gives a single-file
#      executable target, satisfying allow_single_file + executable
#      attrs of the proto rules) ----
genrule(
    name = "protoc",
    srcs = ["protoc_wrapper.sh"],
    outs = ["protoc_out"],
    cmd = "cp $(location protoc_wrapper.sh) $@ && chmod +x $@",
    executable = True,
)

# ---- Java protobuf runtime (dist-shipped jars, same as Stage-1 classpath) ----
java_import(
    name = "protobuf_java",
    jars = [
        "jars/libcore.jar",
        "jars/liblite.jar",  # ByteString and lite runtime classes
    ],
)

java_import(
    name = "protobuf_java_util",
    jars = ["jars/libutil.jar"],
)

java_import(
    name = "protobuf_javalite",
    jars = ["jars/liblite.jar"],
)

# Python protos: prebuilt python artifacts are not needed for bazel-bin_nojdk.
filegroup(
    name = "protobuf_python",
    srcs = [],
)

# ---- Well-known type protos (from the zopen install include tree).
#      strip_import_prefix makes the import path google/protobuf/<n>.proto. ----
[
    proto_library(
        name = n + "_proto",
        srcs = ["includes/google/protobuf/" + n + ".proto"],
        strip_import_prefix = "includes",
    )
    for n in [
        "any",
        "api",
        "descriptor",
        "duration",
        "empty",
        "field_mask",
        "source_context",
        "struct",
        "timestamp",
        "type",
        "wrappers",
    ]
]

# ---- C++ protobuf (zopen-built static lib + headers).
#      The static archive references its abseil/utf8_range/c-ares deps, which
#      are linked directly from the same zopen install. ----
cc_library(
    name = "protoc_lib",
    srcs = [],
    hdrs = glob([
        "includes/google/protobuf/**/*.h",
        "includes/absl/**/*.h",
        "includes/google/protobuf/**/*.proto",
    ]),
    includes = ["includes"],
    linkopts = [ZLIB_DIR + "/libprotobuf.a", ZLIB_DIR + "/libprotoc.a"] + ABSL_LIBS + [
        # zopen-built protobuf uses zoslib's ASCII-mode runtime wrappers
        "/home/haritha/zopen_23may/zopen/usr/local/zopen/zoslib/zoslib-v4.0.5.20260618_120743.zos/lib/libzoslib.a",
        "-lpthread",
        "-ldl",
    ],
)

alias(
    name = "protobuf",
    actual = ":protoc_lib",
)

# ---- Proto-language toolchains (defaults used by bazel's proto rules, cf.
#      ProtoConstants.java -> //tools/proto:* aliases). Mirrors upstream
#      protobuf's root BUILD declarations; runtimes point at our local targets. ----
proto_lang_toolchain(
    name = "cc_toolchain",
    command_line = "--cpp_out=$(OUT)",
    output_files = "multiple",
    progress_message = "Generating C++ %{output_short_paths}",
    runtime = ":protoc_lib",
)

proto_lang_toolchain(
    name = "java_toolchain",
    command_line = "--java_out=$(OUT)",
    output_files = "multiple",
    progress_message = "Generating Java %{output_short_paths}",
    runtime = ":protobuf_java",
)

proto_lang_toolchain(
    name = "javalite_toolchain",
    command_line = "--java_out=$(OUT)",
    output_files = "multiple",
    progress_message = "Generating Java lite %{output_short_paths}",
    runtime = ":protobuf_javalite",
)

filegroup(
    name = "protobuf_headers",
    srcs = glob([
        "includes/google/protobuf/**/*.h",
        "includes/absl/**/*.h",
    ]),
)
