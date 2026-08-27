#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(git rev-parse --show-toplevel)" || exit 1
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

: "${GD_TOOLS_VERSION:?GD_TOOLS_VERSION must contain the package version}"

xmake config -P "$ROOT_DIR" -y -m release --tests=n
xmake pack -P "$ROOT_DIR" -y -v --formats=deb --outputdir=dist --basename=gd-tools gd-tools
