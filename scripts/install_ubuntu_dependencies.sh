#!/usr/bin/env bash

set -euo pipefail

packages=(
	build-essential
	g++-14
	libcurl4-openssl-dev
	libmecab-dev
	mecab-ipadic-utf8
	mecab-utils
)

case ${1:-} in
--packaging)
	packages+=(debhelper devscripts)
	;;
"")
	;;
*)
	printf -- 'usage: %s [--packaging]\n' "$0" >&2
	exit 1
	;;
esac

sudo apt-get update
sudo apt-get install --yes "${packages[@]}"
