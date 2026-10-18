#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR

# Build the debug application and forward all requested command arguments unchanged.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed xmake || die "xmake is required"
	xmake config -P "$ROOT_DIR" -y -m debug
	xmake build -P "$ROOT_DIR" -y gd-tools
	xmake run -P "$ROOT_DIR" -y gd-tools -- "$@"
}

main "$@"
