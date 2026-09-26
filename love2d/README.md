# Warlords II — the engine

A from-scratch Warlords II engine in Lua. It reads the **original game's data
files** at runtime and plays a real game: move stacks, fight, take cities, set
production, end the turn, and let the computer players answer.

No art, no sound and no game data ships here: all of it is read from **your own
copy of the game** at runtime. The only exception is a handful of short
interface labels — the menu titles and item names — which live inside
`WARLORD2.EXE` rather than in a data file, and so are written out in
`warlords/menu.lua` instead of being loaded.

## Running

Needs [LÖVE 11+](https://love2d.org). Run from the repository root so that
`original/` resolves:

```sh
love love2d                        # the start screens: choose, set up, begin
love love2d ISLADIA                # straight into a scenario
love love2d ERYTHEA /path/to/data  # your own copy of the game files
```

The game opens full screen, in the display's own biggest mode — on a Mac set
to a scaled resolution, the panel's real pixels rather than a bigger picture
macOS shrinks to fit — with a notch's strip along the top left clear. The chrome is not a lookalike: the background,
buttons, regions and fonts are the game's own, read from its data files at
runtime (`docs/formats/screens.md`). How it fills a screen bigger than the
original's 640 × 480 is **not** the original's:

- The interface is drawn at a whole-number scale in the display's real pixels,
  the biggest that still fits 640 × 480, so every pixel of the art stays a
  sharp block (`display.lua`). View › Interface 1x is one game pixel to one
  pixel of the screen.
- The start screens stay the original's 640 × 480, centred.
- On the main screen the fixed pieces keep their size and move to an edge —
  the strategic map to the top right, the control panel to the bottom right,
  the bottom bar to the bottom, centred under the map — and the map takes the rest
  (`warlords/layout.lua`). The dialogs sit in a 640 × 480 frame centred on the
  screen, so each keeps the coordinates the original gives it.
- The map has a zoom of its own, also a whole number of device pixels, and
  slides smoothly rather than a tile at a time.

On a 640 × 480 screen all of this comes back to the original's layout, pixel
for pixel.

| input | does |
|---|---|
| any key or click | dismiss the start-of-turn banner, and nothing else |
| the menu bar, or a letter | open a menu and pick an item; a letter is its accelerator, `Alt`-letter the Game menu's, `Ctrl-Q` Quit |
| left click a city | open it: Info, City (rename, raze, buy production), Production, Vector |
| left click the map | pick up a stack of yours, or send the selection there |
| | a stack under orders shows its route as rings, crossed where this turn's movement runs out |
| right click the map | what is on a tile, while the button is held |
| click the strategic map | recentre the view |
| `Enter` / `Esc` | next army / quit army (done for this turn) — the first live button of the original's default and cancel lists |
| `1`–`9` | step the stack one tile, laid out like the numeric pad; `5` centres on it |
| arrows, the 3×3 pad | move the view a tile |
| drag the map | slide the view with the pointer |
| mouse wheel / `PageUp` `PageDown` / keypad `+` `-` | zoom the map in and out (not the original's) |
| View › Map 1x… / Interface 1x… | pick the map's zoom, or the interface's scale (not the original's); the one in use is ticked |
| View › Full screen / Window | the video mode: the whole screen, or a resizable window on the desktop (not the original's) |
| View › 4:3 | on or off, in either mode: only the original's 640 × 480, with black round it (not the original's) |
| `Space` | group the whole stack |
| `Tab` / `Backspace` | look at where the stack is going, and back / forget its destination |
| `Home` / `End` / `Del` | centre on the stack / put it down / walk on along its route |
| `m` | Move All: every stack under orders walks on as far as it can |
| `c` `b` `p` `v` | the city dialog on the city nearest the view's centre, in Info, City, Production or Vector |
| the five buttons above the pad | walk on, next army, quit army, fortify (dug in until picked up again), deselect |
| `Alt-E` | end the turn; the computer players then take theirs |
| `F5` / `F9` | quick save and load (not the original's) |

## Sound

The music, effects and advisor are the game's own files, played as the
Sound Blaster FM version plays them: `sound.lua` queues the `.8SN` samples
one behind another, and `musicthread.lua` synthesises the songs on a thread
of their own through `warlords/ailfm.lua` and `warlords/opl.lua`. Game ›
Settings turns Music, Effects and Speech on and off, and writes them to
`DATA/OPTIONS.SND` as the original does. What plays when is in
`docs/re/sound.md`. To hear a song on its own:

```sh
luajit tools/xmi2wav.lua SSTARTUP title.wav
```

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
luajit love2d/test/ui.lua original TUTORIA 20250918 980x615   # a bigger screen
```

**That it runs is not that it looks right.** The stub's `setColor` is a no-op
and nothing asserts what reaches the screen, so a fault that is purely one of
appearance — a border a pixel out, text drawn in the colour the thing before
it left set — passes the harness cleanly. `test/shot` is the other half: a
LÖVE project that runs the real front end on a real canvas, drives it through
the same handlers a player uses, and writes a PNG per step, to be compared
against a screenshot of the original:

```sh
love love2d/test/shot                                   # one shot of the screen
W2_OUT=/tmp/shots W2_SCRIPT=love2d/test/shot/shots.lua \
  love love2d/test/shot ERYTHEA original 12345
```

The arguments are the game's own, and passing the seed is what makes two runs
comparable. `W2_SCRIPT` names a file of `{name, fn}` steps — see
`test/shot/shots.lua`. The window is 640 × 480 unless `W2_SIZE=980x615` asks for
another size, `W2_FULLSCREEN=1` for desktop full screen, or `W2_NATIVE=1` for the
window the game opens for itself.

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
| `uidata.lua` | `JOIN.DAT`, `AREA.DAT`, `BUTTON.DAT`, `FILE.DAT`: the screen layout |
| `font.lua` | the `.FNT`/`.FIN` proportional fonts |
| `screen.lua` | the main screen: background, controls, hit regions |
| `layout.lua` | where the main screen's pieces go on a screen of any size (not the original's) |
| `menu.lua` | the menu bar and its items, laid out the original's way |
| `ai.lua` | a computer player |
| `cues.lua` | which song each moment gets, and what the advisor says |
| `xmi.lua` | XMIDI, the songs' format |
| `ailfm.lua` | the AIL AdLib driver the music was written for, ported from `ADLIB.ADV` |
| `opl.lua` | the OPL2 FM chip it plays on |

The dialogs sit in `ui/`, on top of the front end rather than inside the rules
core. `ui/kit.lua` is what they are made of — the popup frame, the fonts by the
original's numbers, armies on their rings, shields, fields, and the stack that
makes a dialog modal — and each other module is one of the original's dialogs,
with the routine it was read out of cited beside every number:

| module | dialog |
|---|---|
| `ui/city.lua` | the city dialog and its four modes |
| `ui/buyprod.lua` | Build Production |
| `ui/input.lua` | the text-entry and yes-or-no dialog (Rename, Raze) |

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

Some of the sounds (`docs/re/sound.md` lists which), and the interface for
much of what the rules core can already do:
there is no diplomacy screen and no quest log, and most of the menu's reports
and views are not built yet — those systems run, but only the engine drives
them. PLAN.md › Phase 3.5 has the list.

The computer player is honest about its limits. Its phase order, city roles,
production purposes and garrison sizes come from the original; its target
scoring, standing orders and exploring are ours, and with *Hidden Map* on it
expands far more slowly than the original would.
