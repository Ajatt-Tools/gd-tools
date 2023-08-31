#!/bin/sh

# Define the path to the target directory
target_dir="$HOME/.local/gd-mandarin/"

# Check if the target directory exists, if not, create it
if [ ! -d "$target_dir" ]; then
    mkdir -p "$target_dir"
fi

res_files="res/mandarin_dict"
cp -- "$res_files"/* "$target_dir"
