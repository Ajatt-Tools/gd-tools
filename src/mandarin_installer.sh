#!/bin/sh

# Define the path to the target directory
target_dir="$HOME/.local/gd-mandarin/"

# Check if the target directory exists, if not, create it
if [ ! -d "$target_dir" ]; then
    mkdir -p "$target_dir"
fi

zip_file="res/gd-mandarin_dict/gd-mandarin.zip"

unzip "$zip_file" -d "$target_dir"

# Move the files to their specific locations
mv "$target_dir"char.bin "$target_dir"dicrc "$target_dir"matrix.bin "$target_dir"sys.dic "$target_dir"unk.dic "$target_dir"user.dic "$target_dir"
