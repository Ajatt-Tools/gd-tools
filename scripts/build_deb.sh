#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR

# Build a versioned Debian package with the same curl linkage as release artifacts.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed xmake || die "xmake is required"
	[[ -n ${GD_TOOLS_VERSION:-} ]] || die "GD_TOOLS_VERSION must contain the package version"
	# Force shared libcurl so local DEB builds outside Ubuntu match release artifacts.
	export GD_TOOLS_USE_SYSTEM_CURL=y
	xmake config -P "$ROOT_DIR" -y -m release --tests=n
	xmake pack -P "$ROOT_DIR" -y -v --formats=deb --outputdir=dist --basename=gd-tools gd-tools
}

main "$@"
