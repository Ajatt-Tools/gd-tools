#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

xmake config -P "$ROOT_DIR" -y -m debug
xmake build -P "$ROOT_DIR" -y gd-tools
xmake run -P "$ROOT_DIR" -y gd-tools -- "$@"
