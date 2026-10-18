#!/usr/bin/env bash

set -euo pipefail

# shellcheck source=scripts/lib.sh
. "$(dirname -- "$0")/lib.sh"

# Install Ubuntu build dependencies, optionally including Debian packaging tools.
main() {
	local -a packages=(
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
		die "usage: $0 [--packaging]"
		;;
	esac
	# Freeze the completed list only after optional packaging dependencies have been added.
	readonly -a packages
	is_installed sudo || die "sudo is required"
	is_installed apt-get || die "apt-get is required"
	sudo apt-get update
	sudo apt-get install --yes "${packages[@]}"
}

main "$@"
