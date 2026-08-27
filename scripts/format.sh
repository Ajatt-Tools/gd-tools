#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

if ! command -v clang-format >/dev/null 2>&1; then
    printf 'clang-format is required to format the C/C++ sources.\n' >&2
    exit 127
fi

# Restrict formatting to tracked files so generated and dependency sources remain untouched.
mapfile -d '' -t source_files < <(git ls-files -z -- '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hh' '*.hpp' '*.hxx')
readonly source_files

if ((${#source_files[@]} > 0)); then
    clang-format --sort-includes -i -- "${source_files[@]}"
fi
