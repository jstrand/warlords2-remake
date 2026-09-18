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
| `F5` / `F9` | save and load |
| `esc` | quit |

## The rules core

Everything under `warlords/` except `pal.lua`, `pck.lua` and the drawing in
`main.lua` is **headless** — it never touches `love.*`, so the rules can be run
and checked without a window:

```sh
lua love2d/test/run.lua             # ~6500 assertions, about a second
luajit love2d/test/run.lua          # what LOVE actually runs -- check both
lua love2d/test/run.lua /path/to/data
```

The front end can be exercised without a window too. `love2d/test/ui.lua`
stubs enough of the LÖVE API to load `main.lua` and click, key, save, load
and play a whole game through it — it asserts nothing about what is drawn,
only that the code runs:

```sh
luajit love2d/test/ui.lua           # Tutoria
luajit love2d/test/ui.lua original ERYTHEA
```

**Write for Lua 5.1.** LÖVE embeds LuaJIT, so the engine avoids `//` and the
`&`/`|` operators — they parse under a modern `lua` binary and then fail to
load in the game. Use `math.floor`, and the `has`/`with` helpers in `move.lua`
for the cost grid's flags.

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
| `site.lua` | ruins, temples and sages: contents, searching, blessings |
| `quest.lua` | taking a quest, checking it off, the reward |
| `diplomacy.lua` | the pair matrix, proposals, the diplomatic rating |
| `save.lua` | saving and loading a game in progress |
| `ai.lua` | a computer player |

Every rule cites where it came from: `docs/rules.md` for the rule itself, and
a Ghidra address for the routine it was read out of. If the two ever disagree,
`docs/rules.md` is the spec and this is the bug.

## The original's bugs

`rules.bugs` decides whether the engine reproduces faults found in
`WARLORD2.EXE`. They are **on** by default, so a game can be compared against
the original move for move; set one to `false` to play the game as it was
evidently meant to work.

A flag only exists once the engine actually reads it — a test enforces that —
so the list is shorter than the list of bugs in `docs/re/`. Two decoded AI
faults have no flag yet because the phases they live in have no counterpart
here.

## What is not here yet

Sound, and the interface for most of what the rules core can already do:
there is no city dialog, no way to split a stack, no diplomacy screen and no
quest log — those systems run, but only the engine drives them.

The computer player is honest about its limits. Its phase order, city roles,
production purposes and garrison sizes come from the original; its target
scoring, standing orders and exploring are ours, and with *Hidden Map* on it
expands far more slowly than the original would.
