# `.MAP` / `.RD` — scenario terrain — **SOLVED**

Renderer: `tools/mapren.py`. Every shipped scenario decodes and renders.

## `.MAP` — terrain grid

**112 wide × 156 tall = 17472 tiles**, `u16` little-endian, row-major, no
header. All six shipped maps are exactly this size (34944 bytes).

- low 15 bits — tile index into the terrain sheets
- bit 15 (`0x8000`) — a flag, **meaning unknown** (see below)

### How the width was determined

This one is a trap. Naive vertical autocorrelation says the width is **224**,
scoring 0.483 against 0.014 for 112 — and a 224×78 render even *looks*
plausible at a glance. Both are artefacts.

The terrain uses **paired tile variants in a checkerboard**. Tiles 9 and 31 are
two variants of the same terrain, and at width 112 they split by `(x + y)`
parity with **zero exceptions** across 5218 tiles — even → 31, odd → 9. Tiles
39 and 45 are likewise a pair, and render to *byte-identical* graphics (the only
colliding pair among the 111 tile indices Erythea uses).

So adjacent tiles almost never repeat numerically, which drives the lag-112
score to noise, while lag-224 (two rows down, same parity) scores 0.483. The
autocorrelation was measuring the checkerboard, not the rows.

At 224×78 each rendered row is two file rows side by side, which produces two
near-identical images — the even rows and the odd rows of the same world. Only
25.6% of the left/right tile pairs are actually equivalent.

**Lesson for the remaining formats: confirm grid dimensions with a structural
invariant (here, the parity split), not with a similarity score.** The same
applies to sheet layouts — `ROAD.PCK` turned out to use a different stride from
the terrain sheets, and only a content-run alignment check caught it.

## `.RD` — road / overlay layer

Exactly **one byte per tile**, same 112×156 grid (17472 bytes).

- `0` — nothing (95%+ of the map)
- otherwise — a **road-piece id**; piece `N` draws `ROAD.PCK` cell `N-1`

### ROAD.PCK does not use the terrain grid

This sheet is laid out differently from `SCENERY*.PCK` and it is easy to get
wrong. Its tiles are 40×40, but on a **48-pixel horizontal stride** — 40 pixels
of art followed by 8 of padding — **13 tiles per row, 2 rows**.

Reading it on the terrain sheet's 40px grid drifts 8px per column, so roads
land in the right *places* but draw the wrong *shapes* (wrong rotations, broken
joins). The giveaway in the data: content runs straddle 40px cell boundaries
(e.g. x 96–135 and 144–183), but every run fits inside a 48px cell exactly, and
cell 1's vertical bar lands dead centre at rel x 15–24.

### The ids are an enumeration, not a bitmask

Verified against the actual neighbour connectivity of every road tile in
Erythea — for each id the observed N/E/S/W neighbour mask is essentially
unanimous (e.g. id 1 → mask `EW` in 152 of 157 cases):

| id | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| cell | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
| shape | EW | NS | ✚ | ESW | NSW | NEW | NES | SW | NW | NE | ES | W | S | E | N | EW | NS |

Ids 16 and 17 carry the same connectivity as 1 and 2 but different art: they
are the **bridges** (the "Bridge" terrain in `STRING.DAT` group 128). Erythea
has 16 and 13 of them.

`ROAD.PCK` cells beyond 16 hold site graphics — ruins, towers, temples,
signposts — which `.RD` does not appear to reference.

## The `0x8000` flag

Rare and absent from Erythea entirely:

| Scenario | flagged tiles |
|---|---|
| Erythea | 0 |
| Tutoria | 3 |
| Hadesha | 6 |
| Isladia | 8 |
| Dragon | 9 |
| Sorcery | 17 |

Low bits stay in range `0..152` whether flagged or not, so it is a genuine
per-tile boolean, not part of the index. Counts this small suggest a
scenario-specific marker rather than a terrain property.

## Tile sheets

`TERRAIN0/SCENERY0.PCK` and `SCENERY1.PCK`, each 640×240 = a **16×6 grid of
40×40 cells**, so 96 tiles per sheet and 192 total:

```
sheet = index // 96      cell = index % 96
col   = cell % 16        row  = cell // 16
```

Erythea uses 111 distinct indices, max 152 — comfortably inside the two sheets.

## Usage

```sh
python3 tools/mapren.py original/ERYTHEA/ERYTHEA.MAP out.png 40   # full size
python3 tools/mapren.py original/ERYTHEA/ERYTHEA.MAP out.png 8    # overview
```

The `.RD` overlay is drawn automatically when the sibling file exists.

## Open questions

- What `0x8000` marks.
- How sites (ruins, temples) are placed — `ROAD.PCK` holds the art but `.RD`
  does not reference those cells, so positions are probably in `.SPC` (2728
  bytes) or `.SGN` (11858 bytes, "signs").
- City placement: `.CTY` is plain text descriptions only, so city *positions*
  must live in `.SCN` or `.SPC`. The city icons visible in the render come from
  the terrain tiles themselves, so the map bakes in the city graphic while the
  scenario data must separately record ownership and stats.
