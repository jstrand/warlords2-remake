# `SAVE\SAVEn.DAT` — saved game — **layout from the executable**

Not yet checked against a real save file (none ship in `original/SAVE`). The
layout comes from `save_game` (Ghidra `7721:0a46`), which writes the game's
live memory blocks one after another with no header of its own. Everything is
little-endian.

| # | size | contents | memory source |
|---|---|---|---|
| 1 | `0x54` (84) | assorted globals, not yet mapped | `DS:5d64` |
| 2 | `0x2ee1` (12001) | **the live `.SCN` image**: sides, options, cities, sites, items, quests, diplomacy — same layout as `docs/formats/scenario.md` | segment `2c04` |
| 3 | `0xde70` (56944) | `0x8880` bytes of map words (112×156 × `u16`), then **1000 × 22-byte army records** | segment `1e1d` |
| 4 | `0x4440` (17472) | one byte per tile: low 5 bits are the road overlay (as in `.RD`), higher bits are runtime flags | segment `19d9` |
| 5 | `0x4440` (17472) | a second byte-per-tile map, not yet identified | memory handle `DS:1270` |
| 6 | `u32` + data | contents of `CURRENT.SGN` (signposts) | file |
| 7 | `u32` + data | contents of `RANDOM\RANDOM.SPC` — **only if** `.SCN` `0x120` is set (random map) | file |
| 8 | `u32` + data | contents of `RANDOM\RANDOM.CTY` — same condition | file |
| 9 | `u32` + data | contents of `CURRENT.HST` (history) | file |
| 10 | 8 × `0x42c` + `0x4b0` | computer player data, one block per side, then shared data | `623c:0da9` |

Block 10 is written only for a normal numbered save. When the same routine
writes to a caller-supplied file name (the "save map" path), it stops after
block 9.

## Map word (block 3)

The map words are the `.MAP` tile words, reused at run time. The **low byte**
is the tile index. The code uses the **high byte** for state:

- low nibble: owner of the tile (15 = nobody), refreshed from armies and cities
- `0x10`: occupied by an army
- `0x20`: tower

## Army record (22 bytes, block 3)

| offset | meaning |
|---|---|
| `+0`, `+2` | x, y (`-1`/`-2` = in transit) |
| `+4` | army type id (28 = hero) |
| `+5` | owner |
| `+6` | maximum moves |
| `+7` | moves left |
| `+8` | strength |
| `+9` | home city |
| `+10` | hero slot (heroes only) |
| `+11` | upkeep |
| `+12` | `u16` flags: `0x02`/`0x04`/`0x08`/`0x10` blessed at temple 1–4, `0x1000` at sea |
| `+14` | byte: low nibble is the standing order used by the computer players (e.g. 3); upper nibble unidentified |
| `+15` | byte: upper 7 bits = destination city while in transit, bit 0 unidentified |
| `+16` | byte: transit state — `0x65` just left, `0x66` arriving next turn, `0xff` none |
| `+18`, `+20` | `u16` move target x, y (`-1` = none); cleared when reached |
