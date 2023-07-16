#!/bin/bash

# This script can be used instead of `make install`.

set -euo pipefail

xmake f -m release
xmake build -vwy
xmake install -v --all --installdir=/usr --admin
