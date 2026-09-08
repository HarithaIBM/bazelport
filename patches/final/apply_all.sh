#!/bin/bash
################################################################################
# Apply All Bazel 7.2.0 z/OS Patches
# 
# This script applies all patches in the correct order to a clean Bazel 7.2.0
# source tree.
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."  # Go to Bazel source root

echo "══════════════════════════════════════════════════════════════════"
echo "  Applying Bazel 7.2.0 z/OS Patches"
echo "══════════════════════════════════════════════════════════════════"
echo ""

# Verify we're in the right place
if [ ! -f "MODULE.bazel" ]; then
  echo "❌ ERROR: Not in Bazel source directory"
  echo "Expected to find MODULE.bazel"
  exit 1
fi

echo "✅ Found Bazel source tree"
echo ""

# Count patches
PATCH_COUNT=$(ls -1 "$SCRIPT_DIR"/*.patch 2>/dev/null | wc -l)
echo "Found $PATCH_COUNT patches to apply"
echo ""

# Apply each patch
SUCCESS=0
FAILED=0

for patch in "$SCRIPT_DIR"/*.patch; do
  PATCH_NAME=$(basename "$patch")
  echo -n "Applying $PATCH_NAME... "
  
  if patch -p1 --dry-run < "$patch" > /dev/null 2>&1; then
    patch -p1 < "$patch" > /dev/null 2>&1
    echo "✅"
    ((SUCCESS++))
  else
    echo "❌ FAILED"
    ((FAILED++))
    echo "  Error applying $PATCH_NAME"
    echo "  You may need to apply this manually"
  fi
done

echo ""
echo "══════════════════════════════════════════════════════════════════"
echo "  Results"
echo "══════════════════════════════════════════════════════════════════"
echo "  ✅ Successfully applied: $SUCCESS"
echo "  ❌ Failed: $FAILED"
echo ""

if [ $FAILED -eq 0 ]; then
  echo "🎉 All patches applied successfully!"
  echo ""
  echo "Next steps:"
  echo "  1. Set up environment:"
  echo "     export JAVA_HOME=/usr/lpp/java/J21.0_64"
  echo "     export PYTHON_BIN_PATH=/home/opnzos/local/pyz/bin/python3"
  echo "     export GOROOT=/home/haritha/dhgo/go"
  echo "     export OUTPUT_DIR=\$(pwd)/output"
  echo ""
  echo "  2. Build Stage 1:"
  echo "     ./compile.sh compile"
  echo ""
  echo "  3. Test:"
  echo "     ./output/bazel version"
  echo ""
else
  echo "⚠️  Some patches failed to apply"
  echo "Review the errors above and apply manually if needed"
  exit 1
fi
