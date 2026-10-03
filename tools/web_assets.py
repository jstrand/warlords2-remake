"""Convert the original game's files into what a browser can load, for web/.

    python3 tools/web_assets.py [original] [web/assets]

Run from the repository root. Writes, under the output directory:

  data/<path>.png   every .PCK -- and the fonts' .FNT, which are .PCKs too -- as
                    an 8-bit greyscale PNG whose grey is the colour index times
                    17, named <original name>.png (A0.PCK.png); the JS reads
                    the indices back, so palettes, colour keys and remaps work
                    as they do in Lua
  data/<path>.wav   every .8SN, unsigned 8-bit mono at 11000 Hz
  data/<path>       every other data file the remake reads, byte for byte
  music/S*.ogg      the AdLib songs, rendered through the remake's own OPL
                    (tools/xmi2wav.lua) -- skipped when already there
  music/M*.ogg, R*  the MT-32 and Sound Canvas recordings, copied
  manifest.json     { files: [...], images: {path: [w, h]}, songs: {name: s} }

Paths in the manifest keep the original's spelling, so `TERRAIN0/A0.PCK` is
still asked for by that name; the JS swaps the extension.
"""
import json
import os
import shutil
import subprocess
import sys
import wave
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pck import load as load_pck  # noqa: E402

from PIL import Image  # noqa: E402

# what the browser never needs: programs, drivers, the installer's leftovers
SKIP_EXT = {'.EXE', '.COM', '.ADV', '.AD', '.BAT', '.INI', '.HTML', '.XMI', '.ME'}
SKIP_FILES = {'SWAP.DAT', 'SOUND.DAT', 'VER102.DAT', 'CURRENT.HST', 'CURRENT.SGN'}


def convert_pck(src, dst):
    w, h, px = load_pck(src)
    img = Image.frombytes('L', (w, h), bytes(v * 17 for v in px))
    img.save(dst, optimize=True)
    return w, h


def convert_8sn(src, dst):
    with open(src, 'rb') as f:
        data = f.read()
    with wave.open(dst, 'wb') as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(11000)
        w.writeframes(data)


def song_lengths(orig, names):
    """Each song's length in seconds, as sound.lua works it out: the tick
    after its last event, at XMIDI's 120 a second."""
    paths = [os.path.join(orig, 'SOUND', n + '.XMI') for n in names]
    out = subprocess.run(['luajit', 'tools/xmi_lengths.lua'] + paths, capture_output=True,
                         text=True, check=True).stdout
    lengths = {}
    for line in out.splitlines():
        p, secs = line.split('\t')
        lengths[os.path.basename(p)[:-4]] = float(secs)
    return lengths


# what a recording runs on past its song's end, for the notes to die away; a
# song with a loop in it never ends by itself, so its play-through is cut here
TAIL = 4


def render_song(orig, name, dst, length):
    if os.path.exists(dst):
        return
    tmp = dst[:-4] + '.wav'
    subprocess.run(['luajit', 'tools/xmi2wav.lua', name, tmp, str(length + TAIL), orig],
                   check=True, capture_output=True)
    subprocess.run(['oggenc', '-Q', '-q', '4', '-o', dst, tmp], check=True)
    os.remove(tmp)


def main():
    orig = sys.argv[1] if len(sys.argv) > 1 else 'original'
    out = sys.argv[2] if len(sys.argv) > 2 else 'web/assets'
    data = os.path.join(out, 'data')
    music = os.path.join(out, 'music')
    os.makedirs(data, exist_ok=True)
    os.makedirs(music, exist_ok=True)

    files, images = [], {}
    for root, _, names in os.walk(orig):
        for n in sorted(names):
            src = os.path.join(root, n)
            rel = os.path.relpath(src, orig).replace(os.sep, '/')
            ext = os.path.splitext(n)[1].upper()
            if ext in SKIP_EXT or n.upper() in SKIP_FILES or n.startswith('.'):
                continue
            if rel.upper().startswith(('SAVE/', 'START/', 'RANDOM/')):
                continue
            os.makedirs(os.path.join(data, os.path.dirname(rel)), exist_ok=True)
            if ext in ('.PCK', '.FNT'):
                images[rel] = convert_pck(src, os.path.join(data, rel + '.png'))
            elif ext == '.8SN':
                convert_8sn(src, os.path.join(data, rel[:-4] + '.wav'))
            else:
                shutil.copyfile(src, os.path.join(data, rel))
            files.append(rel)

    # the songs: the FM set rendered, the Roland sets copied where recorded
    snd = os.path.join(orig, 'SOUND')
    fm = sorted(n[:-4] for n in os.listdir(snd) if n.upper().startswith('S')
                and n.upper().endswith('.XMI'))
    fm_lengths = song_lengths(orig, fm)
    with ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
        list(pool.map(lambda n: render_song(orig, n, os.path.join(music, n + '.ogg'),
                                            fm_lengths[n]), fm))
    rec = 'pre-rendered-sound'
    recorded = []
    if os.path.isdir(rec):
        for n in sorted(os.listdir(rec)):
            if n.endswith('.ogg'):
                shutil.copyfile(os.path.join(rec, n), os.path.join(music, n))
                recorded.append(n[:-4])
    songs = song_lengths(orig, fm + [n for n in recorded
                                     if os.path.exists(os.path.join(snd, n + '.XMI'))])
    available = sorted(n[:-4] for n in os.listdir(music) if n.endswith('.ogg'))

    with open(os.path.join(out, 'manifest.json'), 'w') as f:
        json.dump({'files': sorted(files), 'images': images, 'songs': songs,
                   'music': available}, f, indent=0, sort_keys=True)
    print(f'{len(files)} files, {len(images)} images, {len(available)} songs')


if __name__ == '__main__':
    main()
