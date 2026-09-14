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
5. ~~`.SCN` / `.SGN` / `.SPC`~~ **Mostly done** — sides (incl. starting gold and capitals), cities, sites, items, monsters and signposts all parse and verify against the map and against the running game. Still open: city owner/defence, and the 16-byte per-city block. See `docs/formats/scenario.md`. `.ITM` untouched.
6. ~~`ARMYTYPE.DAT`~~ **Done** — 29 x 62-byte records; strength/time/upkeep/move/cost solved, 6 of 15 bonus fields still open. See `docs/formats/armytype.md`.
7. ~~**`.PCK`**~~ **Done** — see `docs/formats/pck.md`. Kept for the record, the original strategy was:
   - Known-plaintext attack: `CURS.PCK` (1274 B for 240×80 = 19200 px) is tiny and heavily compressed → mostly transparent. `BLACK.PCK` in `START/` is probably a solid fill → its byte stream will show the raw run-length primitive.
   - Expect a simple RLE over 8-bit palette indices with a transparent-run opcode — near-universal for 1993 VGA sprite formats.
   - If pattern-matching fails, fall back to Phase 2: breakpoint the decompressor in the DOSBox-X debugger and read it, which is ~200 lines of 16-bit asm at worst.
   - Note `PCK` files hold *sprite sheets*; you'll separately need the frame/tile subdivision, likely a fixed grid derivable from the declared width/height.
8. ~~`.GFX`~~ **Done** — not an image format at all: plain-text screen-layout markup for the credits/help/tutorial screens. See `docs/formats/gfx.md`.
9. `SAVE/` format — **defer**. Save compatibility is a nice-to-have, not a v1 goal.

Deliverable: every asset in `original/` round-trips to PNG/WAV/JSON, plus a `docs/formats/` spec per format.

### Phase 2 — Targeted disassembly
Only for rules you can't observe or infer. Keep a running `docs/rules.md`.

Handling the overlays:
- Load `WARLORD2.EXE` into Ghidra as 16-bit real-mode x86. The root segment analyses fine.
- Overlaid functions are called via `INT 3F` stubs followed by a segment/offset pair — recognise these and you can map the call graph even before resolving the targets.
- **Easier alternative:** go dynamic. Run in DOSBox-X, use its built-in debugger (`debug` build) to break on interesting code and dump live memory once the relevant overlay is paged in. For combat maths, breakpointing on the RNG call and single-stepping the battle loop is dramatically faster than reading cold disassembly.
- Highest-value targets, in order: (a) combat resolution, (b) city income/production, (c) movement cost table, (d) AI turn logic, (e) random map generator, (f) hero/quest/item rules.

### Phase 2.5 — Headless loader — **DONE**

`tools/gamestate.py` builds a validated starting game state from any scenario
and passes every structural check on all six. See `docs/gamestate.md`.
Remaining gaps before it is a *playable* position: starting garrisons, city
defence, and the real tile-to-terrain table.

### Phase 2.6 — Playable slice — **DONE**

`love2d/` is a LOVE 11 project that reads the original data files at runtime
(no bundled assets), draws a scenario at 100% scale, and allows exactly one
move. Verified running: map, roads, cities and army render correctly, and the
three implemented rules (no water, no mountains, cities must be attacked) all
fire. See `love2d/README.md`.

### Phase 3 — Engine
Data model → turn loop → rendering → input → AI. Build headless first: load a scenario, run a turn, assert state, *then* draw it.

Engine/language is an open decision (see below). Whatever you pick, keep `tools/` (Python) and the game engine separate — the Python side stays the format lab forever.

Milestones: render Erythea's map → move a stack → take a city → combat → production → one working AI opponent → full scenario playable to victory.

### Phase 4 — Audio
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

## 5. Open decision: engine language

Not needed until Phase 3, but it shapes the codebase:
- **C++ / C with SDL2** — you already have SDL2 installed; closest to the original's model; largest body of reference code (ScummVM, DevilutionX).
- **Rust** (`macroquad` / `bevy`) — memory safety and a genuinely pleasant build story for a from-scratch turn-based game.
- **TypeScript + Canvas/WebGL** — makes it playable in a browser instantly, which is a large distribution win for a 1993 strategy game; but "natively runnable" was the stated goal.

Recommendation: **Rust + macroquad**, unless you want the ScummVM reference code to be directly copy-pasteable, in which case C++/SDL2.

## 6. Legal

The engine is a clean reimplementation and is yours. The contents of `original/` are copyrighted (SSG / Strategic Studies Group, 1993) and must never be committed to a public repository or redistributed. Ship the engine; require the player to point it at their own installed copy. This is exactly the ScummVM/DevilutionX model and it is well-settled practice.
