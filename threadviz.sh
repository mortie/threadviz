#!/bin/sh
set -e

threadvizso='@LIBDIR@/libthreadviz.so'
vizhtml='@DATAROOTDIR@/threadviz/visualize.html'

generate_html=
use_browser=

while [ $# -gt 0 ]; do
	case "$1" in
		--html)
			if [ -z "$2" ]; then
				echo "--html requires an argument."
				exit 1
			fi

			generate_html="$2"
			shift 2
			;;

		--browser)
			use_browser="${BROWSER:-firefox}"
			shift
			;;

		-*)
			echo "Unknown option: $1"
			exit 1
			;;

		*)
			break
	esac
done

if ! [ -z "$generate_html" ]; then
	dataline="$(grep --line-number "@DATA@" "$vizhtml" | cut -d':' -f1)"
	beginhtml="$(head -n $((dataline - 1)) "$vizhtml")"
	endhtml="$(tail -n +$((dataline + 1)) "$vizhtml")"
	printf "%s" "$beginhtml" > "$generate_html.html"
	cat "$generate_html" >> "$generate_html.html"
	printf "%s" "$endhtml" >> "$generate_html.html"

	echo "HTML: $generate_html.html"
	if ! [ -z "$use_browser" ]; then
		$use_browser "$generate_html.html"
	fi
	exit 0
fi

if [ -z "$1" ]; then
	echo "Usage: $0 <command...>"
	echo "Or:    LD_PRELOAD=$threadvizso <command...>"
	echo "       $0 --html <file>"
	exit 1
fi

LD_PRELOAD="$threadvizso" "$@"
