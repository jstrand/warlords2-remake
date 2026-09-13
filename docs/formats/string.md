# `DATA/STRING.DAT` — UI text corpus — **SOLVED**

The game's entire user-facing text: **169 groups, 651 strings**. Every byte of
the 14054-byte file is accounted for — no padding, no unreferenced strings, no
slack. Parser: `tools/string_dat.py` (which includes a `verify()` that
re-derives every invariant below).

## Layout

```
0                 index table: n entries of { u16 offset, u16 count }
<first offset>    pointer region: per group, `count` u16 file offsets
<after pointers>  string data: NUL-terminated latin-1
```

`n` is never stored. **The first index entry's offset is itself the size of the
index table**, so `n = read_u16(0) / 4`. For the shipped file that gives 169
groups, and `169 * 4 == 676` matches exactly.

Verified invariants:

- index table `0..676`, pointer region `676..1978`, string data `1978..14054`
- each group's pointer array tiles the pointer region contiguously and in order
- all 651 pointers ascend; the first equals the start of the string data
- 651 NUL-terminated strings exist and each is referenced **exactly once**
- zero trailing bytes

A *group* is one screen, dialog or enumeration — the unit the game retrieves.

## Groups that encode rules

These are the ones that answer questions elsewhere in the project.

### 105 — per-army ability tags (29 entries)

**Indexed by `ARMYTYPE.DAT`'s `+0` field, not by record order.** This is the
proof that `+0` is the game's **canonical army type ID**, and that the records
in `ARMYTYPE.DAT` are stored in a different (roughly strength-ascending,
production-list) order.

```
0-7   -                  16 -
8     fly   (Pegasi)      17 -
9     fly   (Griffins)    18 -
10    siege (Catapults)   19 spc/fly (Archons)
11-14 -                   20-24 spc
15    fly   (Giant Bats)  25 spc/fly (Dragons)
                          26-27 spc      28 +%d (Hero)
```

Cross-checking against `ARMYTYPE.DAT` settles two previously open fields:

- **`+54` is FLYING.** The five types with `+54 = 1` are Giant Bats, Pegasi,
  Griffins, Archons, Dragons — *exactly* the five tagged `fly`. Set equality.
- **`+48` is "special" (`spc`).** Matches for Giant Worms, Elementals, Wizards,
  Ghosts, Demons, Devils, Archons, Dragons. One anomaly: **Unicorns** are
  tagged `spc` but have `+48 = 0`, so either the tag table is hand-authored or
  `spc` is also derived from the stack-bonus group.
- **`+52` is not simply "siege".** Catapults (the only `siege` tag) have
  `+52 = 1`, but Archons have 2 and Devils 3 without being tagged siege — so
  `+52` is a magnitude, plausibly `'+%d special'` from group 163.

### 163 — Army Bonus wording (17 entries)

Names the 15 bonus fields at `ARMYTYPE.DAT+32..+60`. See
`docs/formats/armytype.md` for the mapping and which parts are confirmed.

```
 0 '  -'                   9 '+%d stack in woods'
 1 'Boat strength of 4'   10 '+%d stack in open'
 2 '+%d hero bonus'       11 '+%d stack in city'
 3 '+1 & cancel hero'     12 '+%d str in hills'
 4 '+1 & cancel non-hero' 13 '+%d str in woods'
 5 '+%d special'          14 '+%d str in open'
 6 'Cancel city bonus'    15 '+%d str in city'
 7 '%d enemy stack'       16 '+%d to stack'
 8 '+%d stack in hills'
```

### 128 / 129 — terrain (10 each, index-paired)

`129[i]` describes `128[i]`:

| # | Terrain | Description |
|---|---|---|
| 0 | Road | The fastest way to travel |
| 1 | Bridge | For land armies to cross water |
| 2 | Water | Only for flying armies and boats |
| 3 | Shore | Only for flying armies and boats |
| 4 | Forest | Slows down movement |
| 5 | Hills | Slows down movement |
| 6 | Mountains | Only for flying armies |
| 7 | Plain | The next best thing to roads |
| 8 | Marsh | Very hard to pass through this |
| 9 | Tower | A good view of the countryside |

**Ten terrain types** — the ordering to expect in the `.MAP` tile indices, and
the set a movement-cost table must cover. No 10-entry table appears in
`ARMYTYPE.DAT`, so movement costs live elsewhere or in code.

### Other notable groups

| Group | Contents |
|---|---|
| 4 | the ten game options (Neutral Cities, Diplomacy, Quests, Hidden Map, View Enemies, View Production, Intense Combat, Quick Start, Military Advisor, Random Turns) |
| 57 | what a ruin/temple hint reveals (gold, artifact, sage, allies, friendship) |
| 62 | quest-target phrasing |
| 94 | the 17 turn-log event messages |
| 101–104 | hero level-ups: Cavalier/Amazon, Champion, Paladin/Valkyrie, plus `Level/Exp/Needs/Str/Move` column headers |
| 106 | the 8 diplomacy reputation ranks, Statesman → Running Dog |
| 113 | 9 site types — index 1 is Temple, the rest Ruin |
| 116 | city income wording |
| 131 | army-info panel labels (`Near:`, `In:`, `Battle:`, `Command:`, `Level:`, `Exp:`) |
| 167 | the 5 item effect types (`+%d in battle!`, `+%d to command!`, `Allows flight!`, `Doubles movement!`, `+%d gold per city!`) |

Group 167 is worth flagging: it enumerates exactly what magic items can do,
which constrains the `.ITM` format in every scenario directory.

## Usage

```sh
python3 tools/string_dat.py                          # verify + dump everything
python3 tools/string_dat.py original/DATA/STRING.DAT 105 163   # specific groups
```

## Likely siblings

`BUTTON.DAT` (15312 bytes) and `FILE.DAT` (4495) are the other large `DATA/`
files and probably share this container. Worth trying `verify()` against them
before writing anything new.
