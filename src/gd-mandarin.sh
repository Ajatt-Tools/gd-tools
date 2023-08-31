#!/bin/bash
#
# gd-tools - a set of programs to enhance goldendict for immersion learning.
# Copyright (C) 2023 Ajatt-Tools
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set -euo pipefail

# Requires installing mecab
# Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add this script as type html.

TEMP_FILE=/tmp/gd-mandarin_cache

# Default path to user_dic.dic, assuming that the user ran `xmake install`.
USER_DICT=~/.local/gd-mandarin/user.dic

FONT_SIZE=2rem
GDWORD=""
GDSEARCH=""

sanitize_input() {
	sed -E \
		-e 's, +,,g' \
		-e 's,[a-z]+, ,Ig' \
		-e 's,\n, ,g'
}

usage() {
	local -r bn=$(basename -- "$0")
	cat <<-EOF
		usage: $bn [OPTIONS] --word %GDWORD% --sentence %GDSEARCH%

		echo input back to GoldenDict as HTML with sentence split into parts

		OPTIONS
	EOF
	column -t -s'|' <<-EOF
		  --user-dict PATH|path to the user dictionary.
		  --font-size SIZE|font size. The default value is 30px
	EOF
}

find_dicdir() {
	dirname -- "$(
		find \
			~/.local/gd-mandarin \
			/usr/lib/mecab/dic \
			~/.local/share/Anki2/addons21 \
			-type f -name dicrc -print -quit
	)"
}

mecab_split() {
	if ! command -v mecab &>/dev/null; then
		echo "Error: MeCab is not installed. Please install MeCab and try again."
		exit 1
	fi

	mecab -d . -Ogdmandarin	 --dicdir="$(find_dicdir)"	--userdic="${USER_DICT}"

}

print_css() {
	cat <<-EOF
		<style>
			.gd-mandarin {
				font-size: ${FONT_SIZE:-2rem};
				color: #1268c3;
				font-weight: normal;
			}
			.gd-mandarin a {
				display: inline-block;
				font-weight: normal;
				color: royalblue;
				text-decoration: none;
			}
			.gd-mandarin a:not(:last-of-type)::after {
				content: "";
				display: inline-block;
				background-color: #333;
				margin: 4px;
				width: 3px;
				height: 3px;
				border-radius: 100vmax;
				vertical-align: middle;
				cursor: text;
				user-select: text;
			}
			.gd-mandarin a b {
				background-color: #ddeeff;
				border-radius: 0.2rem;
				font-weight: 500;
			}
		</style>
	EOF
}

highlight_word() {
	local target_word=$* search=""
	search=$(cat -- -)
	search=${search//">$target_word<"/"><b>${target_word}</b><"}
	echo "$search"
}

die() {
	echo "$*" >&2
	notify-send "$(basename -- "$0")" "$*" &
	exit 1
}

main() {
	if (($# == 0)) || [[ $* == --help ]] || [[ $* == -h ]]; then
		usage
		exit
	fi
	while (($# > 0)); do
		case $1 in
		--font-size)
			shift
			FONT_SIZE=$1
			;;
		--user-dict)
			shift
			USER_DICT=$1
			;;
		--word)
			shift
			GDWORD=$1
			;;
		--sentence)
			shift
			GDSEARCH=$1
			;;
		*)
			die "Invalid argument passed."
			;;
		esac
		shift
	done

	if ! [[ -f $USER_DICT ]]; then
		die "Provided user dictionary doesn't exist or isn't a file."
	fi

	if [[ -z $GDSEARCH ]] || [[ -z $GDWORD ]]; then
		die "Not enough parameters."
	fi

	local -r input=$(echo "$GDSEARCH" | sanitize_input)
	local -r output=$(grep -Fxs -A1 "$input" -- "$TEMP_FILE")

	echo '<div class="gd-mandarin">'
	if [[ -n $output ]]; then
		echo "$output" |
			tail -1 |
			highlight_word "$GDWORD"
	else
		echo "$input" |
			tee -a "$TEMP_FILE" |
			mecab_split |
			tee -a "$TEMP_FILE" |
			highlight_word "$GDWORD"
		printf -- '\n\n' >>"$TEMP_FILE"
	fi
	echo '</div>'

	print_css
}

main "$@"
