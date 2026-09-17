# `.SCN` / `.SGN` / `.SPC` / `.CTY` — scenario data

Status: **structure and placement solved; per-city and per-side stats not yet decoded.**
Parser: `tools/scenario.py`.

**The game loads the whole file verbatim into one segment** (flat `2c04`, Ghidra
`3c04`) and uses it as live game state, so every offset in this document is
also a memory address in `WARLORD2.EXE`. Confirmed by the code's table
addresses matching the file layout: sites `0x811`, items `0xce9`, cities
`0x157d`, fight order `0x60b`. Offsets in the unknown region that the code
names:

| offset | meaning |
|---|---|
| `0x0110` | current player (0 in files) |
| `0x0112` | combat modifier cap, 5 in every shipped scenario |
| `0x011a`…`0x012c` | the ten game options, `u16` each, in `STRING.DAT` group 4 order |
| `0x012e` | hidden option, 1 only in `TUTORIA.SCN` (tutorial hero immunity) |
| `0x0181` | army count |
| `0x080f` | site count; `0x157b` city count |
| `0x05e3` | hero experience, one byte per hero (upper 6 bits, max 60) |
| `0x1007` | `u16[10]` monster strengths, one per monster record |

**Site record (31 bytes), fields the code uses:** `+0` x, `+2` y, `+0x18`
content (file: 1 temple / 2 ruin; at game start: 2 item, 3 sage, 4 gold,
5 allies), `+0x19` item index, `+0x1a` guardian monster index or ally army
type, `+0x1b` `u16` "rich" flag.

**Item record (29 bytes):** `+0` name, `+20` effect type (1 battle, 2 command,
8 standard; STRING.DAT group 167 also names flight, double movement and gold
per city), `+21` value, `+22` status (0 out of play, 1 on the ground, 2 in a
ruin, 3 carried), `+23` `u16` holder or site, `+25`/`+27` x, y.
| `0x0710` | tile id → terrain type table (`0..11`; 10 = city) |

Every shipped `.SCN` is **exactly 12001 bytes** — a fixed layout with fixed-size
arrays, so unused slots are simply zero. That fixed size across all six
scenarios is what makes the layout easy to walk.

## `.SCN` map

| Offset | Contents |
|---|---|
| `0` | 8 × 20-byte **side names** |
| `160` | scenario configuration — mostly undecoded; see below |
| `387` | 8 × 20-byte **side** records |
| `2063` | `u16` site count (≤ 40) |
| `2065` | 40 × 31-byte **site** records |
| `3305` | 22 × 29-byte **item** records (8 standards + 14 magic items) |
| `3943` | 10 × 16-byte **monster** records (slot 0 blank, then 9 guardians) |
| `4103` | unused / not decoded (almost entirely zero) |
| `5499` | `u16` city count (≤ 80) |
| `5501` | 80 × 65-byte **city** records |
| `10701` | trailing zeros to end of file |

### Side record — 20 bytes, 8 of them at `387`

```
+0   u16   side index (0..7, always sequential)
+2   u16   STARTING GOLD
+4   u16   always 0
+6   u16   capital x
+8   u16   capital y
+10  10 bytes, always 0
```

Verified two ways. Every side whose name is not `"Not Used"` has a capital
`(x, y)` that resolves **exactly** to a city in the same scenario's city array —
Mirea for the Sirians, Doom's Keep for the Lich King, Dreamgate for the Dream
Knights, and so on across all six scenarios. Unused side slots carry stale
coordinates that resolve to nothing, which is itself a useful signal for
detecting which sides a scenario actually uses.

The gold field cross-checks against the running game: Erythea's Sirians start
with **200**, and at turn 1 the status bar reads **234gp** — `200 + 38 income
− 4 upkeep = 234`.

### Site record — 31 bytes

```
+0   u16      x
+2   u16      y
+4   char[20] name
+24  u16      type: 1 = Temple, 2 = Ruin
```

The type indexes `STRING.DAT` group 113, whose `[1]` is `'Type: Temple'`.

### Item record — 29 bytes

```
+0   char[20] name
+20  u16      type / effect id   (cf. STRING.DAT group 167)
```

**No coordinates** — items are carried or hidden in ruins, not placed on the
map. Erythea's 22 entries are 8 standards (one per side) and 14 magic items:
Firesword, Icesword, Lightsword, Darksword, Staff of Might, Crown/Sceptre/Orb
of Loriel, Ring of Power, Staff of Ruling, Spear of Ank, Bow of Eldros, Horn of
Ages, Crimson Banner.

### Monster record — 16 bytes

```
+0   char[12] name
```

Slot 0 is blank (a "none" entry), then Troll, Giant, Wolf, Goblin, Dragon,
Demon, Devil, Wizard, Ghost — the ruin guardians.

### City record — 65 bytes

```
+0   u16      x
+2   u16      y
+4   char[16] name
+20  u8       file: unknown, 0..8. In memory the game overwrites it with the
              city DEFENCE (1 or 2) at setup -- see docs/rules.md > Production
+21  u8       OWNER; 15 = neutral in every file, capitals assigned at game start
+22  u8[4]    PRODUCTION SLOTS: army type ids, 255 = empty
+26  u8[16]   four more per-slot arrays -- partially decoded, see below
+42  u8       INCOME in gold (confirmed: Mirea 38, Axbridge 27)
+43  zero padding (all 22 bytes, every city, every scenario)
```

### Production slots (`+22..+25`) — confirmed

Army type ids using the **canonical id** (`ARMYTYPE.DAT +0`, the same id
`STRING.DAT` group 105 is indexed by). Across all **306 cities in all six
scenarios** the four entries are strictly ascending, padded with `255` at the
end only, and never exceed 28. That invariant is what confirms the field.

Verified against the game: Mirea's slots are `[1, 4, 5, 10]` =
**Heavy Inf, Heavy Cav, Navy, Catapults**, and the city dialog shows a footman,
a horseman and a catapult plus one empty circle. The empty circle is the
un-purchased fourth type — screenshot 3's *"Build Prod — Buy new army types to
produce!"*. Reading the ids as ARMYTYPE *record* indices instead gives
"Giant Bats, Light Cav, Catapults, Heavy Cav", which does not match the dialog.

### `+42` — income, confirmed

Two independent readings: Mirea shows `Income: 38 gold` and holds 38; Axbridge
shows `Income: 27 gold` and holds 27.

### Per-slot arrays (`+26..+41`) — the production stats, rebuilt at game start

**Solved in `WARLORD2.EXE`.** The four arrays are, per slot, **time `+26`,
strength `+30`, move `+34`, cost `+38`**, and the production screen really
does display them. The values stored in the `.SCN` file are **overwritten at
game start**: `setup_city_production` copies each type's stats from
`ARMYTYPE.DAT` and then applies small random variations (`docs/rules.md` ›
Production). That's why the file contents never matched the screen. The
analysis below is kept for the record; its conclusion ("not the production
stats") was wrong about the fields and right about the file values.

### Original analysis

Sixteen bytes that look like four arrays of four. Ground truth rules out the
obvious reading:

**Catapults at Mirea displays pure ARMYTYPE base stats** (`Time 4, Cost 16,
Strength 2, Move 16` — all four match record 5 exactly). If the city block
supplied the displayed stats, Catapults' slot would have to contain those
values; it contains `3, 4, 8, 8` instead. So the production screen reads
**ARMYTYPE**, and this block is something else.

Largash confirms the *alignment* even though the meaning is open: it has three
types (`[1, 4, 10, 255]`) and **every fourth array element is 0**, so the block
really is four arrays of four at `+26`, `+30`, `+34`, `+38`, indexed by slot.

Three independent disproofs that these are the displayed production stats:

| City | Unit | slot | array values | game shows |
|---|---|---|---|---|
| Mirea | Heavy Cav. | 1 | 2, 6, 18, 6 | time 3, cost 8, str 4, move 20 |
| Mirea | Catapults | 3 | 3, 4, 8, 8 | time 4, cost 16, str 2, move 16 |
| Largash | Heavy Inf. | 0 | 1, 5, 10, 4 | move 16 |

In every case the displayed values are absent from the slot. Time, cost and
strength come from `ARMYTYPE.DAT`; **Move is computed at runtime** (see
`docs/formats/armytype.md`).

The clincher: Mirea and Largash have **byte-identical slot-0 arrays**
(`1, 5, 10, 4`, both Heavy Inf), yet the game shows Move 20 at Mirea and 16 at
Largash. Identical input, different output — these 16 bytes cannot be the
source of the production stats. Candidates worth testing instead: starting
garrison composition, or per-slot production progress.

### Initial ownership is DERIVED, not stored

There is no owner field anywhere, and there does not need to be: **at scenario
start each side owns exactly its capital, and every other city is neutral.**

```
owner(city) = the side whose capital (x, y) equals this city's, else neutral
```

Three independent confirmations:

1. The running game shows the Sirian player holding exactly **1 city** on turn 1.
2. Mirea is the Sirian capital and reads `Owner: Sirians`; Axbridge and Largash
   are not capitals and both read `Owner: Neutral City`.
3. In every scenario the number of capitals that resolve to a real city equals
   the number of sides that are not named `"Not Used"` — 8 for Erythea, Hadesha
   and Isladia, 5 for Dragon, 4 for Sorcery, 2 for Tutoria — and all capitals
   are distinct.

Ownership changes during play, of course; that belongs to the save format, not
the scenario.

### Defence is NOT in the city record

Mirea (Sirians, capital) and Axbridge (neutral) both show `Defence: 2`, yet
**no byte position holds 2 in both records**. And no position has an
owner-shaped distribution. Combined with `+43..+64` being zero everywhere, city
ownership and defence must live in the per-side block.

## `.SGN` — signposts

```
u16 count, then count x 104-byte records:
  +0   u16      x
  +2   u16      y
  +4   char[100] text, NUL-terminated
```

The tail after the NUL is **uninitialised memory** — you can read leftover
strings like `Erythea\Erythea.spc`, `Erythea\Erythea.scn` and `Road` in it. The
game wrote a 100-byte buffer without clearing it, so each record leaks a slice
of whatever was on the heap. Harmless, but don't mistake it for data.

## `.SPC` / `.CTY` — descriptions

Plain text, one entry per line: `#NNN|line|line|` terminated by CRLF, indexed
by site / city id. `.SPC` always holds 40 entries; `.CTY` holds one per city
(80 for Erythea and Hadesha, 40 Dragon, 20 Sorcery, 6 Tutoria).

## How placement was verified

Coordinates were not assumed — each record's `(x, y)` was checked against the
map, and every class lands on its own dedicated terrain tile:

| Records | Terrain tile | Art |
|---|---|---|
| signposts | 0 | wooden signpost |
| sites | 10 or 12 | temple (columned building) / ruins (broken columns) |
| cities | 96 | castle |

For **all six** scenarios the number of tile-0 cells equals the sign count and
the number of tile-96 cells equals the city count, exactly:

| Scenario | cities = tile 96 | signs = tile 0 | sites |
|---|---|---|---|
| Erythea | 80 | 114 | 40 |
| Hadesha | 80 | 117 | 40 |
| Isladia | 80 | 88 | 40 |
| Dragon | 40 | 51 | 24 |
| Sorcery | 20 | 16 | 16 |
| Tutoria | 6 | 2 | 6 |

**The site *type* is independent of the art.** Both tile 10 and tile 12 carry
both types, so you cannot read temple-vs-ruin off the map. The type field is
authoritative, and it checks out on names: every type-1 site is called
"… Temple" / "Temple of …" and no type-2 site is. Tutoria has none.

## Open questions

- **`160..2062` (1903 bytes, 26% non-zero)** — the one substantial unknown.
  Almost certainly per-side configuration: starting gold, capital, AI
  personality, diplomacy state. It opens with eight `u16`s
  `{15, 7, 8, 9, 10, 6, 5, 0}`, one per side.
- **City `+26..+41`** — the per-slot production stats (see above).
- **City defence** is still unlocated. It is mutable at runtime (the help file's
  `- Pillage -` reads "Reduce defence for gold"), so the scenario only needs to
  store a starting value; no byte position holds 2 for both Mirea and Axbridge,
  which both display `Defence: 2`.
- **`1547..1999`** is a **fighting order table**: the 29-element permutation
  `8 3 1 6 12 28 5 11 16 17 13 0 9 10 2 15 4 7 14 25 18 21 22 20 19 26 24 23 27`
  repeated ~15 times, i.e. a per-side combat order over the 29 army types. It
  matches `STRING.DAT` group 127 ("Fighting Order" / "Order of combat for %s").
  All the copies in Erythea are identical, so every side starts with the same
  default order.
- **`+20`** correlates strongly with income (mean income rises monotonically
  16.7 → 36.3 as `+20` goes 3 → 7) — plausibly city size or rank.
- `4103..5498` — nearly all zero; possibly reserved.
- Whether the item `type` field maps onto `STRING.DAT` group 167's five effects
  (`+%d in battle!`, `+%d to command!`, `Allows flight!`, `Doubles movement!`,
  `+%d gold per city!`).
