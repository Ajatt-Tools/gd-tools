#!/usr/bin/env bash

# This script can be used instead of `make install`.

set -xeuo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

this_dir=$(dirname -- "$(readlink -f -- "$0")")
readonly this_dir

readonly target="gd-tools"

xmake config -P "$ROOT_DIR" -y -m release --tests=n
xmake build -P "$ROOT_DIR" -vwy "$target"

run_mandarin_script=false

# Parse optional flags
while [[ $# -gt 0 ]]; do
    case $1 in
    --local | --user)
	readonly local_install=true
        shift
        ;;
    --mandarin)
        readonly run_mandarin_script=true
        shift
        ;;
    *)
        shift
        ;;
    esac
done

args=(xmake install -P "$ROOT_DIR" -v --all)

if ${local_install:-false}; then
	"${args[@]}" --installdir=~/.local/ "$target"
else
	"${args[@]}" --installdir=/usr --admin "$target"
fi

if $run_mandarin_script; then
	"$this_dir/mandarin_installer.sh"
fi
