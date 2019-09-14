#!/bin/bash

set -eo pipefail

PLATFORM_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR="$PLATFORM_DIR/../.."
BUILD_DIR="$ROOT_DIR/build/nx"
DIST_DIR="$BUILD_DIR/dist"

source "$PLATFORM_DIR/_env.sh"

if [[ -z "$DEVKITPRO" ]]; then
	echo "env var DEVKITPRO is missing" 1>&2
	exit 1
fi

if [[ -z "$MAKE_OPTS" ]]; then
	MAKE_OPTS="-j$(nproc --all)"
fi

source "$DEVKITPRO/switchvars.sh"
CFLAGS="$CFLAGS -g -I$PORTLIBS_PREFIX/include -L$PORTLIBS_PREFIX/lib -D__SWITCH__ -I$DEVKITPRO/libnx/include"

export SDL2DIR="$DEVKITPRO/portlibs/switch"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"
cmake -G"Unix Makefiles" "$ROOT_DIR" \
	-DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
	-DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/switch.cmake" \
	-DCMAKE_C_FLAGS="$CFLAGS $CPPFLAGS $DEFINES $INCLUDES" \
	-DCMAKE_CXX_FLAGS="$CFLAGS $DEFINES $INCLUDES" \
	-DCMAKE_AR="$DEVKITPRO/devkitA64/bin/aarch64-none-elf-gcc-ar"
make $MAKE_OPTS -- milkytracker dl

"$DEVKITPRO/tools/bin/nacptool" --create "$APP_TITLE" "$APP_AUTHOR" "$APP_VERSION" "$BUILD_DIR/milkytracker.nacp"
"$DEVKITPRO/tools/bin/elf2nro" "$BUILD_DIR/src/tracker/milkytracker" "$BUILD_DIR/milkytracker.nro" \
	--nacp="$BUILD_DIR/milkytracker.nacp" \
	--icon="$PLATFORM_DIR/icon.jpg"

if [[ "$1" == "dist" ]]; then
	DIST_FOLDER="${APP_TITLE}_${APP_VERSION}"
	mkdir -p "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE"
	cp -f "$BUILD_DIR/milkytracker.nro" "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE/milkytracker.nro"

	cd "$ROOT_DIR/docs"
	mkdir -p "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE/docs"
	for i in *; do
		if [[ "$i" != "CMakeLists.txt" ]]; then
			cp -f "$ROOT_DIR/docs/$i" "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE/docs/$i"
		fi
	done

	cp -f "$PLATFORM_DIR/README.md" "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE/docs/README-NX.md"

	cd "$ROOT_DIR/resources/music"
	mkdir -p "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE/examples"
	for i in *; do
		if [[ "$i" != "CMakeLists.txt" ]]; then
			cp -f  "$ROOT_DIR/resources/music/$i" "$BUILD_DIR/$DIST_FOLDER/$APP_TITLE/examples/$i"
		fi
	done

	cd "$BUILD_DIR/$DIST_FOLDER"
	zip -r "$DIST_FOLDER.zip" "$APP_TITLE"
	mv -f "$BUILD_DIR/$DIST_FOLDER/$DIST_FOLDER.zip" "$BUILD_DIR/$DIST_FOLDER.zip"
	rm -rf "$BUILD_DIR/$DIST_FOLDER"
fi