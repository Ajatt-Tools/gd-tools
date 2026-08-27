#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(dirname -- "$(readlink -f -- "$0")")/.."
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

readonly target=gd-tools
readonly mandarin_dir="$HOME/.local/gd-mandarin"

case ${1:-} in
--local | --user)
    xmake uninstall -P "$ROOT_DIR" --installdir="$HOME/.local" "$target"
    ;;
"")
    xmake uninstall -P "$ROOT_DIR" --installdir=/usr --admin "$target"
    ;;
*)
    printf 'usage: %s [--local|--user]\n' "$0" >&2
    exit 1
    ;;
esac

for source_file in res/mandarin_dict/*; do
    rm -f -- "$mandarin_dir/${source_file##*/}"
done
if [[ -d "$mandarin_dir" ]]; then
    rmdir --ignore-fail-on-non-empty -- "$mandarin_dir"
fi
