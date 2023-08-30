#!/bin/bash

# This script can be used instead of `make install`.

set -xeuo pipefail

readonly target="gd-tools"

xmake f --tests=n
xmake f -m release
xmake build -vwy "$target"

run_mandarin_script=false

# Parse optional flags
while [[ $# -gt 0 ]]; do
    case $1 in
    --local | --user)
        xmake install -v --all --installdir=~/.local/ "$target"
        shift
        ;;
    --mandarin)
        run_mandarin_script=true
        shift
        ;;
    *)
        xmake install -v --all --installdir=/usr --admin "$target"
        shift
        ;;
    esac
done

if $run_mandarin_script; then
    ./mandarin_installer.sh
fi
