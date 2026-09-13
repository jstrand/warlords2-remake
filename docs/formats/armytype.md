# `ARMYTYPE.DAT` / `ARMYTYP2.DAT` — army type table

Status: **stats solved, bonus fields partially solved.** Parser: `tools/armytype.py`.

`29 records x 62 bytes = 1798 bytes`, no file header.

## Record layout

| Offset | Type | Meaning |
|---|---|---|
| `+0` | `u16` | sprite index into the army sheet |
| `+2` | `char[16]` | NUL-padded name |
| `+18` | `u16` | always 0 in both shipped files |
| `+20` | `u16` | always 0 in both shipped files |
| `+22` | `u16` | **strength**, 1–9 |
| `+24` | `u16` | **production time** in turns, 1–4 |
| `+26` | `u16` | **upkeep**, gold/turn |
| `+28` | `u16` | **movement points** |
| `+30` | `i16` | **production cost** in gold; **negative ⇒ not buildable in cities** |
| `+32`…`+60` | `15 × u16` | combat bonuses (below) |

`ARMYTYP2.DAT` is byte-identical apart from four names — the `s` switch from
`READ.ME`: Ghosts→Shamblers, Demons→Gargoyles, Devils→Imps, Archons→Winged
Folk. **Diffing the two files is what cracked the format**: the four changed
runs sit at 1366/1428/1552/1614, whose deltas (62, 124, 62) give the stride,
and `1798 = 29 × 62` confirms the record count with no header.

### `+0` is the canonical army type ID

`STRING.DAT` group 105 is a 29-entry ability-tag table indexed by this field,
which proves `+0` is the game's **army type ID** — the records in this file are
stored in a different (roughly strength-ascending) display order. An engine
should key army types by `+0`, not by record position.

It doubles as the sprite index: a permutation of 0…28, indexing 32×32 cells
(16 per row) in
`TERRAIN0/A0.PCK`…`A8.PCK` — one sheet per player colour. Verified visually:
cell 15 is a bat (Giant Bats), 12 a spider (Spiders), 8 and 21 white horses
(Pegasi, Unicorns), 27 a red figure with a pitchfork (Devils).
`tools/export_png.py` plus the snippet in this repo's history will cut named
sprites into `preview/armies/`.

### Negative cost

Nine types have a negative cost and cannot be produced in a city: Navy (−500),
Wizards / Giant Worms / Ghosts / Demons (−1500), Elementals / Devils (−2000),
Archons (−2500), Dragons (−3000), Hero (−1). These are the units that are
found in ruins, allied, or hired. The magnitudes look like hire/ally prices,
which is worth confirming in game.

## Combat bonus fields

The game's own wording is in `DATA/STRING.DAT` group 163 (see
`docs/formats/string.md`):

```
 0  '  -'                   9  '+%d stack in woods'
 1  'Boat strength of 4'   10  '+%d stack in open'
 2  '+%d hero bonus'       11  '+%d stack in city'
 3  '+1 & cancel hero'     12  '+%d str in hills'
 4  '+1 & cancel non-hero' 13  '+%d str in woods'
 5  '+%d special'          14  '+%d str in open'
 6  'Cancel city bonus'    15  '+%d str in city'
 7  '%d enemy stack'       16  '+%d to stack'
 8  '+%d stack in hills'
```

The record's fields run in **reverse order** against that list. The ends are
nailed down by five independent anchors; the middle is not.

| Offset | Meaning | Evidence |
|---|---|---|
| `+32` | `+n str in city` *(inferred)* | Minotaurs 1, Spiders 2, Griffins 2 |
| `+34` | **`+n str in open`** | Light Cav. 1, Pikemen 1, Heavy Cav. 2 — cavalry in the open |
| `+36` | **`+n str in woods`** | Archers 1 |
| `+38` | **`+n str in hills`** | Dwarves 1 |
| `+40` | `+n stack in city` | set as a group of four for Pegasi/Unicorns/magicals |
| `+42` | `+n stack in open` | ditto |
| `+44` | `+n stack in woods` | ditto |
| `+46` | **`+n stack in hills`** | Wolfriders 1 — fixes the order of this group |
| `+48` | **`special` (spc)** | confirmed against STRING.DAT group 105; one anomaly, Unicorns are tagged `spc` with `+48 = 0` |
| `+50` | **unresolved** | Elephants only, value **−1** — the only negative in the table |
| `+52` | **unresolved, a magnitude** | Catapults 1 (tagged `siege`), Archons 2, Devils 3 (not tagged siege) — plausibly `'+%d special'` |
| `+54` | **unresolved** | Giant Bats, Pegasi, Griffins, Archons, Dragons — exactly the fliers |
| `+56` | **unresolved** | Scouts, Orcish Mob, Archers |
| `+58` | **unresolved** | Scouts, Orcish Mob, Dwarves, Giants |
| `+60` | **`Boat strength of 4`** | Navy only |

Pegasi, Unicorns and the magical units carry `+1` in all four *stack* slots
(Dragons `+2`), i.e. a flat stack bonus everywhere — which matches how those
units behave in Warlords II.

### Why the middle is still open

A strict reverse-sequential mapping would make `+50` *'Cancel city bonus'* and
`+52` *'+n special'*. That reads wrong: **Catapults** are the canonical
cancel-city-bonus unit, and they sit at `+52`, not `+50`. Shifting the whole
map by one to fix that instead breaks all five confirmed anchors, so the
middle of the list is not a simple linear run and should not be guessed.

Two cheap in-game checks in DOSBox would settle it — open the army info panel
for a **Catapult** and an **Elephant** and read off which bonus line each
shows. `+54`'s membership is exactly the flying units, but *flying* is a
movement capability (`STRING.DAT` group 129 describes terrain as "Only for
flying armies"), not one of the 17 bonuses, so `+54` may be a bonus that only
fliers happen to carry — or the movement table may live elsewhere entirely.

## Open questions

- Where per-terrain **movement costs** are stored. `STRING.DAT` group 128 names
  ten terrain types (Road, Bridge, Water, Shore, Forest, Hills, Mountains,
  Plain, Marsh, Tower) but no 10-entry table appears in this record.
- Whether `+18`/`+20` are reserved, or fields only non-zero in custom army sets.
- Whether the negative costs double as ally/hire prices.
