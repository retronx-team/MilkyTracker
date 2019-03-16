#!/bin/bash

set -eo pipefail

PLATFORM_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR="$PLATFORM_DIR/../.."
BUILD_DIR="$ROOT_DIR/build/nx"

source "$PLATFORM_DIR/_env.sh"

if [[ -z "$DEVKITPRO" ]]; then
	echo "env var DEVKITPRO is missing" 1>&2
	exit 1
fi

if [[ -z "$MAKE_OPTS" ]]; then
	MAKE_OPTS="-j$(nproc --all)"
fi

export SDL2DIR="$DEVKITPRO/portlibs/switch"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"
cmake "$ROOT_DIR" -DCMAKE_TOOLCHAIN_FILE="$PLATFORM_DIR/Toolchain.cmake"
make $MAKE_OPTS -- milkytracker dl

"$DEVKITPRO/tools/bin/nacptool" --create "$APP_TITLE" "$APP_AUTHOR" "$APP_VERSION" "$BUILD_DIR/milkytracker.nacp"
"$DEVKITPRO/tools/bin/elf2nro" "$BUILD_DIR/src/tracker/milkytracker" "$BUILD_DIR/milkytracker.nro" \
	--nacp="$BUILD_DIR/milkytracker.nacp" \
	--icon="$PLATFORM_DIR/icon.jpg"
