#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(git rev-parse --show-toplevel)" || exit 1
readonly ROOT_DIR
cd -- "$ROOT_DIR" || exit 1

# Define the path to the target directory
target_dir="$HOME/.local/gd-mandarin/"

# Check if the target directory exists, if not, create it
if [ ! -d "$target_dir" ]; then
    mkdir -p "$target_dir"
fi

res_files="res/mandarin_dict"
cp -- "$res_files"/* "$target_dir"
