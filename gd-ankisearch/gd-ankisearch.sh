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

FIELD_NAME=""
DECK_NAME=""
GDWORD=""

ankiconnect_request() {
	curl -fsS '127.0.0.1:8765' -X POST -d "$*"
}

escape_quotes() {
	local text=$*
	text=${text//'"'/'\"'}
	echo "$text"
}

find_notes() {
	local query=$GDWORD
	local request='{
	    "action": "findNotes",
	    "version": 6,
	    "params": {
		"query": "<query>"
	    }
	}'
	if [[ -n $FIELD_NAME ]]; then
		query="\"$FIELD_NAME:*$query*\""
	fi
	if [[ -n $DECK_NAME ]]; then
		query="\"deck:$DECK_NAME\" $query"
	fi
	ankiconnect_request "${request//<query>/$(escape_quotes "$query")}"
}

filter_nids() {
	grep -Po '[0-9]+'
}

print_nids() {
	echo "<ol>"
	find_notes | filter_nids | while read -r nid; do
		printf -- '<li><a href="ankisearch:nid:%s">%s</a></li>\n' "$nid" "$nid"
	done
	echo "</ol>"
}

usage() {
	local -r bn=$(basename -- "$0")
	cat <<-EOF
		usage: $bn [OPTIONS] WORD

		Search you Anki collection and output Note Ids that match query.

		OPTIONS
	EOF
	column -t -s'|' <<-EOF
		  --field-name NAME|optional field to limit search to.
		  --deck-name NAME|optional deck to limit search to.
	EOF
	cat <<-EOF

		EXAMPLES

		$bn --field-name VocabKanji %GDWORD%
		$bn --deck-name Mining %GDWORD%
	EOF
}

main() {
	if (($# == 0)) || [[ $* == --help ]] || [[ $* == -h ]]; then
		usage
		exit
	fi
	while (($# > 0)); do
		case $1 in
		--field-name)
			shift
			FIELD_NAME=$1
			;;
		--deck-name)
			shift
			DECK_NAME=$1
			;;
		*)
			GDWORD="${GDWORD}$1"
			;;
		esac
		shift
	done
	print_nids
}

main "$@"
