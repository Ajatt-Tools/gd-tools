#!/bin/bash

set -xeuo pipefail

xmake f -m release
xmake f --tests=y
xmake build -w tests
xmake run tests
