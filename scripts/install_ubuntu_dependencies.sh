#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

readonly packages=(
	build-essential
	g++-14
	libcurl4-openssl-dev
	libmecab-dev
	mecab-ipadic-utf8
	mecab-utils
)

# Install Ubuntu build dependencies, optionally including Debian packaging tools.
main() {
	case ${1:-} in
	--packaging)
		packages+=(debhelper devscripts)
		;;
	"")
		;;
	*)
		die "usage: $0 [--packaging]"
		;;
	esac
	is_installed sudo || die "sudo is required"
	is_installed apt-get || die "apt-get is required"
	sudo apt-get update
	sudo apt-get install --yes "${packages[@]}"
}

main "$@"
