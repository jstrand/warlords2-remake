#!/bin/sh
# Builds the C++ port for x86_64 Windows with MinGW-w64's gcc, in an MSYS2
# UCRT64 shell, and packs it with the game's files:
#
#   pacman -S --needed git zip mingw-w64-ucrt-x86_64-{gcc,cmake,ninja,pkgconf,SDL2,libvorbis}
#   sh cpp/windows/build.sh          # from anywhere, in the UCRT64 shell
#
# Out comes cpp/build/windows/warlords2-windows-x86_64.zip. Unpack it anywhere
# writable and run warlords2.exe (see cpp/windows/README.md).
#
# The game links against the Universal C Runtime, which Windows 10 and 11
# carry. Every other DLL it needs (SDL2, the Vorbis libraries, gcc's own)
# goes beside it from the UCRT64 environment.

set -eu

[ "${MSYSTEM:-}" = UCRT64 ] || { echo "run this in MSYS2's UCRT64 shell" >&2; exit 1; }

here=$(cd "$(dirname "$0")" && pwd)
repo=$(cd "$here/../.." && pwd)
out="$repo/cpp/build/windows"
obj="$out/obj"
pkg="$out/Warlords2"
name=warlords2-windows-x86_64

cmake -S "$repo/cpp" -B "$obj" -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build "$obj" --target warlords2 w2test

# The rules suite, as a check that the build is sound.
(cd "$repo" && "$obj/w2test.exe")

rm -rf "$pkg" "$out/$name.zip"
mkdir -p "$pkg"
cp "$obj/warlords2.exe" "$pkg/"
strip "$pkg/warlords2.exe"
# The DLLs it loads from the UCRT64 environment, and theirs; Windows' own
# stay behind.
ldd "$pkg/warlords2.exe" | awk 'tolower($3) ~ /^\/ucrt64\// { print $3 }' | sort -u | while read -r dll; do
  cp "$dll" "$pkg/"
done
cp "$here/README.md" "$pkg/README-windows.txt"
# The game's files, as committed (not the working tree's leftovers).
(cd "$repo" && git archive --format=tar HEAD original pre-rendered-sound) | tar -xf - -C "$pkg"
echo "--- bundled:"; ls -l "$pkg"/*.dll
echo "--- needs:"; ldd "$pkg/warlords2.exe"
(cd "$out" && zip -qr "$name.zip" Warlords2)
echo "built $out/$name.zip"
