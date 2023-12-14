#!/bin/bash

# This script can be used instead of `make install`.

set -xeuo pipefail

readonly target="gd-tools"

xmake config --tests=n
xmake config -m release
xmake build -vwy "$target"

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

args=(xmake install -v --all)

if ${local_install:-false}; then
	"${args[@]}" --installdir=~/.local/ "$target"
else
	"${args[@]}" --installdir=/usr --admin "$target"
fi

if $run_mandarin_script; then
	./mandarin_installer.sh
fi
