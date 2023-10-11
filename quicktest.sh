#!/bin/bash

set -xeuo pipefail

readonly mode=release
readonly prog=tests

xmake f -m "$mode"
xmake f --tests=y
xmake build -w "$prog"
xmake run --workdir="$(pwd)" "$prog"
