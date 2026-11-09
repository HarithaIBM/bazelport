#!/bin/bash
# z/OS protoc wrapper: runs the system zopen protoc, then re-tags any
# created JAR/zip files as binary. The zopen protoc creates them with a
# text tag (ISO8859-1) because of the system's _CEE_RUNOPTS AUTOTAG setting,
# which breaks the z/OS JRE's zip filesystem reader (AUTOCVT.ERROR pread).
#
# This wrapper is used as the protoc executable in the Bazel build.

PROTOC=/home/haritha/zopen_23may/zopen/usr/local/zopen/grpc/grpc/bin/protoc-31.0.0

# Create a marker file to identify outputs created by this invocation
MARKER=$(mktemp /tmp/.protoc_marker.XXXXXX)
touch "$MARKER"

# Run protoc with all arguments
"$PROTOC" "$@"
rc=$?

# Re-tag any JAR/zip files created after the marker as binary.
# Search the entire bazel-out tree (protoc writes to bazel-out/ paths
# relative to the execroot, which is the CWD for Bazel actions).
if [ $rc -eq 0 ]; then
  find bazel-out -name "*.jar" -newer "$MARKER" -type f 2>/dev/null | while read -r f; do
    chtag -b "$f" 2>/dev/null
  done
  find bazel-out -name "*.zip" -newer "$MARKER" -type f 2>/dev/null | while read -r f; do
    chtag -b "$f" 2>/dev/null
  done
  # Also check the current directory (for outputs written to relative paths)
  find . -maxdepth 1 -name "*.jar" -newer "$MARKER" -type f 2>/dev/null | while read -r f; do
    chtag -b "$f" 2>/dev/null
  done
fi

rm -f "$MARKER"
exit $rc
