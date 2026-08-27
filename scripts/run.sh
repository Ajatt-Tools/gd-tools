#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

mode=debug
prog=gd-tools
platform=$(uname)

xmake f -m "$mode"
xmake -w -v

./"build/${platform,,}/x86_64/$mode/$prog" "$@"
