#!/usr/bin/env bash

set -xeuo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
readonly mode=release
readonly prog=tests

# Build and run the Catch2 suite from the repository root so bundled dictionary fixtures resolve correctly.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed xmake || die "xmake is required"
	xmake config -P "$ROOT_DIR" -y -m "$mode" --tests=y
	xmake build -P "$ROOT_DIR" -y -w "$prog"
	xmake run -P "$ROOT_DIR" --workdir="$ROOT_DIR" "$prog"
}

main "$@"
