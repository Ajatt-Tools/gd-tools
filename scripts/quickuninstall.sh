#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

readonly target=gd-tools

case ${1:-} in
--local | --user)
    xmake uninstall -P "$ROOT_DIR" --installdir="$HOME/.local" "$target"
    ;;
"")
    xmake uninstall -P "$ROOT_DIR" --installdir=/usr --admin "$target"
    ;;
*)
    printf 'usage: %s [--local|--user]\n' "$0" >&2
    exit 2
    ;;
esac
