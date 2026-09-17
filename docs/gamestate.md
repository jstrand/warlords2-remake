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

**Terrain.** Each tile's terrain type comes from the table the game itself
uses: 255 bytes at `.SCN` `0x710` (tile index → type 0–11: road, bridge, water,
shore, forest, hills, mountains, plain, marsh, tower, city, site). The earlier
palette-colour heuristic is gone. `gamestate.py` also carries the executable's
movement cost and combat class for each type. Roads exist only in the `.RD`
overlay, and terrain type 9 ("tower") is exactly the signpost tiles.

## Start-of-game setup

`apply_game_start(g, seed=0, neutral_cities=1)` applies what the game applies
when a scenario starts, following `docs/rules.md`:

- Navy is dropped from every city's production list.
- Each slot's stats (strength, time, cost, move) are copied from
  `ARMYTYPE.DAT` and randomly nudged, then the slots are sorted by purchase
  price.
- City defence is derived from the remaining slot count.
- Each city gets **one garrison army**: capitals at purpose 3, neutral cities
  at a purpose rolled from the *Neutral Cities* option, or a placeholder
  Scouts when the option is off.

The seed makes a position reproducible. What it still doesn't do: ruin
contents, hero offers, and the turn loop itself.
