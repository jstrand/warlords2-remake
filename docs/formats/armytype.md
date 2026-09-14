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
| `+26` | `u16` | **cost** — the per-turn gold cost shown on the production screen (confirmed: Heavy Cav = 8) |
| `+28` | `u16` | **disputed.** Was read as movement, but the game shows Heavy Cav Move 20 where this field holds 16. See note below. |
| `+30` | `i16` | **purchase price** — what 'Build Prod → Buy new army types to produce!' charges to add this type to a city; **negative ⇒ can never be bought** |
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

### Time, cost and strength confirmed; **Move is NOT in this file**

Three in-game production screens pin `+24`, `+26` and `+22` exactly:

| City | Unit | `+24` time | `+26` cost | `+22` str | `+28` | **game Move** |
|---|---|---|---|---|---|---|
| Mirea | Heavy Cav. | 3 | 8 | 4 | 16 | **20** |
| Mirea | Catapults | 4 | 16 | 2 | 16 | **16** |
| Largash | Heavy Inf. | 2 | 5 | 3 | 8 | **16** |

Time, cost and strength match every time. **Move never resolves to `+28`:**

- Heavy Cav and Catapults both have `+28 = 16` but display **20** and **16**,
  so the displayed move is not a function of `+28` alone.
- Heavy Inf has `+28 = 8` and displays **16**.
- Byte-level search: the value 20 appears **nowhere** in Heavy Cav's 62-byte
  record, and 16 appears nowhere in Heavy Inf's. It is not a mis-read offset.

It is not in the scenario either: Largash's city record contains no byte equal
to 16 at all, and a scan for any 29-entry table (u8 or u16, indexed by either
canonical id or record id) matching the three known moves finds **zero hits**
in `WARLORD2.EXE`, `START.EXE`, `ARMYTYPE.DAT` and `ERYTHEA.SCN`.

So the displayed **Move is computed at runtime**, not stored. `+28` remains
unidentified — it is plausibly upkeep (its values are upkeep-shaped: foot 8-12,
Navy 30, Wizards 50), but do not label it movement.

#### Move is per-city, and not stored in the city either

Heavy Inf displays **Move 20 at Mirea** and **Move 16 at Largash** — same unit,
two cities. So move is per-city. But those two cities' slot-0 arrays are
**byte-identical** (`1, 5, 10, 4`), which rules out the city record as the
source too.

All observations so far:

| City | Owner | Heavy Inf | Heavy Cav | Catapults |
|---|---|---|---|---|
| Mirea | Sirians | **20** | **20** | **16** |
| Largash | neutral | **16** | — | **16** |

Base `+28` for those three units is 8, 16, 16 — which matches none of it.

The pattern fits a **per-side movement bonus**: at neutral Largash both units
read 16; at Sirian Mirea the infantry and cavalry read 20 while Catapults stays
16. I.e. the Sirians appear to grant +4 to non-siege units, with siege engines
exempt. That is consistent with Mirea being "the home city of the Sirian
Knights", and it means `+28` is not move at all (its values are upkeep-shaped:
foot 8-12, Navy 30, Wizards 50).

Parked rather than solved — the per-side bonus table has not been located. An
earlier `4 x (+20) - 8` fit to two points is a coincidence: it goes negative for
the 47 cities with `+20 = 0`, and it cannot explain Catapults staying at 16.

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

| Offset | Meaning | Confirmed by |
|---|---|---|
| `+32` | **individual +n strength in CITY** | Manual App. C: "CITY: Minotaurs (+1) Spiders (+2) Griffins (+2)" — exactly this field |
| `+34` | **individual +n strength in OPEN** | "OPEN: Light Cav. (+1) Pikemen (+1) Heavy Cav. (+2)" |
| `+36` | **individual +n strength in WOODS** | "WOODS: Archers (+1)" |
| `+38` | **individual +n strength in HILLS** | "HILLS: Dwarves (+1)" |
| `+40` | **group (MAX TERRAIN) bonus, CITY** | Manual: "MAX TERRAIN: WolfRiders (+1 hills), Dragon (+2 all), Wizard, Worm, Undead, Demon, Elemental, Devil, Archon, Unicorn, Pegasi (+1 all)" |
| `+42` | **group bonus, OPEN** | ditto |
| `+44` | **group bonus, WOODS** | ditto |
| `+46` | **group bonus, HILLS** | Wolfriders carry only this one — matches "WolfRiders (+1 hills)" |
| `+48` | special/magical flag | set for exactly the nine non-buildable magical types |
| `+50` | **MAX SUBTRACT** (−n to every enemy army) | Manual: "MAX SUBTRACT: Elephant (−1 to all enemy armies)" — Elephants are the only −1 |
| `+52` | **ability enum**: 1 = Siege, 2 = Negate Hero, 3 = Negate Non-Hero | Manual: "SIEGE: Catapult / NEGATE HERO: Archon / NEGATE NON HERO: Devil" — and the file holds Catapults 1, Archons 2, Devils 3 |
| `+54` | **FLYING** | `STRING.DAT` group 105 tags exactly these five `fly` |
| `+56` | unresolved | Scouts, Orcish Mob, Archers |
| `+58` | unresolved | Scouts, Orcish Mob, Dwarves, Giants |
| `+60` | **Boat strength of 4** | Navy only; Manual: boats attacking boats/fliers fight at strength 4 or natural, whichever is lower |

Both bonus groups run in the order **CITY, OPEN, WOODS, HILLS** — the reverse of
`STRING.DAT` group 163's listing, which is why the reverse-order inference was
right. Only `+56` and `+58` remain unidentified.

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
