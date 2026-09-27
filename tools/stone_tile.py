"""Rebuild the main screen's stone ground as one 244x220 tile, for screens
bigger than the original's 640x480.

    python3 tools/stone_tile.py [original_dir] [out.lua] [preview_dir]

The stone around the original's panels is a 244x220 tile, repeated from the
screen's own (0, 0): the band along the top (under the menu bar too), the band
along the bottom and the columns at either side are exact copies of it (every
border pixel agrees with its neighbour 244 across or 220 down). The border only
ever shows slices of it -- 27 rows along the top, 8 along the bottom, 13
columns at each side -- and the gutters between the panels are painted
separately, so the rest of the tile is nowhere in the game's files.

This fills the missing part by patch quilting from the screen's own stone:
every 8x8 cell of the tile takes the patch of stone whose surroundings best
agree with what is already known around the cell, so the seams fall in the
dither where they do not show. The pixels one out from each panel's black
outline are a bevel, not stone, and are never used.

What it writes is not art but a list of rects -- where each piece of the tile
comes from on the original's 640x480 screen -- so the game builds the tile at
run time from SCREEN0-3.PCK (love2d/warlords/screen.lua). Seeded, so a rerun
writes the same file.
"""
import os
import random
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pck import load as load_pck, write_png
from pal import load as load_pal

TW, TH = 244, 220
CELL, MARGIN = 8, 3

# The main screen's panels, outline included: map, strategic map, control
# panel, bottom bar. Each is ringed by a one-pixel bevel.
PANELS = [(15, 29, 362, 362), (399, 29, 226, 314), (399, 354, 226, 116),
          (15, 402, 362, 68)]

# The tile's slices the border shows whole, as (tx, ty, w, h, sx, sy): the
# rows above the bevels at the top, the rows under them at the bottom, and
# the columns outside them at either side. The screen's own edge is a
# frame, not stone: light along the top and left, dark along the bottom and
# right. Where the gutters meet the border their painting runs a row or two
# into it, so each slice is taken from where it does not.
KNOWN = [
    (0, 1, TW, 27, TW, 1),
    (0, 32, TW, 7, TW, 472),
    (0, 31, 151, 1, 2 * TW, 471),
    (151, 31, 93, 1, 151, 471),
    (1, 18, 13, TH - 18, 1, 18),
    (1, 0, 13, 18, 1, TH),
    (138, 18, 13, TH - 18, 626, 18),
    (138, 0, 13, 18, 626, TH),
]


def screen_pixels(src):
    full = np.zeros((480, 640), np.int16)
    for i in range(4):
        w, h, px = load_pck(os.path.join(src, 'PICS', f'SCREEN{i}.PCK'))
        qx, qy = (i % 2) * 320, (i // 2) * 240
        full[qy:qy + h, qx:qx + w] = np.frombuffer(bytes(px), np.uint8).reshape(h, w)
    return full


def stone_mask():
    """The screen's stone: everything that is not the screen's frame, a
    panel, its outline or its bevel."""
    m = np.zeros((480, 640), bool)
    m[1:479, 1:639] = True
    for x, y, w, h in PANELS:
        m[y - 1:y + h + 1, x - 1:x + w + 1] = False
    return m


def build(src, seed=1):
    full = screen_pixels(src)
    stone = stone_mask()
    tile = np.full((TH, TW), -1, np.int16)
    rects = []
    for tx, ty, w, h, sx, sy in KNOWN:
        assert stone[sy:sy + h, sx:sx + w].all()
        piece = full[sy:sy + h, sx:sx + w]
        old = tile[ty:ty + h, tx:tx + w]
        assert ((old < 0) | (old == piece)).all(), 'the border is not one tile'
        tile[ty:ty + h, tx:tx + w] = piece
        rects.append((tx, ty, w, h, sx, sy))

    # The unknown part, as bands of rows by bands of columns, each cut into
    # cells, taken top to bottom from under a known band so every cell has
    # known stone above it; row 0 comes last, between the wrap and the
    # top band.
    rows = [(28, 3), (39, TH - 39), (0, 1)]
    cols = [(14, 124), (151, 93), (0, 1)]
    cells = []
    for ry, rh in rows:
        for y in range(0, rh, CELL):
            for cx, cw in cols:
                for x in range(0, cw, CELL):
                    cells.append((cx + x, ry + y, min(CELL, cw - x), min(CELL, rh - y)))

    rng = random.Random(seed)
    used = {}
    for cx, cy, w, h in cells:
        ww, wh = w + 2 * MARGIN, h + 2 * MARGIN
        ys = [(cy - MARGIN + j) % TH for j in range(wh)]
        xs = [(cx - MARGIN + i) % TW for i in range(ww)]
        want = tile[np.ix_(ys, xs)]
        known = want >= 0
        # every window of the screen's stone this size, on the same checker
        # parity as the cell, so the dither carries straight across
        ok = np.lib.stride_tricks.sliding_window_view(stone, (wh, ww)).all(axis=(2, 3))
        oy, ox = np.nonzero(ok)
        par = ((ox + MARGIN + oy + MARGIN) - (cx + cy)) % 2 == 0
        oy, ox = oy[par], ox[par]
        wins = np.lib.stride_tricks.sliding_window_view(full, (wh, ww))[oy, ox]
        err = (np.abs(wins - want) * known).sum(axis=(1, 2)).astype(float)
        # a patch used before costs a little, so the tile does not repeat
        # itself inside its own 244x220
        for k, (a, b) in enumerate(zip(oy, ox)):
            err[k] += used.get((a, b), 0) * 4
        best = err.min()
        pick = [k for k in range(len(err)) if err[k] <= best * 1.1 + 1]
        k = rng.choice(pick)
        sx, sy = int(ox[k]) + MARGIN, int(oy[k]) + MARGIN
        used[(oy[k], ox[k])] = used.get((oy[k], ox[k]), 0) + 1
        tile[cy:cy + h, cx:cx + w] = full[sy:sy + h, sx:sx + w]
        rects.append((cx, cy, w, h, sx, sy))

    assert (tile >= 0).all()
    return full, tile, rects


def write_lua(path, rects):
    with open(path, 'w') as f:
        f.write('-- Generated by tools/stone_tile.py -- do not edit.\n')
        f.write('--\n')
        f.write('-- The main screen\'s 244x220 stone tile, as the rects of the original\'s\n')
        f.write('-- 640x480 screen it is made from: { tx, ty, w, h, sx, sy }, drawn in order.\n')
        f.write(f'return {{\n  w = {TW}, h = {TH},\n')
        for r in rects:
            f.write('  { %d, %d, %d, %d, %d, %d },\n' % r)
        f.write('}\n')


def main(src='original', out='love2d/warlords/stonetile.lua', preview=None):
    full, tile, rects = build(src)
    write_lua(out, rects)
    print(f'{len(rects)} rects -> {out}')
    if preview:
        pal = load_pal(os.path.join(src, 'STANDARD.PAL'))
        big = np.tile(tile, (3, 3)).astype(np.uint8)
        write_png(os.path.join(preview, 'stonetile.png'), big.shape[1], big.shape[0],
                  big.tobytes(), pal)


if __name__ == '__main__':
    main(*sys.argv[1:])
