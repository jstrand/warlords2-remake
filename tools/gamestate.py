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
    tiles     terrain grid + road overlay, with the game's own terrain types

Two values are DERIVED, not stored (see docs/rules.md):
  * ownership -- at scenario start each side owns exactly its capital
  * defence   -- 1 if a city produces fewer than 3 army types, else 2

Terrain comes from the tile -> terrain-type table stored in every .SCN at
0x710 (the same table WARLORD2.EXE reads), with the movement costs and combat
classes the executable uses for each type (docs/rules.md > Movement, Combat).
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

MAP_W, MAP_H = mapren.MAP_W, mapren.MAP_H
NEUTRAL = None
NAVY = 5   # army type id removed from all production lists at game start

# terrain type ids 0..11 as used by WARLORD2.EXE (STRING.DAT group 128 names 0..9)
TERRAIN_TYPES = ['road', 'bridge', 'water', 'shore', 'forest', 'hills',
                 'mountains', 'plain', 'marsh', 'tower', 'city', 'site']
# DS:1274 in WARLORD2.EXE; 0 = impassable on land. A road overlay makes any tile cost 1.
MOVE_COST = [1, 1, 1, 2, 4, 6, 0, 2, 5, 2, 1, 2]
# combat_terrain_class: bonus fields in ARMYTYPE.DAT run city, open, woods, hills
COMBAT_CLASS = ['open', 'open', 'open', 'open', 'woods', 'hills',
                'hills', 'open', 'open', 'open', 'city', 'city']
TERRAIN_TABLE_OFFSET = 0x710


def load_terrain_table(scn_path):
    """tile index (low byte of the map word) -> terrain type id, from the .SCN (255 entries)."""
    with open(scn_path, 'rb') as f:
        f.seek(TERRAIN_TABLE_OFFSET)
        return list(f.read(255))   # 0x80f is the site count


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
    terrain_table = load_terrain_table(base + '.SCN')
    terrain = {t: TERRAIN_TYPES[v] for t, v in enumerate(terrain_table)}

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
        # the game drops Navy from every city at start, then derives defence
        ids = [t for t in c['data'][2:6] if t not in (255, NAVY)]
        cities.append(dict(
            index=c['index'], name=c['name'], x=c['x'], y=c['y'],
            income=c['data'][22],
            produces=[by_id[t]['name'] for t in ids],
            produce_ids=ids,
            defence=2 if len(ids) >= 3 else 1,
            owner=owner,
            owner_name=sides[owner]['name'] if owner is not None else 'Neutral',
            is_capital=owner is not None,
            description=city_text.get(c['index'], []),
        ))

    sites = [dict(s, description=site_text.get(s['index'], [])) for s in scn['sites']]

    return dict(name=name, sides=sides, cities=cities, sites=sites, signs=signs,
                items=scn['items'], monsters=scn['monsters'], armies=armies,
                tiles=tiles, roads=roads, terrain=terrain, terrain_table=terrain_table,
                strings=strings,
                width=MAP_W, height=MAP_H)


def validate(g):
    """Cross-check the loaded state. Returns a list of (ok, message)."""
    out = []
    T = g['terrain']

    def tile_at(x, y):
        return g['tiles'][y * MAP_W + x] & 0xff

    out.append((len(g['tiles']) == MAP_W * MAP_H,
                f"map is {MAP_W}x{MAP_H} = {len(g['tiles'])} tiles"))
    out.append((len(g['roads']) == MAP_W * MAP_H,
                f"road overlay is 1 byte/tile ({len(g['roads'])})"))

    bad = [c for c in g['cities'] if T.get(tile_at(c['x'], c['y'])) != 'city']
    out.append((not bad, f"all {len(g['cities'])} cities stand on a city tile"))

    bad = [s for s in g['sites'] if T.get(tile_at(s['x'], s['y'])) != 'site']
    out.append((not bad, f"all {len(g['sites'])} sites stand on a site tile"))

    bad = [s for s in g['signs'] if T.get(tile_at(s['x'], s['y'])) != 'tower']
    out.append((not bad, f"all {len(g['signs'])} signs stand on a tower-type tile (signpost)"))

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

    bad = {t & 0xff for t in g['tiles'] if g['terrain_table'][t & 0xff] >= len(TERRAIN_TYPES)}
    out.append((not bad, f"every used tile has a terrain type ({len({t & 0xff for t in g['tiles']})} tiles used)"))
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

    tc = collections.Counter(g['terrain'][t & 0xff] for t in g['tiles'])
    total = len(g['tiles'])
    print("\nterrain (from the .SCN tile table):")
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
