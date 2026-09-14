# Warlords II — one move

The smallest honest slice of the engine: it reads the **original game's data
files** at runtime, draws a scenario map at **100% scale** (40px tiles), places
one army on the player's capital, and lets you make **exactly one move**.

Then it stops. That is the whole point — it proves the decoded formats render
and drive a real game loop, without pretending to be a game yet.

## Running

Needs [LÖVE 11+](https://love2d.org). Run from the repository root so that
`original/` resolves:

```sh
love love2d                        # Erythea
love love2d ISLADIA                # another scenario
love love2d ERYTHEA /path/to/data  # your own copy of the game files
```

Arrow keys or WASD move the army one tile. Esc quits.

## No assets are bundled

Everything is decoded from your own copy of Warlords II at load time —
palette, terrain sheets, road overlay, army sprites, map, and scenario. Nothing
in this folder is derived from SSG's content.

| File | Reads |
|---|---|
| `warlords/pal.lua` | `TERRAIN0/WAR2.PAL` — 16 colours, components are percentages |
| `warlords/pck.lua` | `.PCK` — four LZ77 bitplanes, 4bpp |
| `warlords/scn.lua` | `.SCN` sides/cities, `.MAP` terrain, `.RD` roads |
| `warlords/terrain.lua` | classifies tiles so movement can be blocked |

## Rules actually implemented

Only the ones needed for a single step, all from `docs/rules.md`:

- **Ownership is derived** — a side starts owning only its capital.
- **Defence is derived** — 1 if a city produces fewer than 3 army types, else 2.
  (Shown in the status bar; nothing fights yet.)
- **Illegal moves** — off the map; into water or mountains, which land armies
  cannot enter; into a city, which must be attacked rather than entered.

Everything else — stacks, movement points, terrain costs, combat — is not here.

## Known shortcut

Terrain classification is a **heuristic** (dominant palette colour per tile),
because the game's own tile-to-terrain table has not been located. It is fine
for "can a land army stand here"; it is **not** a basis for movement costs. See
`../docs/gamestate.md`.
