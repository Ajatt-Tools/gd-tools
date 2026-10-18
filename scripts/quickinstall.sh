#!/usr/bin/env bash

# This script can be used instead of `make install`.

set -xeuo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
this_dir=$(dirname -- "$(readlink -f -- "$0")")
readonly this_dir
readonly target="gd-tools"

# Build and install the application, optionally installing the current user's Mandarin dictionary.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed xmake || die "xmake is required"
	local local_install=false run_mandarin_script=false
	# Parse optional flags
	while (($# > 0)); do
		case $1 in
		--local | --user)
			local_install=true
			;;
		--mandarin)
			run_mandarin_script=true
			;;
		esac
		shift
	done
	# Freeze the selected options after parsing so repeated flags remain valid.
	readonly local_install run_mandarin_script
	xmake config -P "$ROOT_DIR" -y -m release --tests=n
	xmake build -P "$ROOT_DIR" -vwy "$target"
	local -ar args=(xmake install -P "$ROOT_DIR" -v --all)
	if $local_install; then
		"${args[@]}" --installdir="$HOME/.local/" "$target"
	else
		"${args[@]}" --installdir=/usr --admin "$target"
	fi
	if $run_mandarin_script; then
		"$this_dir/mandarin_installer.sh"
	fi
}

main "$@"
