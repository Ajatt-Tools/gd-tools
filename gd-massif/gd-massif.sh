#!/bin/bash

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
