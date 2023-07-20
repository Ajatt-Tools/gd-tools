#!/bin/bash

# This script can be used instead of `make install`.

set -xeuo pipefail

xmake f -m release
xmake build -vwy
xmake install -v --all --installdir=~/.local/
