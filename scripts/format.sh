#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR=$(  dirname -- "$(  readlink -f -- "$0"  )"  )
ROOT_DIR=$(  dirname -- "$ROOT_DIR" )
readonly ROOT_DIR

# Format project sources, including new files, without splitting filenames at whitespace or newlines.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed clang-format || die "clang-format is required"
	local -a source_files
	mapfile -d '' -t source_files < <(
		find src tests -type f \( -iname '*.cpp' -o -iname '*.h' -o -iname '*.hpp' -o -iname '*.cc' \) -print0
	)
	if ((${#source_files[@]} > 0)); then
		clang-format --sort-includes -i -- "${source_files[@]}"
	fi
}

main "$@"
