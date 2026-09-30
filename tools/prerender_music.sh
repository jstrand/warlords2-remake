#!/bin/sh
# Record the MT-32 (M*.XMI) and Sound Canvas (R*.XMI) arrangements to
# pre-rendered-sound/<M|R><song>.ogg, for Game > MT-32 music and Sound Canvas
# music (love2d/sound.lua). Run from the repository root. Needs:
#
#   MUNT     mt32emu-smf2wav from github.com/munt/munt, and MT32_ROMS a folder
#            with an MT-32's control and PCM ROMs
#   EMU88    88EmuCli from 88emuPlayer, with SC-55 ROMs in its data folder
#   ffmpeg, oggenc (vorbis-tools), python3
#
# A recording's first sample is the song's tick 0: sound.lua loops it at the
# song's length. 88EmuCli starts 0.2 s late, which is cut; its SC-55 also
# sits 512 above zero and some 9 dB under the MT-32, which is taken out.
set -e
: "${MUNT:?set MUNT to mt32emu-smf2wav}" "${MT32_ROMS:?set MT32_ROMS}" "${EMU88:?set EMU88 to 88EmuCli}"
DATA=${1:-original}
OUT=pre-rendered-sound
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$OUT"
python3 tools/xmi2mid.py "$DATA/SOUND" "$TMP" M >/dev/null
python3 tools/xmi2mid.py "$DATA/SOUND" "$TMP" R >/dev/null
for mid in "$TMP"/M*.mid; do
  n=$(basename "$mid" .mid)
  "$MUNT" -f -m "$MT32_ROMS" -o "$TMP/$n.wav" "$mid" >/dev/null 2>&1
  oggenc -Q -q 5 -o "$OUT/$n.ogg" "$TMP/$n.wav"
done
for mid in "$TMP"/R*.mid; do
  n=$(basename "$mid" .mid)
  "$EMU88" --device sc55 --reset gs --bits 16 --quiet --overwrite --output "$TMP/$n.wav" "$mid" >/dev/null 2>&1
  ffmpeg -loglevel error -i "$TMP/$n.wav" \
    -af "atrim=start=0.2,asetpts=PTS-STARTPTS,dcshift=-0.015625,volume=9dB" -f wav - |
    oggenc -Q -q 5 -o "$OUT/$n.ogg" -
done
echo "$(ls "$OUT" | wc -l) recordings in $OUT"
