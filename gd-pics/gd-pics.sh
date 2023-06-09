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
