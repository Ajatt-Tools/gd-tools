#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

: "${GD_TOOLS_VERSION:?GD_TOOLS_VERSION must contain the package version}"
# DEB builds use Ubuntu's supported shared libcurl, including when run locally.
export GD_TOOLS_USE_SYSTEM_CURL=y

xmake config -P "$ROOT_DIR" -y -m release --tests=n
xmake pack -P "$ROOT_DIR" -y -v --formats=deb --outputdir=dist --basename=gd-tools gd-tools
