# Warlords II — the engine

A from-scratch Warlords II engine in Lua. It reads the **original game's data
files** at runtime and plays a real game: move stacks, fight, take cities, set
production, end the turn, and let the computer players answer.

Nothing here is derived from SSG's content — no art, no text, no data. You
supply your own copy of the game.

## Running

Needs [LÖVE 11+](https://love2d.org). Run from the repository root so that
`original/` resolves:

```sh
love love2d                        # Erythea
love love2d ISLADIA                # another scenario
love love2d ERYTHEA /path/to/data  # your own copy of the game files
```

| input | does |
|---|---|
| left click a stack | select it |
| left click elsewhere | walk there — and attack whatever blocks the way |
| right click | inspect a tile |
| `p` | cycle what the selected city builds |
| `space` | end the turn; the computer players then take theirs |
| `c` | centre on the selection |
| arrows / WASD | scroll |
| `y` / `n` | hire or refuse an offered hero |
| `esc` | quit |

## The rules core

Everything under `warlords/` except `pal.lua`, `pck.lua` and the drawing in
`main.lua` is **headless** — it never touches `love.*`, so the rules can be run
and checked without a window:

```sh
lua love2d/test/run.lua             # ~5500 assertions, about a second
lua love2d/test/run.lua /path/to/data
```

| module | holds |
|---|---|
| `rng.lua` | the game's `dice(n, sides, bonus)`, the single source of randomness |
| `scn.lua` | `.SCN`, `.MAP`, `.RD` and `.ITM`: sides, cities, sites, items, options, fight order |
| `armytype.lua` | `ARMYTYPE.DAT`, keyed by army type id |
| `rules.lua` | the decoded formulas — production slots, garrisons, defence — and the bug flags |
| `game.lua` | game state and the turn loop: income, production, vectoring, movement reset, battles, capture |
| `move.lua` | the cost grid, stack modes, pathfinding, walking a path |
| `combat.lua` | battle lines, modifiers, the d20 resolution, the Military Advisor |
| `hero.lua` | offers, allies, experience, promotion, death |
| `ai.lua` | a computer player |

Every rule cites where it came from: `docs/rules.md` for the rule itself, and
a Ghidra address for the routine it was read out of. If the two ever disagree,
`docs/rules.md` is the spec and this is the bug.

## The original's bugs

`rules.bugs` decides whether the engine reproduces faults found in
`WARLORD2.EXE`. They are **on** by default, so a game can be compared against
the original move for move; set one to `false` to play the game as it was
evidently meant to work. Each is documented where it is used.

## What is not here yet

Diplomacy, ruins and temples, quests, sea transport, the hidden map, and
sound. The rules for all of them are decoded — see `docs/rules.md` — they are
simply not wired up.
