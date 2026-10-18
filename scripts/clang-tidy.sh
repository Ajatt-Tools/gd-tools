#!/usr/bin/env bash
# Copyright: Ajatt-Tools and contributors; https://github.com/Ajatt-Tools
# License: GNU AGPL, version 3 or later; http://www.gnu.org/licenses/agpl.html

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR

# Analyze project sources, or repository-relative filenames supplied as arguments, without editing them.
main() {
	cd -- "$ROOT_DIR" || die "can't enter project root"
	is_installed clang-tidy || die "clang-tidy is required"
	[[ -f build/compile_commands.json ]] || die "build/compile_commands.json not found. Configure and build with Xmake first"

	local -a source_files=("$@")
	if (($# == 0)); then
		# Enable tests in Xmake before scanning both directories. Headers are analyzed through their source files.
		mapfile -d '' -t source_files < <(
			find src tests -type f \( -iname '*.cpp' -o -iname '*.cc' -o -iname '*.cxx' \) -print0
		)
	fi
	readonly -a source_files
	((${#source_files[@]} > 0)) || die "no C++ source files found"

	# GCC-only warning flags must not stop Clang. Never apply fixes automatically to non-const-iterable range views.
	exec clang-tidy -p build --extra-arg=-Wno-unknown-warning-option "${source_files[@]}"
}

main "$@"
