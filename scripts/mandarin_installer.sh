#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR

# Copy the bundled Mandarin dictionary for the current user without requiring a rebuild or root privileges.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	# Define the path to the target directory
	local -r target_dir="$HOME/.local/gd-mandarin/"
	# Check if the target directory exists. If not, create it.
	mkdir -p -- "$target_dir" || die "can't create Mandarin dictionary directory"
	cp -- res/mandarin_dict/* "$target_dir" || die "can't install Mandarin dictionary files"
}

main "$@"
