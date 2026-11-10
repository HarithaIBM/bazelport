#!/bin/bash
# z/OS: wrapper that exposes the system-installed zopen grpc_cpp_plugin
# (z/OS-native binary) as a Bazel executable target.
exec /home/haritha/zopen_23may/zopen/usr/local/zopen/grpc/grpc/bin/grpc_cpp_plugin "$@"
