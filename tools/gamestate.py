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
    sites     temples and ruins with position, type, description and contents
    signs     signpost positions and text
    items     standards and magic items
    armies    the 29 army types with strength / time / cost
    tiles     terrain grid + road overlay, with the game's own terrain types
    armies    one garrison per city, from the start-of-game setup rules

Two values are DERIVED, not stored (see docs/rules.md):
  * ownership -- at scenario start each side owns exactly its capital
  * defence   -- 1 if a city produces fewer than 3 army types, else 2

`apply_game_start()` then rolls what the game rolls when a scenario starts:
Navy dropped from production, per-city production stats copied from
ARMYTYPE.DAT and randomly nudged, slots sorted, defence derived, and one
garrison army placed per city, and what each ruin holds. Pass a seed for a
reproducible position.

Terrain comes from the tile -> terrain-type table stored in every .SCN at
0x710 (the same table WARLORD2.EXE reads), with the movement costs and combat
classes the executable uses for each type (docs/rules.md > Movement, Combat).
"""
import collections
import os
import random
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

# Production purpose weights (time, strength, move) and the purpose each
# garrison level uses -- see docs/rules.md > Starting garrisons.
PURPOSE_WEIGHTS = {1: (10, 4, 1), 2: (10, 10, 1), 3: (5, 10, 1),
                   4: (5, 10, 1), 5: (5, 10, 1), 6: (10, 1, 10)}
GARRISON_PURPOSE = [1, 6, 2, 3]
SCOUTS = 11        # placeholder garrison when Neutral Cities is off
SIEGE_ABILITY = 1  # ARMYTYPE +52

# Ruin contents and ally types, from the tables in WARLORD2.EXE
# (docs/rules.md > Ruins, temples and sages). 3 = sage, 4 = gold, 5 = allies.
RUIN_CONTENT = {'rich': [5, 5, 4], 'far': [3, 4, 5, 3, 4], 'near': [3, 4, 5]}
ALLY_TYPES = {'rich': [25, 23, 27, 19], 'far': [24, 20, 26], 'near': [22, 20]}
SITE_CONTENT = {0: 'empty', 1: 'temple', 2: 'item', 3: 'sage', 4: 'gold', 5: 'allies'}
CAPITAL_RANGE = 15

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


def load_item_pool(path):
    """The scenario's .ITM magic item pool (docs/formats/itm.md)."""
    with open(path, 'rb') as f:
        lines = f.read().split(b'\r\n')
    pool = []
    for line in lines[1:1 + int(lines[0])]:
        name = line[:20].decode('latin1').replace('_', ' ').strip()
        pool.append(dict(name=name, type=int(line[21:22]), value=int(line[23:24])))
    return pool


def item_reserved(item):
    """Items that can only be hidden in a rich ruin (item_reserved, 66d4:08f4)."""
    return item['type'] in (5, 6, 8) or (item['type'] == 2 and item['value'] >= 2)


def fill_item_pool(items, pool, reserved_count, rng):
    """Refill item records 8..21 from the pool, as load_item_pool does: the
    first `reserved_count` slots take reserved items, the rest ordinary ones."""
    used = set()
    by_index = {it['index']: it for it in items}
    for idx in range(8, 22):
        if idx not in by_index:
            continue
        want = idx < 8 + reserved_count
        choices = [i for i in range(len(pool)) if i not in used
                   and item_reserved(pool[i]) == want]
        if not choices:          # the game would spin here; the pool never runs out
            continue
        pick = rng.choice(choices)
        used.add(pick)
        by_index[idx].update(pool[pick])
    return items


def city_slots(city, by_id, rng):
    """The city's production slots as the game sets them up: stats copied from
    ARMYTYPE.DAT, randomly nudged, then sorted by purchase price."""
    slots = []
    for t in city['produce_ids']:
        a = by_id[t]
        strength, time = a['strength'], a['production_turns']
        cost, move = a['upkeep'], a['movement']
        if rng.randrange(100) < 10:                       # strength
            strength = min(9, strength + 1) if rng.randrange(100) < 60 else max(1, strength - 1)
        if rng.randrange(100) < 20:                       # move
            r = rng.randrange(100)
            move += 4 if r < 10 else 2 if r < 60 else -2 if r < 95 else -4
            move = max(2, move)
        move = max(6, move)
        if rng.randrange(100) < 10:                       # cost
            cost += -(cost // 4) if rng.randrange(100) < 60 else cost // 4
        if rng.randrange(100) < 10:                       # time
            time = max(1, time - 1) if rng.randrange(100) < 60 else time + 1
        slots.append(dict(type=t, name=a['name'], strength=strength, time=time,
                          cost=cost, move=move, price=abs(a['cost'])))
    slots.sort(key=lambda s: s['price'])
    return slots


def best_slot(slots, purpose, by_id, side_bonus=False):
    """The slot the game would build for a purpose (docs/rules.md)."""
    w_time, w_str, w_move = PURPOSE_WEIGHTS[purpose]
    best, best_score = None, 0
    for slot in reversed(slots):                           # ties go to the later slot
        if purpose == 4 and not by_id[slot['type']]['bonuses'][54]:
            continue                                       # purpose 4 wants fliers
        strength = min(9, slot['strength'] + (2 if side_bonus else 0))
        if by_id[slot['type']]['bonuses'][52] == SIEGE_ABILITY:
            strength += 2
        time = slot['time'] + (1 if strength < 3 and purpose != 6 else 0)
        score = (10 - min(10, time)) * w_time + strength * w_str + slot['move'] * w_move // 2
        if score > best_score:
            best, best_score = slot, score
    return best


def apply_site_setup(g, rng):
    """Roll what each ruin holds, as setup_random_sites does. The game's own
    site-eligibility test and distance metric are approximated: any unassigned
    ruin can take an item, and distance to a capital is Chebyshev."""
    sites = g['sites']
    caps = [(s['capital'][0], s['capital'][1]) for s in g['sides'] if s['in_use']]
    for s in sites:
        s['content'] = 1 if s['type'] == 1 else 0
        s['item'] = s['guardian'] = None
        s['rich'] = False

    # mark_rich_sites (66d4:091e): 30% of the non-temple sites
    ruins = [s for s in sites if s['type'] != 1]
    for s in rng.sample(ruins, min(len(ruins), len(sites) * 3 // 10)):
        s['rich'] = True

    for s in sites:
        near = any(max(abs(s['x'] - cx), abs(s['y'] - cy)) < CAPITAL_RANGE for cx, cy in caps)
        s['band'] = 'rich' if s['rich'] else ('near' if near else 'far')

    band_hi = len(sites) * 2 // 10
    band_lo = min(rng.randrange(1, 4) + rng.randrange(1, 4) + 1, band_hi)
    last = min(22, len(sites) // 3 + rng.randrange(1, 6) - 3 + 8)
    items = {it['index']: it for it in g['items']}
    for item in g['items']:
        item['status'] = 0
    free = [s for s in sites if s['content'] == 0]
    rng.shuffle(free)
    for idx in range(8, last):
        if 8 + band_lo <= idx < 8 + band_hi:
            continue                       # held back, probably for quest rewards
        # a reserved item needs a rich ruin, an ordinary one an ordinary ruin
        if idx not in items:
            continue
        want = item_reserved(items[idx])
        site = next((s for s in free if s['rich'] == want), None)
        if site is None:
            continue
        free.remove(site)
        site['content'], site['item'] = 2, idx
        items[idx]['status'] = 2           # in a ruin

    for s in sites:
        if s['content'] == 0:
            s['content'] = rng.choice(RUIN_CONTENT[s['band']])
        if s['content'] == 1:
            s['guardian'] = 0
        elif s['content'] == 5:
            s['guardian'] = rng.choice(ALLY_TYPES[s['band']])
        else:
            s['guardian'] = rng.randrange(1, 10)
        s['content_name'] = SITE_CONTENT[s['content']]
    return g


def apply_game_start(g, seed=0, neutral_cities=1):
    """Roll the start-of-game setup: per-city production stats, defence and one
    garrison army per city. See docs/rules.md > Production, Starting garrisons.
    `neutral_cities` is the option value (0 = off)."""
    rng = random.Random(seed)
    by_id = {a['sprite']: a for a in g['armies']}
    armies = []
    for c in g['cities']:
        c['slots'] = city_slots(c, by_id, rng)
        c['defence'] = 2 if len(c['slots']) >= 3 else 1
        if c['owner'] is not None:
            level = 3
        elif neutral_cities <= 0:
            level = None
        else:
            level = min(3, rng.randrange(1, 5) + neutral_cities - 2)
        slot = None
        if level is not None and c['slots']:
            slot = best_slot(c['slots'], GARRISON_PURPOSE[max(0, level)], by_id)
        if slot is None:
            armies.append(dict(x=c['x'], y=c['y'], type=SCOUTS, name=by_id[SCOUTS]['name'],
                               owner=NEUTRAL, strength=1, moves=0, upkeep=0, city=c['index']))
        else:
            armies.append(dict(x=c['x'], y=c['y'], type=slot['type'], name=slot['name'],
                               owner=c['owner'], strength=slot['strength'], moves=0,
                               upkeep=slot['cost'] // 2, city=c['index']))
    g['armies_placed'] = armies
    if g.get('item_pool'):
        fill_item_pool(g['items'], g['item_pool'], len(g['sites']) * 2 // 10, rng)
    apply_site_setup(g, rng)
    return g


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
    item_pool = load_item_pool(base + '.ITM')

    return dict(name=name, sides=sides, cities=cities, sites=sites, signs=signs,
                items=scn['items'], item_pool=item_pool, monsters=scn['monsters'], armies=armies,
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

    if 'armies_placed' in g:
        placed = g['armies_placed']
        out.append((len(placed) == len(g['cities']),
                    f"{len(placed)} garrison armies placed, one per city"))
        ids = {a['sprite'] for a in g['armies']}
        out.append((all(a['type'] in ids for a in placed),
                    "every garrison army has a real army type"))
        at = collections.Counter((a['x'], a['y']) for a in placed)
        out.append((max(at.values()) <= 8, "no tile holds more than 8 armies"))

    if g['sites'] and 'content' in g['sites'][0]:
        placed = [s['item'] for s in g['sites'] if s['content'] == 2]
        out.append((len(placed) == len(set(placed)), "each placed item is in one ruin"))
        in_ruin = [i['index'] for i in g['items'] if i['status'] == 2]
        out.append((sorted(placed) == sorted(in_ruin),
                    f"{len(in_ruin)} of {len(g['items'])} items placed in ruins"))
        out.append((all(s['guardian'] is not None for s in g['sites']),
                    "every site has a guardian or ally type"))

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

    st = collections.Counter(s.get('content_name', s['type_name']) for s in g['sites'])
    print(f"\nsites: {len(g['sites'])}  " + ", ".join(f"{k}={v}" for k, v in st.items()))
    print(f"signs: {len(g['signs'])}   items: {len(g['items'])}   "
          f"monsters: {len(g['monsters'])}   army types: {len(g['armies'])}")

    tc = collections.Counter(g['terrain'][t & 0xff] for t in g['tiles'])
    total = len(g['tiles'])
    print("\nterrain (from the .SCN tile table):")
    for k, v in tc.most_common():
        print(f"  {k:<10} {v:>6}  {v * 100 / total:>5.1f}%")
    print(f"  roads/overlay {sum(1 for b in g['roads'] if b):>5} tiles")

    if 'armies_placed' in g:
        own = collections.Counter(
            'neutral' if a['owner'] is None else g['sides'][a['owner']]['name']
            for a in g['armies_placed'])
        print("\ngarrisons: " + ", ".join(f"{k}={v}" for k, v in own.most_common(4))
              + (" ..." if len(own) > 4 else ""))
        for a in g['armies_placed'][:4]:
            print(f"  ({a['x']:>3},{a['y']:>3}) {a['name']:<12} str={a['strength']} "
                  f"upkeep={a['upkeep']}")

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
            g = apply_game_start(load_game(os.path.dirname(p)))
            allok &= report(g)
            print()
        sys.exit(0 if allok else 1)
    g = apply_game_start(load_game(args[0] if args else 'original/ERYTHEA'))
    sys.exit(0 if report(g) else 1)
