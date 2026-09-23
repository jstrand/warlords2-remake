# Warlords II — native port plan

## 0. What we're actually looking at

Findings from inspecting `original/` (424 files, 5.4 MB):

| Thing | Finding | Consequence |
|---|---|---|
| `WARLORD2.EXE` (530 KB) | 16-bit **real-mode** DOS `MZ`, built with **Borland C++ 3.x (1991)**, uses **VROOMM overlays** (`"Runtime overlay error"`, swaps through `SWAP.DAT`) | Static disassembly is possible but overlay segments are not mapped by default. This is the hard part. |
| `START.EXE` (74 KB) | Separate intro/animation player (`START/*.GFX`, `*.XMI`) | Can be skipped entirely in v1. |
| `PATCH.EXE` | Pocket Soft RTPatch utility, MS C runtime | Not game code. Ignore. |
| `INSTALL.EXE`, `IMPORT.EXE` | Borland C, installer + WL1 scenario importer | Low priority. |
| `*.PCK` | **SOLVED.** 6-byte header then four 1-bit planes, LZ77-compressed (`tools/pck.py`, `docs/formats/pck.md`) | All 92 files decode exactly. |
| `*.PAL` | **ASCII text**, `"RR GG BB"` per line, values are **percentages 0–99** | Solved — see `tools/pal.py`. 16 entries ⇒ the game is 16-colour. |
| `*.MAP` | **SOLVED.** `u16` tile grid, **112×156**, + `.RD` 1-byte/tile road overlay | Renders correctly; see `docs/formats/map.md`. |
| `*.RD` | **SOLVED.** 1 byte/tile overlay: 0 = none, else index into `ROAD.PCK` | Roads + sites. |
| `*.CTY` | **Plain text**, `#NNN\|line\|line\|` city descriptions | Done. |
| `*.SCN` | Fixed 12-byte name records (side names, army-set names) + scenario params | Easy. |
| `ARMYTYPE.DAT` | Fixed-stride record table: name + `u16` stat fields (strength, move, upkeep, cost) | **The rules goldmine.** |
| `DATA/*.DAT` | UI strings, terrain table, quotes, error text | Mostly text. |
| `WARLORD2.HLP` | **SOLVED.** 388 x 104-byte context-help records; see `docs/formats/hlp.md` | A full UI action list. |
| `*.XMI` | Miles AIL **XMIDI** (`FORM/XDIR/CAT /XMID`), + `MIDPAK`/`DIGPAK`/`*.ADV` drivers | Solved problem — ScummVM has an XMI parser. |
| `*.8SN` | Raw **unsigned 8-bit PCM**, ~centered on 0x7E | One-line conversion to WAV. |
| `SOUND/V*.TXT` | Subtitles for each advisor voice clip | Free localisation hooks. |

**The strategic conclusion:** SSG kept almost all content *outside* the executable, in formats that are plain text or flat fixed-size records. This is a **data-archaeology project with a small disassembly component**, not a decompilation project. Do not try to decompile 530 KB of overlaid Borland C. Decode the data, then disassemble only the handful of routines whose behaviour you cannot infer from playing the game.

## 1. Approach

Reimplement the engine from scratch; read the original data files at runtime. Use the DOS original as an executable spec you can diff against.

Three sources of truth, in order of cost:
1. **Play the game in DOSBox** and observe. Free. Answers 80% of rules questions.
2. **Parse the data files.** Answers unit stats, terrain, map layout, scenario setup.
3. **Disassemble.** Reserved for: combat resolution maths, AI decision-making, income/production formulas, hero/item/quest mechanics, random-map generation.

## 2. Phases

### Phase 0 — Foundations (half a day)
- `git init`, commit `original/` as pristine reference. Add `.gitignore` for build output.
- Get the game running under DOSBox-X. Verify it launches, note the `WAR2 x/h/s/m/v/n` switches from `READ.ME`.
- Record baseline video/screenshots of: main map, city dialog, combat, production screen. These are your regression targets.
- Decide licensing stance: **engine is yours, data files are not.** The port must require the user to supply their own `original/` directory. Write this into the README now so you never accidentally commit assets to a public repo.

### Phase 1 — File format archaeology (the bulk of the work)
Build a Python toolkit in `tools/` — one module per format, each with a dump-to-PNG/JSON command.

Order, easiest first (each win makes the next easier):
1. ~~`.PAL` → RGB palettes.~~ **Done** — `tools/pal.py`.
2. ~~`DATA/STRING.DAT`~~ **Done** — 169 groups / 651 strings, whole UI corpus; see `docs/formats/string.md`. `.CTY` and `SOUND/*.TXT` are plain text.
3. `.8SN` → WAV (`u8` PCM; sample rate is the unknown — try 11025, confirm by ear against DOSBox).
4. ~~`.MAP` / `.RD`~~ **Done** — 112×156 u16 tile grid + 1-byte/tile road overlay, rendered via `tools/mapren.py`. Note the 156×112 guess was wrong; see `docs/formats/map.md`.
5. ~~`.SCN` / `.SGN` / `.SPC`~~ **Mostly done** — sides (incl. starting gold and capitals), cities, sites, items, monsters and signposts all parse and verify against the map and against the running game. City owner and defence are derived at game start (`docs/rules.md`); still open: the 16-byte per-city block and the `160..2062` per-side region. See `docs/formats/scenario.md`. `.ITM` is decoded (`docs/formats/itm.md`) and read by `love2d/warlords/scn.lua`.
6. ~~`ARMYTYPE.DAT`~~ **Done** — 29 x 62-byte records; strength/time/upkeep/move/cost solved, 6 of 15 bonus fields still open. See `docs/formats/armytype.md`.
7. ~~**`.PCK`**~~ **Done** — see `docs/formats/pck.md`. Kept for the record, the original strategy was:
   - Known-plaintext attack: `CURS.PCK` (1274 B for 240×80 = 19200 px) is tiny and heavily compressed → mostly transparent. `BLACK.PCK` in `START/` is probably a solid fill → its byte stream will show the raw run-length primitive.
   - Expect a simple RLE over 8-bit palette indices with a transparent-run opcode — near-universal for 1993 VGA sprite formats.
   - If pattern-matching fails, fall back to Phase 2: breakpoint the decompressor in the DOSBox-X debugger and read it, which is ~200 lines of 16-bit asm at worst.
   - Note `PCK` files hold *sprite sheets*; you'll separately need the frame/tile subdivision, likely a fixed grid derivable from the declared width/height.
8. ~~`.GFX`~~ **Done** — not an image format at all: plain-text screen-layout markup for the credits/help/tutorial screens. See `docs/formats/gfx.md`.
9. `SAVE/` format — layout decoded from the executable (`docs/formats/save.md`), not yet checked against a real save.

Deliverable: every asset in `original/` round-trips to PNG/WAV/JSON, plus a `docs/formats/` spec per format.

### Phase 2 — Targeted disassembly
Only for rules you can't observe or infer. Keep a running `docs/rules.md`.

**Status: the rules are decoded.** `tools/exe.py flatten` rebuilds
`WARLORD2.EXE` as a flat MZ (all 69 overlays inlined, thunks redirected, x87
emulation decoded), and `tools/ghidra/` imports it with labels and a fix for
the CS-relative jump tables Ghidra gets wrong. Roughly 500 functions are named.

Decoded and written up in `docs/rules.md`: combat, movement, production, the
turn loop, city capture, garrisons, heroes, ruins/temples/sages, quests,
diplomacy, setup and difficulty, and end-of-game. `docs/formats/save.md` has
the save layout; `docs/re/` documents the method, the computer players and the
random map generator, both now decoded in full. Start at `docs/re/README.md`.

The user interface has since been decoded too: the screens, controls, command
set, menu and dialogs are in `docs/re/ui.md` and `docs/formats/screens.md`.

Still open:
- one unverified army stat — Heavy Infantry "Move 16/20" (`docs/rules.md` ›
  Still unknown); settle it in DOSBox.
- `ARMYTYPE.DAT` `+18`/`+20` and whether negative costs are hire prices
  (`docs/formats/armytype.md` › Open questions).
- the save layout's unmapped globals block and second byte-per-tile map, and a
  check against a real save made in DOSBox (`docs/formats/save.md`).
- the UI open questions in `docs/re/ui.md` (`UDB.DAT` item-to-command table,
  `.FIN` spacing fields).
- sound plumbing (deliberately skipped until Phase 4).

### Phase 2.5 — Headless loader — **DONE**

`tools/gamestate.py` builds a validated starting game state from any scenario
and passes every structural check on all six. See `docs/gamestate.md`.
The former gaps (starting garrisons, city defence, the real tile-to-terrain
table) are all resolved from the executable; see `docs/rules.md`.

### Phase 2.6 — Playable slice — **DONE**

`love2d/` is a LOVE 11 project that reads the original data files at runtime
(no bundled assets) and draws a scenario at 100% scale. See `love2d/README.md`.

### Phase 3 — Engine — **in progress**

**Lua, in `love2d/warlords/`.** The rules core is headless: it never touches
`love.*`, so `lua love2d/test/run.lua` checks it without a window (~8000
assertions). `tools/` stays Python and stays the format lab.

Done: game state and the turn loop, movement, combat, city capture and
pillage/sack/raze, heroes, ruins/temples/sages, quests, diplomacy, the hidden
map, the end-of-game conditions, saving and loading, and a computer player.
A scenario plays to victory on its own.

`rules.bugs` reproduces the original's faults, one flag per fault the engine
actually reads.

Left in the engine itself: a computer player that explores as well as the
original's (with *Hidden Map* on it expands far too slowly), and flags for the
two decoded AI faults whose phases have no counterpart here yet.

### Phase 3.5 — Interface — **in progress**

The front end (`love2d/main.lua`, with the dialogs in `love2d/ui/`) is rebuilt
on the original's own 640×480 screen, read from `AREA.DAT`/`BUTTON.DAT`/
`JOIN.DAT` at runtime (`docs/formats/screens.md`, `docs/re/ui.md`).
`love2d/test/ui.lua` drives it headless; `love2d/test/shot` renders PNGs.
Checking against the original: native captures in
`original-screenshots/native/` (made with `tools/dosbox/`, kept out of git),
compared pixel for pixel. The main screen under the turn banner and the hero
offer now differ from the original only where the two games differ.

Done: the main screen — menu bar and dropdowns, turn strip, strategic map
(four-pixel tiles, roads, city shields, view box), bottom bar, control panel;
the army slots and Grp switch; the cycle's buttons and the 3×3 pad; the
configurable buttons; the start-of-turn banner; the hero offer; the assault
and the spoils dialog; **the city dialog in all four modes** — Info, City
(Rename, Raze, Build Production), Production and Vector (send, redirect, See
All); **the five reports**; **the original's keyboard**; the fonts' true
metrics (ink widths, advances, the space).

Left, roughly in order of what blocks playing a full game from the interface:

1. **Stack splitting proper** — the slots toggle armies in and out of the
   moving group; check this covers every way the original splits and merges.
2. **Hero screens**: Inspect / army info (`,`, `6c1b`), Plant Flag (`f`) and
   vectoring to the standard, Levels (`u`), items (`t`, pick up / drop), and
   dialogs for search results at ruins, temples and sages (`z`).
3. **Quest screen** (`=`, `4976:0167`).
4. **Diplomacy screen** (`d`, `484e`).
5. **Orders**: Fight Order (`i`, `6a89`), Disband (`q`), Signpost (`x`),
   Resign (`r`).
6. **View**: Army Bonus (`o`), Items (`t`), Ruins (`.` — the city dialog's
   mode 4, a site's info), Stack (`s`).
7. **History**: City, Events, Gold, Winners, Triumphs (`h e j y l`, `6d51`).
8. **Game menu**: Settings, Shortcuts (`UDB.DAT`), New game and side setup,
   Save/Load game and map with the original's dialogs, About.
9. **Help screens**: control 188 and the `.GFX` pages (`HELP\HMOUSE.GFX`).
10. **End of game**: a proper win/lose screen (today the loop just stops).
11. Right-click tile info (`740d:131a`, the 256 × 75 popup).

Every unimplemented menu item is greyed. Each new dialog gets a step in a
`test/shot` script, and is compared with the original wherever a capture
exists.

Engine notes found on the way: the long ERYTHEA run of `test/ui.lua`
(150 rounds) takes hours in the computer players' pathfinding; the engine does
not yet vector to a planted standard (`STANDARD_DEST` is treated as none).

### Phase 4 — Audio — **not started**

Nothing plays yet; `.8SN` → WAV (Phase 1, item 3) is still undone too.
- XMI → MIDI conversion (or direct playback). Port ScummVM's XMIDI parser; it's the reference implementation.
- Playback via FluidSynth with a soundfont, or emulate OPL2/AdLib (Nuked-OPL3) for period-accurate sound. The `.ADV` driver files tell you which devices were supported.
- `.8SN` digitised sounds + advisor voice: straight PCM playback.

### Phase 5 — Modernisation (post-parity)
Window scaling/fullscreen, save-anywhere, hotseat/network, undo, larger maps, scenario editor. Don't touch any of this before parity.

## 3. Tools to install

You already have: `python3`, `git`, `brew`, `sdl2`, `sdl2_image`.

**Essential**
```sh
brew install --cask dosbox-x          # run the original; has a built-in debugger (plain dosbox does not)
brew install --cask ghidra            # 16-bit real-mode MZ disassembly, free, scriptable
brew install openjdk@21               # Ghidra dependency
brew install hexfiend                 # macOS hex editor, good for eyeballing records
brew install python@3.12 pipx         # keep tooling out of the system python
```

**Strongly recommended**
```sh
brew install rizin                    # or radare2 — fast scripted binary queries; complements Ghidra
brew install ffmpeg                   # raw PCM -> WAV/inspection, one-liner for the .8SN files
brew install fluid-synth              # MIDI playback for the XMI music
brew install cmake ninja pkg-config   # build system, once the engine starts
pipx install kaitai-struct-compiler   # declarative binary format specs -> generated parsers
python3 -m pip install --user pillow numpy construct
```
- **Pillow + numpy** — render decoded graphics to PNG; numpy makes brute-forcing the PCK compression scheme far quicker.
- **construct** or **Kaitai Struct** — write formats declaratively once, get a parser plus documentation. Worth it for a project that is 70% file formats.

**Optional / situational**
- **ImHex** (`brew install --cask imhex`) — hex editor with a pattern language and entropy/data-inspector views; better than Hex Fiend for reversing *unknown* formats.
- **Tiled** (`brew install --cask tiled`) — once `.MAP` is decoded, export to TMX and you get a free map viewer/editor.
- **Nuked-OPL3** (source, not a package) — cycle-accurate AdLib emulation.
- **ScummVM source tree** — reference implementations for XMIDI and a dozen 90s RLE image formats. Clone it; don't link against it.
- **VS Code + the Ghidra/asm extensions**, or **Cutter** (Rizin GUI) if you prefer a GUI over Ghidra.

## 4. Prior art worth reading before writing code
- **LordsAWar!** — a mature GPL Warlords II clone. Its data model (stacks, cities, heroes, quests) is a proven design you can learn from. Do not copy code if you want a different licence.
- **Warbarons** — online Warlords II-alike; useful for rules discussion.
- **The Warlords II fan/scenario-design community** — decades of forum posts documenting exact combat odds and AI quirks. Often cheaper than disassembly.
- **ScummVM / DevilutionX / OpenTTD** — models for "reimplement a 90s engine, load original assets."

## 5. Engine language — **decided: Lua on LÖVE 11**

Chosen for Phase 2.6 and kept: the rules core is plain Lua 5.1 (runs under
both `lua` and LuaJIT), the front end is LÖVE. The options weighed before that
are kept below for the record.

- **C++ / C with SDL2** — you already have SDL2 installed; closest to the original's model; largest body of reference code (ScummVM, DevilutionX).
- **Rust** (`macroquad` / `bevy`) — memory safety and a genuinely pleasant build story for a from-scratch turn-based game.
- **TypeScript + Canvas/WebGL** — makes it playable in a browser instantly, which is a large distribution win for a 1993 strategy game; but "natively runnable" was the stated goal.

(Original recommendation was Rust + macroquad or C++/SDL2; superseded.)

## 6. Legal

The engine is a clean reimplementation and is yours. The contents of `original/` are copyrighted (SSG / Strategic Studies Group, 1993) and must never be committed to a public repository or redistributed. Ship the engine; require the player to point it at their own installed copy. This is exactly the ScummVM/DevilutionX model and it is well-settled practice.
