# Headless scenario loader

`tools/gamestate.py` composes every decoded format into a single starting game
state and cross-checks the pieces against each other. No rendering, no UI, no
rules — this exists to prove the formats in `docs/formats/` actually fit
together into something playable.

```sh
python3 tools/gamestate.py original/ERYTHEA
python3 tools/gamestate.py --all          # exit code 0 only if every check passes
```

## What it builds

| Field | Contents |
|---|---|
| `sides` | 8 slots: name, starting gold, capital, `in_use` |
| `cities` | position, name, income, production types, **derived** owner, description |
| `sites` | temples and ruins: position, type, description |
| `signs` | signpost positions and text |
| `items` | 8 standards + 14 magic items |
| `monsters` | the 9 ruin guardians |
| `armies` | 29 army types with strength / production time / cost |
| `tiles`, `roads` | 112×156 terrain grid + 1 byte/tile road overlay |
| `terrain` | tile index → terrain class (**derived**, see caveat) |
| `strings` | the 169-group UI text corpus |

## Validation

Every scenario passes all checks:

```
[ok] map is 112x156 = 17472 tiles
[ok] road overlay is 1 byte/tile (17472)
[ok] all 80 cities stand on a city tile
[ok] all 40 sites stand on a temple/ruins tile
[ok] all 114 signs stand on a signpost tile
[ok] 8 sides in use, 8 cities owned at start (1 each), 72 neutral
[ok] every city production slot names a real army type
[ok] 0 of 111 used tiles unclassified by the colour heuristic
```

| Scenario | cities | sides in use | sites | signs |
|---|---|---|---|---|
| Erythea | 80 | 8 | 40 | 114 |
| Hadesha | 80 | 8 | 40 | 117 |
| Isladia | 80 | 8 | 40 | 88 |
| Dragon | 40 | 5 | 24 | 51 |
| Sorcery | 20 | 4 | 16 | 16 |
| Tutoria | 6 | 2 | 6 | 2 |

The checks are structural, not cosmetic: city/site/sign coordinates are
validated against the terrain tile they land on, so a wrong offset or stride
anywhere in `.SCN`, `.SGN` or `.MAP` fails loudly.

An independent confirmation against the running game: Erythea's Sirians start
with 200 gold and Mirea's income is 38; the game's turn-1 status bar reads
234gp = `200 + 38 − 4 upkeep`.

## Two derived values

**Ownership.** There is no owner field. At scenario start each side owns exactly
its capital and everything else is neutral — see `docs/formats/scenario.md`.

**Terrain class.** Classified by the dominant palette colour of each tile in
`SCENERY0/1.PCK`, plus the four tiles pinned by scenario data (signpost 0,
temple 10, ruins 12, city 96). It classifies all 92–111 tiles each scenario
uses, and the proportions are plausible (Erythea: 47% plain, 32% water, 10%
mountain, 9.5% forest). **But it is a convenience, not authority** — the game's
own tile-to-terrain table has not been found. `FILE.DAT` references
`TERRAIN%d\MAPCOLOR.DAT`, which ships with no copy; that is the most likely
home for the real mapping. Movement costs must not be built on this heuristic
until the real table is located or the rule is read out of the executable.

## What it does not do

No armies exist yet: the starting garrisons are presumably in the undecoded
16-byte per-city block, and city **defence** is likewise unlocated. So this is a
map plus an economy plus a production plan — not yet a position you can play.
