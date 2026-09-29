#!/bin/sh
set -eu

here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
lib="$here/libgfn_cursor_arrow.so"
app=com.nvidia.geforcenow

if flatpak info --user "$app" >/dev/null 2>&1; then
    scope=--user
elif flatpak info --system "$app" >/dev/null 2>&1; then
    scope=--system
else
    echo "GeForce NOW Flatpak ist nicht installiert." >&2
    exit 1
fi

if [ ! -f "$lib" ] || [ "$here/cursor_arrow.c" -nt "$lib" ]; then
    tmp="$lib.$$"
    trap 'rm -f "$tmp"' EXIT HUP INT TERM
    cc -shared -fPIC -O2 -Wall -Wextra -o "$tmp" "$here/cursor_arrow.c" -ldl -lXcursor -lX11
    mv -f "$tmp" "$lib"
    trap - EXIT HUP INT TERM
fi

# NVIDIA's /app/bin/GeForceNOW wrapper explicitly clears LD_PRELOAD. Start the
# actual client from its required working directory so the shim reaches it.
exec flatpak run "$scope" --command=sh "$app" \
    -c 'lib=$1; mode=$2; shift 2; cd /app/cef && exec env LD_PRELOAD="$lib" GFN_CURSOR_MODE="$mode" ./GeForceNOW "$@"' \
    gfn-cursor-workaround "$lib" "${GFN_CURSOR_MODE:-xcursor}" "$@"
