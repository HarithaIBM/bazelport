#!/bin/bash
# Create a minimal rules_java_builtin for z/OS bootstrap

OUTPUT_DIR="${OUTPUT_DIR:-$(pwd)/output}"
ARCHIVE_DIR="${OUTPUT_DIR}/archive"
RULES_JAVA_DIR="${ARCHIVE_DIR}/rules_java"

mkdir -p "${RULES_JAVA_DIR}/toolchains"

# Create WORKSPACE
cat > "${RULES_JAVA_DIR}/WORKSPACE" << 'WORKSPACE_EOF'
workspace(name = "rules_java_builtin")
WORKSPACE_EOF

# Create MODULE.bazel
cat > "${RULES_JAVA_DIR}/MODULE.bazel" << 'MODULE_EOF'
module(name = "rules_java_builtin", version = "7.6.1")
MODULE_EOF

# Create BUILD
cat > "${RULES_JAVA_DIR}/BUILD" << 'BUILD_EOF'
# Minimal rules_java_builtin for z/OS
BUILD_EOF

# Create toolchains/BUILD
cat > "${RULES_JAVA_DIR}/toolchains/BUILD" << 'TOOLCHAINS_EOF'
# Use system Java for z/OS
package(default_visibility = ["//visibility:public"])

# Platform detection
config_setting(
    name = "zos",
    constraint_values = ["@platforms//os:zos"],
)
TOOLCHAINS_EOF

echo "Created minimal rules_java_builtin in ${RULES_JAVA_DIR}"
