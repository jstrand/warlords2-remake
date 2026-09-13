"""Warlords II ARMYTYPE.DAT / ARMYTYP2.DAT reader -- SOLVED (stats), partial (bonuses).

29 records x 62 bytes = 1798 bytes, no file header.

    +0   u16       sprite index into the army sheet (A0.PCK .. A8.PCK, one per
                   player colour); a permutation of 0..28, 32x32 cells, 16/row
    +2   char[16]  NUL-padded name
    +18  u16       always 0 in both shipped files
    +20  u16       always 0 in both shipped files
    +22  u16       strength          1..9
    +24  u16       production time   turns, 1..4
    +26  u16       upkeep            gold/turn
    +28  u16       movement points
    +30  i16       production cost in gold; NEGATIVE means the type cannot be
                   built in cities (Navy, Hero and all the magical/special
                   units, which are found, allied or hired instead)
    +32..+60       15 x u16 combat-bonus fields -- see BONUS_FIELDS below

ARMYTYP2.DAT is byte-identical except for four names (the 's' command-line
switch documented in READ.ME): Ghosts->Shamblers, Demons->Gargoyles,
Devils->Imps, Archons->Winged Folk. Diffing the two files is what pins the
name field and therefore the 62-byte stride.

Bonus fields
------------
The game's own wording for these lives in DATA/STRING.DAT group 163:

    0  '  -'                     9  '+%d stack in woods'
    1  'Boat strength of 4'     10  '+%d stack in open'
    2  '+%d hero bonus'         11  '+%d stack in city'
    3  '+1 & cancel hero'       12  '+%d str in hills'
    4  '+1 & cancel non-hero'   13  '+%d str in woods'
    5  '+%d special'            14  '+%d str in open'
    6  'Cancel city bonus'      15  '+%d str in city'
    7  '%d enemy stack'         16  '+%d to stack'
    8  '+%d stack in hills'

The fields run in REVERSE order against that list. Five independent anchors
confirm the +32..+46 and +60 region; the middle is not yet pinned down (the
uncertain ones are marked '?' and carry the observed membership instead).
"""
import struct

RECORD_SIZE = 62
COUNT = 29

#              offset, label,                        confirmed-by
BONUS_FIELDS = [
    (32, '+%d str in city',    '?'),
    (34, '+%d str in open',    'Light/Heavy Cav. and Pikemen -- cavalry in the open'),
    (36, '+%d str in woods',   'Archers'),
    (38, '+%d str in hills',   'Dwarves'),
    (40, '+%d stack in city',  'set with the other three for Pegasi/Unicorns/magicals'),
    (42, '+%d stack in open',  'ditto'),
    (44, '+%d stack in woods', 'ditto'),
    (46, '+%d stack in hills', 'Wolfriders (hills) -- fixes the order of this group'),
    (48, '?', 'Wizards, Giant Worms, Ghosts, Demons, Elementals, Devils, Archons, Dragons'),
    (50, '?', 'Elephants only, value -1 (the only negative in the table)'),
    (52, '?', 'Catapults 1, Archons 2, Devils 3'),
    (54, '?', 'Giant Bats, Pegasi, Griffins, Archons, Dragons -- i.e. the fliers'),
    (56, '?', 'Scouts, Orcish Mob, Archers'),
    (58, '?', 'Scouts, Orcish Mob, Dwarves, Giants'),
    (60, 'Boat strength of 4', 'Navy'),
]


def load(path):
    """Return a list of 29 dicts, one per army type."""
    with open(path, 'rb') as f:
        d = f.read()
    if len(d) != RECORD_SIZE * COUNT:
        raise ValueError(f"{path}: expected {RECORD_SIZE * COUNT} bytes, got {len(d)}")
    out = []
    for k in range(COUNT):
        r = d[RECORD_SIZE * k:RECORD_SIZE * (k + 1)]
        sprite, = struct.unpack_from('<H', r, 0)
        strength, time, upkeep, move = struct.unpack_from('<4H', r, 22)
        cost, = struct.unpack_from('<h', r, 30)
        out.append(dict(
            index=k,
            sprite=sprite,
            name=r[2:18].split(b'\0')[0].decode('latin1'),
            strength=strength,
            production_turns=time,
            upkeep=upkeep,
            movement=move,
            cost=cost,
            producible=cost >= 0,
            bonuses={off: struct.unpack_from('<h', r, off)[0] for off, _, _ in BONUS_FIELDS},
        ))
    return out


if __name__ == '__main__':
    import sys
    recs = load(sys.argv[1] if len(sys.argv) > 1 else 'original/TERRAIN0/ARMYTYPE.DAT')
    hdr = f"{'#':>2} {'spr':>3} {'name':<14}{'str':>4}{'time':>5}{'upkp':>5}{'move':>5}{'cost':>7}  bonuses"
    print(hdr)
    print('-' * len(hdr))
    for r in recs:
        b = " ".join(f"@{o}={v}" for o, v in r['bonuses'].items() if v)
        cost = r['cost'] if r['producible'] else f"({r['cost']})"
        print(f"{r['index']:>2} {r['sprite']:>3} {r['name']:<14}{r['strength']:>4}"
              f"{r['production_turns']:>5}{r['upkeep']:>5}{r['movement']:>5}{cost:>7}  {b}")
