#!/usr/bin/env bash

# Shared shell helpers. Source from sibling scripts with . "$(dirname -- "$0")/lib.sh".

# Print a consistent fatal error and terminate the calling script.
die() {
	echo "oops: $*" >&2
	exit 1
}

# Return true when an executable command is available on PATH.
is_installed() {
	[[ -x $(command -v "$1") ]]
}
