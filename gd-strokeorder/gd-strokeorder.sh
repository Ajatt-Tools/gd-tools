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

MAX_LEN=5
FONT_SIZE=10rem
GDWORD=""

print_stroke_order() {
	# Don't output anything for empty or long queries.
	if [[ -z $GDWORD ]] || [[ ${#GDWORD} -gt "$MAX_LEN" ]]; then
		exit
	fi

	cat <<-EOF
		<div class="kanji_stroke_order">$GDWORD</div>
		<style>
		.kanji_stroke_order {
			font-size: $FONT_SIZE;
			font-family: KanjiStrokeOrders;
		}
		</style>
	EOF
}

usage() {
	local -r bn=$(basename -- "$0")
	cat <<-EOF
		usage: $bn [OPTIONS] WORD

		echo input back to GoldenDict as HTML with the KanjiStrokeOrders font applied.

		OPTIONS
	EOF
	column -t -s'|' <<-EOF
		  --max-len NUMBER|maximum length of the input string.
		  --font-size SIZE|default font size.
	EOF
	cat <<-EOF

		EXAMPLES
		  $bn --max-len 5 -font-size 10rem "書"
		  $bn --max-len 3 -font-size 120px "薔薇"
	EOF
}

main() {
	if (($# == 0)) || [[ $* == --help ]] || [[ $* == -h ]]; then
		usage
		exit
	fi
	while (($# > 0)); do
		case $1 in
		--max-len)
			shift
			MAX_LEN=$1
			;;
		--font-size)
			shift
			FONT_SIZE=$1
			;;
		*)
			GDWORD="$GDWORD $1"
			;;
		esac
		shift
	done
	GDWORD=$(sed 's/^ *//g; s/ *$//g' <<<"$GDWORD")
	print_stroke_order
}

main "$@"
