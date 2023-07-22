#!/bin/bash

# This script can be used instead of `make install`.

set -xeuo pipefail

xmake f -m release
xmake build -vwy

case ${1-} in
--local | --user)
    xmake install -v --all --installdir=~/.local/
    ;;
*)
    xmake install -v --all --installdir=/usr --admin
    ;;
esac
