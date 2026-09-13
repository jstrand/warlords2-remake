"""Render a Warlords II .MAP to PNG using the terrain tile sheets.

.MAP is a flat grid of u16 tile references, 112 x 156 = 17472 tiles (every
shipped map is this size). The low 15 bits are a tile index; bit 15 (0x8000) is
a flag some maps set (see docs/formats/map.md).

The width is confirmed by the terrain checkerboard: tiles 9 and 31 are variants
that alternate strictly by (x + y) parity, which only resolves cleanly at 112.

The sibling .RD file is the road/overlay layer: exactly one byte per tile,
0 = nothing, otherwise an index into ROAD.PCK (also 40x40 cells, colour-keyed
on its corner pixel). Rendered on top, these form connected road networks
running between cities, which is what confirms the interpretation.

Tiles come from TERRAIN0/SCENERY0.PCK and SCENERY1.PCK, each 640x240 = a 16x6
grid of 40x40 cells, so 96 tiles per sheet and 192 total:

    sheet = index // 96      cell = index % 96
    col   = cell % 16        row  = cell // 16
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pal import load as load_pal
from pck import load as load_pck, write_png

MAP_W, MAP_H = 112, 156
CELL = 40
SHEET_COLS, SHEET_TILES = 16, 96
TILE_MASK = 0x7FFF


def load_map(path):
    with open(path, 'rb') as f:
        d = f.read()
    n = len(d) // 2
    if n != MAP_W * MAP_H:
        raise ValueError(f"{path}: {n} tiles, expected {MAP_W * MAP_H}")
    return struct.unpack(f'<{n}H', d)


def render(map_path, terrain_dir, scale=CELL, roads=True):
    tiles = load_map(map_path)
    rd_path = os.path.splitext(map_path)[0] + '.RD'
    rd = open(rd_path, 'rb').read() if roads and os.path.exists(rd_path) else None
    sheets = [load_pck(os.path.join(terrain_dir, f'SCENERY{i}.PCK')) for i in (0, 1)]
    step = CELL // scale if scale < CELL else 1
    out_w, out_h = MAP_W * scale, MAP_H * scale

    # pre-slice every tile once, at the requested scale
    cache = {}
    for t in set(tiles):
        idx = t & TILE_MASK
        sw, sh, spx = sheets[min(idx // SHEET_TILES, 1)]
        cell = idx % SHEET_TILES
        sx, sy = (cell % SHEET_COLS) * CELL, (cell // SHEET_COLS) * CELL
        rows = []
        for y in range(0, CELL, step):
            row = spx[(sy + y) * sw + sx:(sy + y) * sw + sx + CELL]
            rows.append(row[::step] if step > 1 else row)
        cache[t] = rows

    px = bytearray(out_w * out_h)
    for my in range(MAP_H):
        base = my * MAP_W
        for sy in range(scale):
            o = (my * scale + sy) * out_w
            for mx in range(MAP_W):
                px[o:o + scale] = cache[tiles[base + mx]][sy]
                o += scale

    if rd:
        rw, rh, rpx = load_pck(os.path.join(terrain_dir, 'ROAD.PCK'))
        key = rpx[0]                       # ROAD.PCK is colour-keyed on its corner pixel
        rcache = {}
        for i in set(rd):
            if not i:
                continue
            sx, sy = (i % SHEET_COLS) * CELL, (i // SHEET_COLS) * CELL
            rows = []
            for y in range(0, CELL, step):
                row = rpx[(sy + y) * rw + sx:(sy + y) * rw + sx + CELL]
                rows.append(row[::step] if step > 1 else row)
            rcache[i] = rows
        for my in range(MAP_H):
            for mx in range(MAP_W):
                i = rd[my * MAP_W + mx]
                if not i:
                    continue
                for sy in range(scale):
                    row = rcache[i][sy]
                    o = (my * scale + sy) * out_w + mx * scale
                    for k in range(scale):
                        if row[k] != key:
                            px[o + k] = row[k]
    return out_w, out_h, bytes(px)


if __name__ == '__main__':
    map_path = sys.argv[1]
    out = sys.argv[2]
    scale = int(sys.argv[3]) if len(sys.argv) > 3 else CELL
    terrain = 'original/TERRAIN0'
    pal = load_pal(os.path.join(terrain, 'WAR2.PAL'))
    w, h, px = render(map_path, terrain, scale)
    os.makedirs(os.path.dirname(out) or '.', exist_ok=True)
    write_png(out, w, h, px, pal)
    print(f"{map_path} -> {out}  {w}x{h} ({scale}px/tile)")
