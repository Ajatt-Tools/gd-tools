#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
readonly target=gd-tools
readonly mandarin_dir="$HOME/.local/gd-mandarin"

# Remove only bundled dictionary filenames, preserving files added by the user.
remove_mandarin_dictionary() {
	local source_file
	for source_file in res/mandarin_dict/*; do
		rm -f -- "$mandarin_dir/${source_file##*/}" || die "can't remove Mandarin dictionary file"
	done
	if [[ -d "$mandarin_dir" ]]; then
		rmdir --ignore-fail-on-non-empty -- "$mandarin_dir" || die "can't remove empty Mandarin dictionary directory"
	fi
}

# Uninstall the selected application prefix and remove only install-owned Mandarin files.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed xmake || die "xmake is required"
	case ${1:-} in
	--local | --user)
		xmake uninstall -P "$ROOT_DIR" --installdir="$HOME/.local" "$target"
		;;
	"")
		xmake uninstall -P "$ROOT_DIR" --installdir=/usr --admin "$target"
		;;
	*)
		die "usage: $0 [--local|--user]"
		;;
	esac
	remove_mandarin_dictionary
}

main "$@"
