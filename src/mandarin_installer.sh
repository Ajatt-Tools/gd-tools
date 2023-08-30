#!/bin/sh

# Define the path to the target directory
target_dir="$HOME/.local/gd-mandarin/"

# Check if the target directory exists, if not, create it
if [ ! -d "$target_dir" ]; then
    mkdir -p "$target_dir"
fi

zip_file="res/gd-mandarin_dict/gd-mandarin.zip"

unzip  -j "$zip_file" -d "$target_dir"
