#!/bin/sh

# Define the path to the target directory
target_dir="$HOME/.local/gd-mandarin/"

# Check if the target directory exists, if not, create it
if [ ! -d "$target_dir" ]; then
    mkdir -p "$target_dir"
fi

# Define the path to the ZIP file you want to unzip
zip_file="path/to/your/file.zip"

# Unzip the file into the target directory
unzip "$zip_file" -d "$target_dir"

# Move the files to their specific locations
mv "$target_dir"char.bin "$target_dir"dicrc "$target_dir"matrix.bin "$target_dir"sys.dic "$target_dir"unk.dic "$target_dir"user.dic "$target_dir"
