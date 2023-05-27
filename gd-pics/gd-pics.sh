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

query_bing() {
	curl_do -s -L -H "User-Agent: Mozilla/5.0" \
		-G --data-urlencode "q=$GDWORD" \
		--data-urlencode "mkt=ja-JP" \
		"https://www.bing.com/images/search" |
		LC_ALL=C grep -m 5 -oa '<img[^>]*class="mimg[^>]*>'
}

print_css() {
	cat <<-EOF
		<style>
		.gallery {
			display: grid;
			gap: 10px;
			margin: 0;
			justify-items: center;
			align-items: start;
			align-content: start;
			justify-content: space-between;
			grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
		}
		.gallery img {
			margin: 0 auto;
			max-width: 100%;
			width: 100%;
			border-radius: 5px;
			display: block;
			max-height: 95vh;
			object-fit: contain;
		}
		</style>
	EOF
}

echo '<div class="gallery">'
query_bing || true
echo '</div>'
print_css
