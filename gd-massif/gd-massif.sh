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

readonly CONNECT_TIMEOUT=2
readonly MAX_TIME=5
readonly GDWORD=$*

curl_do() {
	curl \
		--connect-timeout "$CONNECT_TIMEOUT" \
		--max-time "$MAX_TIME" \
		"$@"
}

fetch_sentences() {
	curl_do \
		-s "https://massif.la/ja/search?q=$GDWORD" |
		grep -F '<li class="text-japanese">' -A 3 --no-group-separator
}

print_css() {
	cat <<-EOF
		<style>
		.gd-massif {
			padding-inline-start: 0;
			margin-inline-start: 0;
		}
		.gd-massif li {
			font-size: 1.5rem;
			list-style: none;
		}
		.gd-massif .result-meta {
			font-size: 12px;
		}
		.gd-massif a {
			color: royalblue;
		}
		.gd-massif em {
			font-style: normal;
			background-color: #ddeeff;
			border-radius: 0.2rem;
		}
		</style>
	EOF
}

main() {
	echo '<ul class="gd-massif">'
	fetch_sentences || true
	echo '</ul>'
	print_css
}

main
