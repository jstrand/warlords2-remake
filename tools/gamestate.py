"""Headless Warlords II scenario loader.

Composes every decoded format into a single starting game state, and validates
the pieces against each other. This is the check that the file formats in
docs/formats/ actually fit together into something playable -- no rendering,
no UI, no rules engine.

    python3 tools/gamestate.py original/ERYTHEA
    python3 tools/gamestate.py --all

What it builds
--------------
    sides     8 slots: name, starting gold, capital, in_use
    cities    position, name, income, production types, derived owner
    sites     temples and ruins with position, type and description text
    signs     signpost positions and text
    items     standards and magic items
    armies    the 29 army types with strength / time / cost
    tiles     terrain grid + road overlay, with a derived terrain class

Two values are DERIVED, not stored (see docs/rules.md):
  * ownership -- at scenario start each side owns exactly its capital
  * defence   -- 1 if a city produces fewer than 3 army types, else 2

Terrain classes are also derived -- by the dominant palette colour of each tile
in SCENERY0/1.PCK, plus the four tiles whose meaning is pinned by the scenario
data (signpost / temple / ruins / city). The game's own tile-to-terrain table
has not been located; `MAPCOLOR.DAT` is referenced by FILE.DAT but ships with
no copy. Treat `terrain_class` as a convenience, not as authority.
"""
import collections
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import armytype
import mapren
import scenario
import string_dat
from pal import load as load_pal
from pck import load as load_pck

MAP_W, MAP_H = mapren.MAP_W, mapren.MAP_H
NEUTRAL = None

# tiles whose meaning is fixed by scenario data (verified 1:1 against record counts)
SPECIAL_TILES = {0: 'signpost', 10: 'temple', 12: 'ruins', 96: 'city'}

# dominant palette index -> terrain class (WAR2.PAL)
COLOUR_CLASS = {
    5: 'water', 6: 'water',
    11: 'plain', 10: 'plain', 43: 'plain',
    12: 'forest',
    1: 'mountain', 2: 'mountain', 3: 'mountain', 4: 'mountain',
}


def classify_terrain(terrain_dir):
    """tile index -> terrain class, by dominant palette colour. Heuristic."""
    sheets = [load_pck(os.path.join(terrain_dir, f'SCENERY{i}.PCK')) for i in (0, 1)]
    out = {}
    for t in range(192):
        if t in SPECIAL_TILES:
            out[t] = SPECIAL_TILES[t]
            continue
        sw, sh, spx = sheets[t // 96]
        c = t % 96
        sx, sy = (c % 16) * mapren.CELL, (c // 16) * mapren.CELL
        hist = collections.Counter(
            spx[(sy + y) * sw + sx + x] for y in range(mapren.CELL) for x in range(mapren.CELL))
        out[t] = COLOUR_CLASS.get(hist.most_common(1)[0][0], 'other')
    return out


def load_game(scenario_dir, terrain_dir='original/TERRAIN0', data_dir='original/DATA'):
    name = os.path.basename(os.path.normpath(scenario_dir))
    base = os.path.join(scenario_dir, name)

    scn = scenario.load_scn(base + '.SCN')
    signs = scenario.load_sgn(base + '.SGN')
    city_text = scenario.load_descriptions(base + '.CTY')
    site_text = scenario.load_descriptions(base + '.SPC')
    armies = armytype.load(os.path.join(terrain_dir, 'ARMYTYPE.DAT'))
    strings = string_dat.load(os.path.join(data_dir, 'STRING.DAT'))

    tiles = mapren.load_map(base + '.MAP')
    with open(base + '.RD', 'rb') as f:
        roads = f.read()
    terrain = classify_terrain(terrain_dir)

    by_id = {a['sprite']: a for a in armies}

    # sides: a slot is in use if its capital resolves to a real city
    at = {(c['x'], c['y']): c for c in scn['cities']}
    sides = []
    for s in scn['sides']:
        cap = at.get(s['capital'])
        sides.append(dict(s, in_use=cap is not None,
                          capital_city=cap['name'] if cap else None))

    # ownership is derived: a side owns only its capital at start
    owner_of = {}
    for i, s in enumerate(sides):
        if s['in_use']:
            owner_of[s['capital']] = i

    cities = []
    for c in scn['cities']:
        owner = owner_of.get((c['x'], c['y']), NEUTRAL)
        cities.append(dict(
            index=c['index'], name=c['name'], x=c['x'], y=c['y'],
            income=c['data'][22],
            produces=[by_id[t]['name'] for t in c['data'][2:6] if t != 255],
            produce_ids=[t for t in c['data'][2:6] if t != 255],
            defence=2 if sum(1 for t in c['data'][2:6] if t != 255) >= 3 else 1,
            owner=owner,
            owner_name=sides[owner]['name'] if owner is not None else 'Neutral',
            is_capital=owner is not None,
            description=city_text.get(c['index'], []),
        ))

    sites = [dict(s, description=site_text.get(s['index'], [])) for s in scn['sites']]

    return dict(name=name, sides=sides, cities=cities, sites=sites, signs=signs,
                items=scn['items'], monsters=scn['monsters'], armies=armies,
                tiles=tiles, roads=roads, terrain=terrain, strings=strings,
                width=MAP_W, height=MAP_H)


def validate(g):
    """Cross-check the loaded state. Returns a list of (ok, message)."""
    out = []
    T = g['terrain']

    def tile_at(x, y):
        return g['tiles'][y * MAP_W + x] & mapren.TILE_MASK

    out.append((len(g['tiles']) == MAP_W * MAP_H,
                f"map is {MAP_W}x{MAP_H} = {len(g['tiles'])} tiles"))
    out.append((len(g['roads']) == MAP_W * MAP_H,
                f"road overlay is 1 byte/tile ({len(g['roads'])})"))

    bad = [c for c in g['cities'] if T.get(tile_at(c['x'], c['y'])) != 'city']
    out.append((not bad, f"all {len(g['cities'])} cities stand on a city tile"))

    bad = [s for s in g['sites'] if T.get(tile_at(s['x'], s['y'])) not in ('temple', 'ruins')]
    out.append((not bad, f"all {len(g['sites'])} sites stand on a temple/ruins tile"))

    bad = [s for s in g['signs'] if T.get(tile_at(s['x'], s['y'])) != 'signpost']
    out.append((not bad, f"all {len(g['signs'])} signs stand on a signpost tile"))

    caps = [s for s in g['sides'] if s['in_use']]
    owned = [c for c in g['cities'] if c['owner'] is not None]
    out.append((len(caps) == len(owned),
                f"{len(caps)} sides in use, {len(owned)} cities owned at start "
                f"(1 each), {len(g['cities']) - len(owned)} neutral"))

    dist = collections.Counter(c['defence'] for c in g['cities'])
    out.append((set(dist) <= {1, 2},
                f"city defence derived from production slots: "
                + ", ".join(f"{v} cities at {k}" for k, v in sorted(dist.items()))))

    ids = {a['sprite'] for a in g['armies']}
    bad = [c for c in g['cities'] for t in c['produce_ids'] if t not in ids]
    out.append((not bad, "every city production slot names a real army type"))

    unknown = {t for t in set(g['tiles']) if T.get(t & mapren.TILE_MASK) == 'other'}
    out.append((True, f"{len(unknown)} of {len({t & mapren.TILE_MASK for t in g['tiles']})} "
                      f"used tiles unclassified by the colour heuristic"))
    return out


def report(g):
    print(f"=== {g['name']}  {g['width']}x{g['height']} map ===\n")
    print("sides:")
    for s in g['sides']:
        if s['in_use']:
            print(f"  {s['index']} {s['name']:<16} gold={s['gold']:>4}  capital={s['capital_city']}")
        else:
            print(f"  {s['index']} {s['name']:<16} (unused)")

    counts = collections.Counter(c['owner_name'] for c in g['cities'])
    print(f"\ncities: {len(g['cities'])}  " + ", ".join(f"{k}={v}" for k, v in counts.most_common()))
    for c in g['cities'][:6]:
        print(f"  {c['index']:>2} ({c['x']:>3},{c['y']:>3}) {c['name']:<14} "
              f"income={c['income']:>3} def={c['defence']} "
              f"owner={c['owner_name']:<14} produces={', '.join(c['produces'])}")
    print(f"  ... {len(g['cities']) - 6} more")

    st = collections.Counter(s['type_name'] for s in g['sites'])
    print(f"\nsites: {len(g['sites'])}  " + ", ".join(f"{k}={v}" for k, v in st.items()))
    print(f"signs: {len(g['signs'])}   items: {len(g['items'])}   "
          f"monsters: {len(g['monsters'])}   army types: {len(g['armies'])}")

    tc = collections.Counter(g['terrain'].get(t & mapren.TILE_MASK, 'other') for t in g['tiles'])
    total = len(g['tiles'])
    print("\nterrain (derived):")
    for k, v in tc.most_common():
        print(f"  {k:<10} {v:>6}  {v * 100 / total:>5.1f}%")
    print(f"  roads/overlay {sum(1 for b in g['roads'] if b):>5} tiles")

    print("\nvalidation:")
    ok = True
    for good, msg in validate(g):
        print(f"  [{'ok' if good else 'FAIL'}] {msg}")
        ok &= good
    return ok


if __name__ == '__main__':
    args = sys.argv[1:]
    if args[:1] == ['--all']:
        import glob
        allok = True
        for p in sorted(glob.glob('original/*/*.SCN')):
            g = load_game(os.path.dirname(p))
            allok &= report(g)
            print()
        sys.exit(0 if allok else 1)
    g = load_game(args[0] if args else 'original/ERYTHEA')
    sys.exit(0 if report(g) else 1)
