"""Warlords II scenario files: .SCN, .SGN, .SPC, .CTY.

Every shipped .SCN is exactly 12001 bytes -- a fixed layout with fixed-size
arrays, so unused slots are simply zero. Offsets below are absolute.

.SCN layout
-----------
       0  8 x 20-byte side names ("Sirians", "Stone Giants", ...)
     160  scenario configuration -- mostly undecoded
     387  8 x 20-byte SIDE records: u16 index, u16 starting gold, u16 zero,
          u16 capital x, u16 capital y, then 10 zero bytes
    2063  u16 site count (<= 40)
    2065  40 x 31-byte SITE records   (temples and ruins)
    3305  22 x 29-byte ITEM records   (8 standards + 14 magic items)
    3943  10 x 16-byte MONSTER records (slot 0 blank, then 9 ruin guardians)
    4103  unused / not yet decoded (almost entirely zero)
    5499  u16 city count (<= 80)
    5501  80 x 65-byte CITY records
   10701  trailing zeros to end of file

SITE record (31 bytes)
     +0  u16     x
     +2  u16     y
     +4  char[20] name
    +24  u16     type: 1 = Temple, 2 = Ruin
                 (indexes STRING.DAT group 113, whose [1] is 'Type: Temple')

ITEM record (29 bytes) -- no coordinates; items are carried or hidden in ruins
     +0  char[20] name
    +20  u8      type / effect id (cf. STRING.DAT group 167)
    +21  u8      value (strength of the effect)
    Records 8..21 are overwritten at game start from the scenario's .ITM file.

MONSTER record (16 bytes)
     +0  char[12] name

CITY record (65 bytes)
     +0  u16     x
     +2  u16     y
     +4  char[16] name
    +20  23 bytes of per-city data -- production, owner, defence (NOT DECODED)
    +43  zero padding

.SGN -- signposts
    u16 count, then count x 104-byte records:
     +0  u16     x
     +2  u16     y
     +4  char[100] text, NUL-terminated
                 The tail after the NUL is uninitialised memory: you can read
                 leftover strings like "Erythea\\Erythea.spc" and "Road" in it.

.SPC / .CTY -- plain text, "#NNN|line|line|\\r\\n" per entry, indexed by the
site / city id. .SPC always holds 40 entries; .CTY holds one per city.

Verification
------------
Positions are confirmed against the map, not assumed. Every record's (x, y)
lands on its own dedicated terrain tile, and the counts match exactly:

    signposts -> tile 0     cities -> tile 96
    sites     -> tile 10 (temple art) or 12 (ruins art)

For all six shipped scenarios the number of tile-0 cells equals the sign count
and the number of tile-96 cells equals the city count. The site *type* field is
independent of which art the map uses: every type-1 site is named "...Temple"
and no type-2 site is.
"""
import re
import struct

SCN_SIZE = 12001
SIDE_NAMES, N_SIDES, SIDE_STRIDE = 0, 8, 20
SIDE_RECS, SIDE_REC_STRIDE = 387, 20
SITES_COUNT, SITES, N_SITES, SITE_STRIDE = 2063, 2065, 40, 31
ITEMS, N_ITEMS, ITEM_STRIDE = 3305, 22, 29
MONSTERS, N_MONSTERS, MONSTER_STRIDE = 3943, 10, 16
CITIES_COUNT, CITIES, MAX_CITIES, CITY_STRIDE = 5499, 5501, 80, 65
SGN_STRIDE = 104

SITE_TYPES = {1: 'Temple', 2: 'Ruin'}


def _s(b):
    return b.split(b'\0')[0].decode('latin1')


def load_scn(path):
    with open(path, 'rb') as f:
        d = f.read()
    if len(d) != SCN_SIZE:
        raise ValueError(f"{path}: {len(d)} bytes, expected {SCN_SIZE}")

    sides = []
    for i in range(N_SIDES):
        o = SIDE_RECS + SIDE_REC_STRIDE * i
        idx, gold, _z, cx, cy = struct.unpack_from('<5H', d, o)
        sides.append(dict(
            index=i,
            name=_s(d[SIDE_NAMES + SIDE_STRIDE * i:SIDE_NAMES + SIDE_STRIDE * (i + 1)]),
            gold=gold,
            capital=(cx, cy),
        ))

    nsites = struct.unpack_from('<H', d, SITES_COUNT)[0]
    sites = []
    for i in range(nsites):
        o = SITES + SITE_STRIDE * i
        x, y = struct.unpack_from('<HH', d, o)
        ty = struct.unpack_from('<H', d, o + 24)[0]
        sites.append(dict(index=i, x=x, y=y, name=_s(d[o + 4:o + 24]),
                          type=ty, type_name=SITE_TYPES.get(ty, f'?{ty}')))

    items = []
    for i in range(N_ITEMS):
        o = ITEMS + ITEM_STRIDE * i
        name = _s(d[o:o + 20])
        if name:
            items.append(dict(index=i, name=name, type=d[o + 20], value=d[o + 21]))

    monsters = []
    for i in range(N_MONSTERS):
        o = MONSTERS + MONSTER_STRIDE * i
        name = _s(d[o:o + 12])
        if name:
            monsters.append(dict(index=i, name=name))

    ncities = struct.unpack_from('<H', d, CITIES_COUNT)[0]
    cities = []
    for i in range(ncities):
        o = CITIES + CITY_STRIDE * i
        x, y = struct.unpack_from('<HH', d, o)
        cities.append(dict(index=i, x=x, y=y, name=_s(d[o + 4:o + 20]),
                           data=d[o + 20:o + 43]))

    return dict(sides=sides, sites=sites, items=items,
                monsters=monsters, cities=cities)


def load_sgn(path):
    with open(path, 'rb') as f:
        d = f.read()
    n = struct.unpack_from('<H', d, 0)[0]
    if len(d) != 2 + SGN_STRIDE * n:
        raise ValueError(f"{path}: {len(d)} bytes for {n} signs")
    out = []
    for i in range(n):
        o = 2 + SGN_STRIDE * i
        x, y = struct.unpack_from('<HH', d, o)
        out.append(dict(index=i, x=x, y=y, text=_s(d[o + 4:o + SGN_STRIDE])))
    return out


def load_descriptions(path):
    """Parse .CTY / .SPC: '#NNN|line|line|' per entry. Returns {id: [lines]}."""
    with open(path, 'r', errors='replace') as f:
        text = f.read()
    out = {}
    for m in re.finditer(r'#(\d+)\|([^\r\n]*)', text):
        out[int(m.group(1))] = [s for s in m.group(2).split('|') if s]
    return out


if __name__ == '__main__':
    import os
    import sys
    scn = sys.argv[1]
    base = os.path.splitext(scn)[0]
    s = load_scn(scn)
    signs = load_sgn(base + '.SGN')
    cty = load_descriptions(base + '.CTY')
    spc = load_descriptions(base + '.SPC')
    print(f"{os.path.basename(base)}: {len(s['sides'])} sides, {len(s['cities'])} cities, "
          f"{len(s['sites'])} sites, {len(signs)} signs, {len(s['items'])} items")
    at = {(c['x'], c['y']): c['name'] for c in s['cities']}
    print("\nsides:")
    for sd in s['sides']:
        print(f"  {sd['index']} {sd['name']:<16} gold={sd['gold']:>4} "
              f"capital={at.get(sd['capital'], '-')}")
    print("\ncities:")
    for c in s['cities'][:10]:
        d = cty.get(c['index'], [])
        print(f"  {c['index']:>2} ({c['x']:>3},{c['y']:>3}) {c['name']:<16} {d[0] if d else ''}")
    print("\nsites:")
    for t in s['sites'][:10]:
        d = spc.get(t['index'], [])
        print(f"  {t['index']:>2} ({t['x']:>3},{t['y']:>3}) {t['type_name']:<7} "
              f"{t['name']:<18} {d[0] if d else ''}")
    print("\nsigns:")
    for g in signs[:6]:
        print(f"  ({g['x']:>3},{g['y']:>3}) {g['text']!r}")
