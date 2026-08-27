#!/usr/bin/env bash

set -xeuo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

readonly mode=release
readonly prog=tests

xmake config -P "$ROOT_DIR" -y -m "$mode" --tests=y
xmake build -P "$ROOT_DIR" -y -w "$prog"
xmake run -P "$ROOT_DIR" --workdir="$ROOT_DIR" "$prog"
