#!/usr/bin/env bash
# Builds the website into dist/: compiles the Doom engine (engine/, GPL-2.0) to
# WebAssembly and copies the page, icons and licences next to it.
# Requires Emscripten 3.1.74 on PATH (source emsdk_env.sh first).
# Single-threaded build: no pthreads/SharedArrayBuffer, so no special server headers.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OBJ="$ROOT/build/obj"
OUT="$ROOT/dist"
mkdir -p "$OBJ"
cd "$ROOT/engine"
make -f Makefile.web -j"$(nproc 2>/dev/null || echo 4)" OBJDIR="$OBJ" OUTPUT="$ROOT/build/doomgeneric" \
  CFLAGS="-O2 -DFEATURE_SOUND -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=[] -Wno-everything -Werror=implicit-function-declaration -Wincompatible-function-pointer-types" \
  SDL_CFLAGS="-sUSE_SDL=2" \
  SDL_LIBS="-sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=[] -lidbfs.js" \
  LDFLAGS="-O2 -sALLOW_MEMORY_GROWTH=1 -sINVOKE_RUN=0 -sEXIT_RUNTIME=0 -sEXPORTED_RUNTIME_METHODS=callMain,FS,IDBFS -sENVIRONMENT=web"
rm -rf "$OUT"
mkdir -p "$OUT"
cp "$ROOT"/build/doomgeneric.js "$ROOT"/build/doomgeneric.wasm "$OUT/"
cp "$ROOT"/web/* "$OUT/"
cp "$ROOT/LICENSE" "$OUT/LICENSE.txt"
echo "Website ready in $OUT"
