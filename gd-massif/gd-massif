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
		li {
			font-size: 1.5rem;
			list-style: none;
		}
		.result-meta {
			font-size: 12px;
		}
		a {
			color: royalblue;
		}
		em {
			font-style: normal;
			background-color: #ddeeff;
			border-radius: 0.2rem;
		}
		</style>
	EOF
}

main() {
	echo '<ul>'
	fetch_sentences || true
	echo '</ul>'
	print_css
}

main
