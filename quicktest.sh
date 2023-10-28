#!/bin/bash

set -xeuo pipefail

readonly mode=release
readonly prog=tests

xmake config -m "$mode"
xmake config --tests=y
xmake build -w "$prog"
xmake run --workdir="$(pwd)" "$prog"
