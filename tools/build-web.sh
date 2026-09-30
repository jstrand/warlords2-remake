#!/bin/sh
# Build the game for the browser with love.js (LÖVE compiled to WebAssembly):
#
#   tools/build-web.sh            # -> build/web/, then serve it:
#   npx http-server build/web     #    and open http://localhost:8080
#
# The .love is laid out as the repository is, so the paths the game reads by
# (original/..., pre-rendered-sound/...) are the same in the browser; web.lua
# puts io.open on love.filesystem there. The browser's build has no threads,
# so no FM music: only the MT-32 recordings go in (SYNTHS to change that,
# e.g. SYNTHS="M R" for the Sound Canvas's too, at twice the download).
#
# Needs Node.js (for npx) and a zip: zip, or Windows' own tar.exe.
set -e
cd "$(dirname "$0")/.."

SYNTHS=${SYNTHS:-M}
OUT=build/web
STAGE=build/web-stage
LOVE=build/warlords2.love

rm -rf "$STAGE" "$OUT" "$LOVE"
mkdir -p "$STAGE/pre-rendered-sound"

# the engine at the root, without its tests
(cd love2d && tar cf - --exclude=./test .) | (cd "$STAGE" && tar xf -)
cp -r original "$STAGE/original"
for s in $SYNTHS; do cp pre-rendered-sound/"$s"*.ogg "$STAGE/pre-rendered-sound/"; done

if command -v zip >/dev/null 2>&1; then
  (cd "$STAGE" && zip -qr -0 ../warlords2.love .)
else
  # bsdtar, which writes zips; stored, as the music is compressed already
  (cd "$STAGE" && /c/Windows/System32/tar.exe --format zip --options zip:compression=store -cf ../warlords2.love *)
fi

# compatibility mode (-c): no threads, and so no special headers from the
# server; memory for the game on top of the data it is given
# (run with node: npx from Git Bash exits without a word)
npm install --silent --no-save --prefix build/tools love.js@11.4.1 http-server@14
node build/tools/node_modules/love.js/index.js -c -t "Warlords II" -m 536870912 "$LOVE" "$OUT"

# love.js copies the save directory (saves, prefs) from memory to IndexedDB
# only as the page unloads, which a closed tab or a crash never gets to: sync
# it every two seconds too, and whenever the page is hidden
node -e '
const fs = require("fs"), f = process.argv[1];
const from = "window.addEventListener(\"beforeunload\",";
const to = "(function(){var busy=false;function sync(){if(busy)return;busy=true;"
  + "FS.syncfs(false,function(err){busy=false;if(err)Module[\"printErr\"](err)})}"
  + "setInterval(sync,2000);document.addEventListener(\"visibilitychange\",function(){"
  + "if(document.visibilityState===\"hidden\")sync()})})();" + from;
const s = fs.readFileSync(f, "utf8");
if (!s.includes(from)) { console.error("build-web: love.js has changed, sync not patched"); process.exit(1); }
fs.writeFileSync(f, s.replace(from, to));
' "$OUT/love.js"
echo "built $OUT: serve it with any static server (node build/tools/node_modules/http-server/bin/http-server $OUT) and open it"
