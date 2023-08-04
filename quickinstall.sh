#!/bin/bash

# This script can be used instead of `make install`.

set -xeuo pipefail

readonly target="gd-tools"

xmake f --tests=n
xmake f -m release
xmake build -vwy "$target"

case ${1-} in
--local | --user)
    xmake install -v --all --installdir=~/.local/ "$target"
    ;;
*)
    xmake install -v --all --installdir=/usr --admin "$target"
    ;;
esac
