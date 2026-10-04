#!/bin/sh
# Play the same all-computer games in the Lua remake and in the C++ port and
# compare them turn by turn. Run from the repository root, after building
# into cpp/build:
#     sh cpp/test/compare.sh [scenario seed turns [hidden|save]]
set -u
tmp=${TMPDIR:-/tmp}/w2compare.$$
mkdir -p "$tmp"
fail=0
run() {
  luajit cpp/test/trace.lua "$@" > "$tmp/lua.txt" 2>&1
  cpp/build/w2trace "$@" > "$tmp/cpp.txt" 2>&1
  if cmp -s "$tmp/lua.txt" "$tmp/cpp.txt"; then
    echo "same  $* ($(wc -l < "$tmp/lua.txt" | tr -d ' ') turns)"
  else
    echo "DIFF  $*"
    diff "$tmp/lua.txt" "$tmp/cpp.txt" | head -6
    fail=1
  fi
}
if [ $# -gt 0 ]; then
  run "$@"
else
  run ERYTHEA 1 60
  run ISLADIA 7 60
  run DRAGON 3 50
  run HADESHA 11 50
  run SORCERY 5 50
  run TUTORIA 2 40
  run ERYTHEA 9 50 hidden
  run ISLADIA 4 50 hidden
  run ERYTHEA 3 40 save
fi
rm -rf "$tmp"
exit $fail
