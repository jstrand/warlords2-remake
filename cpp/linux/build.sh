#!/bin/sh
# Builds the C++ port for x86_64 Linux, in Valve's Steam Runtime 3 "sniper"
# SDK, and packs it with the game's files:
#
#   sh cpp/linux/build.sh            # from anywhere; needs Docker (or DOCKER=podman)
#
# Out comes cpp/build/linux/warlords2-linux-x86_64.tar.gz. Unpack it anywhere
# writable and run warlords2.sh (see cpp/linux/README.md).
#
# The binary is built against sniper's glibc (2.31), so it runs on any
# distribution from about 2020 on, the Steam Deck's SteamOS among them. It
# links libstdc++ statically and carries its own SDL2 and Vorbis in lib/.
# SDL2 is built here from source, because the SDK's own links the runtime's
# X11, Wayland and PulseAudio: built as SDL builds by default it loads those
# when it starts, from whatever the system has, and needs only libc to load
# at all.
set -eu

here=$(cd "$(dirname "$0")" && pwd)
repo=$(cd "$here/../.." && pwd)
out="$repo/cpp/build/linux"
image=registry.gitlab.steamos.cloud/steamrt/sniper/sdk:latest
sdl=SDL2-2.32.10
sdlsum=5f5993c530f084535c65a6879e9b26ad441169b3e25d789d83287040a9ca5165
name=warlords2-linux-x86_64

if [ "${1:-}" != "--inside" ]; then
  mkdir -p "$out"
  if [ ! -f "$out/$sdl.tar.gz" ]; then
    curl -fL -o "$out/$sdl.tar.gz.part" \
      "https://github.com/libsdl-org/SDL/releases/download/release-${sdl#SDL2-}/$sdl.tar.gz"
    mv "$out/$sdl.tar.gz.part" "$out/$sdl.tar.gz"
  fi
  if command -v sha256sum >/dev/null; then sum="sha256sum"; else sum="shasum -a 256"; fi
  echo "$sdlsum  $out/$sdl.tar.gz" | $sum -c -
  # The game's files, as committed (not the working tree's leftovers).
  (cd "$repo" && git archive --format=tar -o "$out/data.tar" HEAD original pre-rendered-sound)
  ${DOCKER:-docker} run --rm --platform linux/amd64 \
    -u "$(id -u):$(id -g)" -e HOME=/tmp \
    -v "$repo:/src" -w /src "$image" sh cpp/linux/build.sh --inside
  rm -f "$out/data.tar"
  echo "built $out/$name.tar.gz"
  exit 0
fi

# ---------------------------------------------------------------- in sniper

out=/src/cpp/build/linux
obj="$out/obj"
pkg="$out/Warlords2"
sdlprefix="$out/sdl"

# SDL2, once. X11, Wayland, PulseAudio, PipeWire, ALSA and the rest are
# loaded at run time (SDL's default), not linked.
if [ ! -f "$sdlprefix/lib/libSDL2-2.0.so.0" ]; then
  rm -rf "$out/sdl-src" "$out/sdl-obj"
  mkdir -p "$out/sdl-src"
  tar -xzf "$out/$sdl.tar.gz" -C "$out/sdl-src" --strip-components=1
  cmake -S "$out/sdl-src" -B "$out/sdl-obj" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$sdlprefix" -DCMAKE_INSTALL_LIBDIR=lib \
    -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST=OFF
  cmake --build "$out/sdl-obj" -j"$(nproc)"
  cmake --install "$out/sdl-obj"
fi

# The newest g++ the SDK carries: C++20 wants more than its default gcc 10.
cxx=
for v in 14 13 12; do
  if command -v "g++-$v" >/dev/null; then cxx="g++-$v"; break; fi
done
[ -n "$cxx" ] || { echo "no g++ 12 or newer in the SDK" >&2; exit 1; }
echo "compiler: $cxx"

# RPATH, not RUNPATH (--disable-new-dtags), so that the bundled libraries'
# own needs (libvorbisfile's libvorbis) are looked for in lib/ too.
cmake -S /src/cpp -B "$obj" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="$cxx" \
  -DSDL2_DIR="$sdlprefix/lib/cmake/SDL2" \
  -DCMAKE_EXE_LINKER_FLAGS="-static-libstdc++ -static-libgcc -Wl,--disable-new-dtags" \
  -DCMAKE_BUILD_RPATH_USE_ORIGIN=ON \
  -DCMAKE_BUILD_RPATH='$ORIGIN/lib'
cmake --build "$obj" -j"$(nproc)" --target warlords2 w2test

# The rules suite, as a check that the build is sound.
(cd /src && "$obj/w2test")

rm -rf "$pkg"
mkdir -p "$pkg/lib"
cp "$obj/warlords2" "$pkg/"
strip "$pkg/warlords2"
# SDL2 and the Vorbis libraries go along; everything else is the base system.
ldd "$obj/warlords2" | awk '/libSDL2|libvorbis|libogg/ { print $3 }' | while read -r lib; do
  cp -L "$lib" "$pkg/lib/"
done
case "$(ldd "$obj/warlords2" | grep libSDL2)" in
  *"$sdlprefix"*) ;;
  *) echo "linked the SDK's SDL2, not the one built here" >&2; exit 1 ;;
esac
cp /src/cpp/linux/warlords2.sh "$pkg/"
chmod +x "$pkg/warlords2.sh"
cp /src/cpp/linux/README.md "$pkg/README-linux.md"
tar -xf "$out/data.tar" -C "$pkg"
echo "--- bundled:"; ls -l "$pkg/lib"
echo "--- needs:"; ldd "$pkg/warlords2"
echo "--- SDL2 needs:"; ldd "$pkg/lib/libSDL2-2.0.so.0"
echo "--- newest glibc asked for:"
for f in "$pkg/warlords2" "$pkg"/lib/*; do objdump -T "$f"; done |
  grep -o 'GLIBC_[0-9.]*' | sort -uV | tail -1
tar -C "$out" -czf "$out/$name.tar.gz" Warlords2
