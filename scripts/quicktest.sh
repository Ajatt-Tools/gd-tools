#!/usr/bin/env bash

set -xeuo pipefail

ROOT_DIR="$(git rev-parse --show-toplevel)" || exit 1
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

readonly mode=release
readonly prog=tests

xmake config -m "$mode"
xmake config --tests=y
xmake build -w "$prog"
xmake run --workdir="$(pwd)" "$prog"
