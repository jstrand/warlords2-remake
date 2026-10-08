#!/bin/sh
# Play the same all-computer games in the Lua remake and in the JS port and
# compare them turn by turn. Run from the repository root:
#     sh web/test/compare.sh [scenario seed turns [hidden|diplo|save]]
set -u
tmp=${TMPDIR:-/tmp}/w2compare.$$
mkdir -p "$tmp"
fail=0
run() {
  luajit web/test/trace.lua "$@" > "$tmp/lua.txt" 2>&1
  node web/test/trace.js "$@" > "$tmp/js.txt" 2>&1
  if cmp -s "$tmp/lua.txt" "$tmp/js.txt"; then
    echo "same  $* ($(wc -l < "$tmp/lua.txt" | tr -d ' ') turns)"
  else
    echo "DIFF  $*"
    diff "$tmp/lua.txt" "$tmp/js.txt" | head -6
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
  run ISLADIA 3 40 diplo
  run ERYTHEA 4 60 diplo
  run DRAGON 6 40 diplo hidden
  run RANDOM 3 40
  run RANDOM 8 30 diplo
fi
rm -rf "$tmp"
exit $fail
