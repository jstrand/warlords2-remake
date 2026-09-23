#!/usr/bin/env python3
"""Drive the original game under DOSBox-X and screenshot it.

The point is to have the original to compare the remake against, screen by
screen, without anyone at the keyboard. A script is a list of steps:

    key NAME        type one key (an AUTOTYPE name: a, enter, esc, f1, ...)
    click X Y [B]   click at (X, Y) in the game's 640x480; B is 1 left
                    (the default) or 2 right
    move X Y        put the pointer there and press nothing
    wait N          do nothing for N steps
    include FILE    another script's steps, here (scripts/ is searched)
    shot NAME       screenshot, saved as NAME.png

one per line, `#` for comments. Every step takes the same time (--pace), so
`wait` is how a slow screen is given longer.

Keys go in through DOSBox-X's own AUTOTYPE. Clicks and screenshots go through
W2HOOK.COM (w2hook.asm, beside this file), a small TSR that the script's F11
and F12 reach: it answers the game's INT 33h polls with the scripted pointer,
and asks DOSBox-X for a screenshot through its integration device.

    python3 tools/dosbox/shoot.py script.txt out/ [--data original]

The game is copied to a scratch directory first, so nothing it writes (saves,
settings) touches your copy.
"""

import argparse
import glob
import os
import shutil
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))

# The pointer defaults to the middle of the screen until a click moves it.
NOTHING = "f10"


def build_hook(work):
    com = os.path.join(work, "W2HOOK.COM")
    subprocess.run(["nasm", "-f", "bin", "-o", com,
                    os.path.join(HERE, "w2hook.asm")], check=True)
    return com


def patch_clicks(com, clicks):
    """Write the click table into the TSR: a u16 count and then x, y, buttons.
    The table is the last thing before the installer, which is laid out after
    the resident part; find it from the end of the resident code."""
    data = bytearray(open(com, "rb").read())
    # the table is 3 * 256 words of zeros directly after `clicks`; nasm puts
    # `clicks` immediately after the INT 33h handler's far jump (EA/2E FF 2E)
    table_len = 2 + 3 * 256 * 2
    # the installer is the code after the table; it begins with mov ax,3509h
    inst = data.rfind(bytes([0xB8, 0x09, 0x35]))
    if inst < 0:
        raise SystemExit("cannot find the installer in W2HOOK.COM")
    at = inst - table_len
    if len(clicks) > 256:
        raise SystemExit("at most 256 clicks")
    struct.pack_into("<H", data, at, len(clicks))
    for i, (x, y, b) in enumerate(clicks):
        struct.pack_into("<HHH", data, at + 2 + i * 6, x, y, b)
    open(com, "wb").write(data)


def parse(path, keys=None, clicks=None, shots=None):
    if keys is None:
        keys, clicks, shots = [], [], []
    for n, line in enumerate(open(path), 1):
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        w = line.split()
        op = w[0]
        if op == "include":
            # a name alone is one of the shared scripts in scripts/
            inc = w[1]
            if not os.path.exists(inc):
                inc = os.path.join(HERE, "scripts", w[1])
            parse(inc, keys, clicks, shots)
        elif op == "key":
            keys.extend(w[1:])
        elif op in ("click", "move"):
            b = 0 if op == "move" else (int(w[3]) if len(w) > 3 else 1)
            clicks.append((int(w[1]), int(w[2]), b))
            keys.append("f11")
        elif op == "wait":
            keys.extend([NOTHING] * int(w[1]))
        elif op == "shot":
            shots.append(w[1])
            keys.append("f12")
        else:
            raise SystemExit(f"{path}:{n}: unknown step {op!r}")
    return keys, clicks, shots


CONF = """\
[sdl]
output=surface
autolock=false
[dosbox]
machine=svga_s3
memsize=16
captures={captures}
[render]
scaler=none
aspect=false
[cpu]
core=dynamic
cputype=386
cycles={cycles}
# the port a DOS program reaches DOSBox-X through; it lives under [cpu]
integration device=true
[mixer]
nosound=true
[speaker]
pcspeaker=false
[sblaster]
sbtype=none
[gus]
gus=false
[dos]
xms=true
ems=true
umb=true
[autoexec]
mount c "{game}"
c:
W2HOOK
AUTOTYPE -w {wait} -p {pace} {keys}
warlord2 n v
exit
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("script")
    ap.add_argument("out")
    ap.add_argument("--data", default="original")
    ap.add_argument("--pace", type=float, default=0.6, help="seconds per step")
    ap.add_argument("--wait", type=float, default=8, help="seconds before the first step")
    ap.add_argument("--cycles", default="fixed 30000")
    ap.add_argument("--limit", type=float, default=0,
                    help="real seconds to let DOSBox-X run (default: from the script)")
    ap.add_argument("--keep", action="store_true", help="keep the scratch copy")
    a = ap.parse_args()

    keys, clicks, shots = parse(a.script)
    os.makedirs(a.out, exist_ok=True)

    work = tempfile.mkdtemp(prefix="w2shoot-")
    game = os.path.join(work, "game")
    caps = os.path.join(work, "capture")
    shutil.copytree(a.data, game)
    os.makedirs(caps)
    com = build_hook(work)
    patch_clicks(com, clicks)
    shutil.copy(com, os.path.join(game, "W2HOOK.COM"))

    conf = os.path.join(work, "dosbox-x.conf")
    with open(conf, "w") as f:
        f.write(CONF.format(captures=caps, game=game, cycles=a.cycles,
                            wait=a.wait, pace=a.pace, keys=" ".join(keys)))

    limit = a.limit or int((a.wait + a.pace * len(keys)) * 4 + 20)
    # -time-limit is not honoured once the game is running, so keep time here
    proc = subprocess.Popen(["dosbox-x", "-silent", "-nopromptfolder", "-conf", conf],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    # stop as soon as the last screenshot is in, and the file written
    import time
    start, done = time.time(), None
    while proc.poll() is None and time.time() - start < limit:
        n = len(glob.glob(os.path.join(caps, "*.png")))
        if n >= len(shots) and done is None:
            done = time.time()
        if done is not None and time.time() - done > 1:
            break
        time.sleep(0.25)
    if proc.poll() is None:
        proc.kill()
        proc.wait()
    print(f"ran {time.time() - start:.0f}s")

    got = sorted(glob.glob(os.path.join(caps, "*.png")), key=os.path.getmtime)
    for name, src in zip(shots, got):
        shutil.copy(src, os.path.join(a.out, name + ".png"))
    print(f"{len(got)} of {len(shots)} screenshots -> {a.out}")
    if len(got) != len(shots):
        print("  some were not taken: is the game still loading? try --wait",
              file=sys.stderr)
    if a.keep:
        print("scratch copy kept at", work)
    else:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    main()
