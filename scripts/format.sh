#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR=$(  dirname -- "$(  readlink -f -- "$0"  )"  )
ROOT_DIR=$(  dirname -- "$ROOT_DIR" )
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

if ! command -v clang-format >/dev/null 2>&1; then
    printf 'clang-format is required to format the C/C++ sources.\n' >&2
    exit 1
fi

readarray -t source_files <<<"$(
	find "$ROOT_DIR/tests" "$ROOT_DIR/src" -type f \
	\( -iname '*.cpp' -o -iname '*.h' -o -iname '*.hpp' -o -iname '*.cc' \)
)"
readonly -a source_files

if ((${#source_files[@]} > 0)); then
    clang-format --sort-includes -i -- "${source_files[@]}"
fi
