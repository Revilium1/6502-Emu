#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"
CXX_COMPILER="${CXX:-g++}"

# A previous executable can occupy the directory name CMake needs here.
if [[ -e "$BUILD_DIR/6502" && ! -d "$BUILD_DIR/6502" ]]; then
	rm -f "$BUILD_DIR/6502"
fi

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
	-G "Unix Makefiles" \
	-DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
	-DCMAKE_BUILD_TYPE=Debug \
	-Dgtest_force_shared_crt=ON

cmake --build "$BUILD_DIR" --target M6502Test -j"$(nproc)"

if [[ "${1:-}" != "--build-only" ]]; then
    "$BUILD_DIR/6502/6502Test/M6502Test" "${@:2}"
fi