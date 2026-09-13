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
invariant (here, the parity split), not with a similarity score.**

## `.RD` — road / overlay layer

Exactly **one byte per tile**, same 112×156 grid (17472 bytes).

- `0` — nothing (95%+ of the map)
- otherwise — an index into `TERRAIN0/ROAD.PCK`, a 16×2 grid of 40×40 cells,
  colour-keyed on its corner pixel (index 1)

Observed values are `0..17`. Rendered on top of the terrain these form
connected road networks running between cities and routing around mountains,
which is what confirms the interpretation. `ROAD.PCK`'s first row is road
pieces (straights, corners, T-junctions, crossings); its second row is site
graphics (ruins, towers, temples, signposts).

Erythea's counts for the two highest values are 16 (`value 16`) and 13
(`value 17`), which look like site counts rather than road pieces — so the
overlay layer likely carries ruins/temples as well as roads. Worth confirming
against `.SPC`.

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
- Whether `.RD` values 16/17 are sites, and how they relate to `.SPC` (2728
  bytes) and `.SGN` (11858 bytes, "signs").
- City placement: `.CTY` is plain text descriptions only, so city *positions*
  must live in `.SCN` or `.SPC`. The city icons visible in the render come from
  the terrain tiles themselves, so the map bakes in the city graphic while the
  scenario data must separately record ownership and stats.
