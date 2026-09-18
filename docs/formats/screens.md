# `JOIN.DAT`, `AREA.DAT` and `BUTTON.DAT` — the screen layout

The original's interface is **data-driven**. Where a screen's clickable areas
are, and where every button sits, is not compiled into `WARLORD2.EXE` — it is
in two files in `DATA/`. Both use the same 14- or 33-byte record shape with a
`0x7d00` magic word marking each group's header.

Coordinates are in the game's 640 × 480 screen (`docs/re/ui.md`).

The dialog opener is `79fa:056a`. Given a **dialog id** it looks that id up in
`JOIN.DAT`, which names one `BUTTON.DAT` group and one `AREA.DAT` screen, then
copies both into the live screen descriptor at `451b:0ce0` and shows it. Its
three fatal errors — "Join data not found", "Area data not found", "Button
data not found" — name the three files in that order.

## `JOIN.DAT` — dialog table, 216 bytes

**36 entries of three u16**: dialog id, `BUTTON.DAT` group, `AREA.DAT` screen.
The mapping is not the identity — dialogs 9/12, 10/13 and 19/20 take each
other's button groups — so the join has to be followed rather than assumed.

**Dialog 0 → button group 0, area screen 2 is the main game screen.**

## `AREA.DAT` — clickable regions, 378 bytes

Seven screens, each a **14-byte header** followed by `count` **14-byte region
records**.

### Header

| off | type | meaning |
|---|---|---|
| 0 | u16 | magic `0x7d00` |
| 2 | u16 | screen id |
| 4 | u16 | bitmask of regions enabled at first show |
| 6 | u16 | number of region records that follow |
| 8 | 6 bytes | zero in every shipped screen |

### Region record

| off | type | meaning |
|---|---|---|
| 0 | u16 | region id (1–15; 0 is a dummy first entry on every screen) |
| 2 | u16 | zero in every shipped record — the runtime's enabled flag |
| 4 | u16 | x |
| 6 | u16 | y |
| 8 | u16 | width |
| 10 | u16 | height |
| 12 | u16 | zero in every shipped record |

This is the array `1726:0009` walks to decide what was clicked, back to front.

### The screens as shipped

**Screen 2 is the main game screen:**

| region | rect | what |
|---|---|---|
| 3 | `(0, 0, 640, 18)` | menu bar |
| 13 | `(0, 17, 392, 386)` | map panel |
| 2 | `(16, 30, 360, 360)` | **map viewport** — 9 × 9 tiles of 40 px |
| 1 | `(400, 30, 224, 312)` | **strategic map** — 2 px per tile of a 112 × 156 map |
| 9 | `(16, 408, 360, 56)` | bottom bar |

The other six:

| screen | regions |
|---|---|
| 0 | none (one dummy) |
| 1 | 3 `(0,0,640,18)` — menu bar only |
| 3 | 6 `(80,60,224,312)`, 14 `(308,180,224,50)` |
| 4 | 8 `(80,60,224,312)`, 12 `(304,110,256,30)` |
| 5 | 10 `(300,150,292,160)`, 11 `(275,322,314,11)` |
| 6 | 15 `(80,60,224,312)` |

Screens 3, 4 and 6 each place a 224 × 312 strategic map at `(80, 60)` — the
map-selection and editor screens.

The region rects cross-check against the art: `STRAT.PCK` is exactly
224 × 312, and the code's own tile arithmetic
(`8611:15ff`, `740d:131a`) independently gives `(16, 30)` and a 40 px tile.

## `BUTTON.DAT` — controls, 15312 bytes

**36 dialogs, 428 controls**, every record 33 bytes — the same `0x21` stride
the running program uses for a control (`docs/re/ui.md`). 36 headers + 428
controls = 464 records = 15312 bytes exactly, and each header's count matches
the gap to the next header, so the file is fully accounted for.

The dialog loader `79fa:056a` searches a table of `0x24` = 36 entries, which
is the same 36.

### Dialog header

| off | type | meaning |
|---|---|---|
| 0 | u16 | magic `0x7d00` |
| 2 | u16 | dialog id (0–35, in order) |
| 4 | u16 | number of control records that follow |
| 6 | 27 bytes | zero |

Control counts, by dialog id: 43, 15, 12, 35, 15, 4, 21, 11, 10, 6, 3, 29, 5,
5, 27, 3, 2, 2, 21, 10, 8, 3, 24, 26, 4, 2, 33, 13, 2, 14, 3, 2, 2, 4, 6, 3.

Dialog **7** is the reports screen — `6ef3:0000` opens it by that number.

### Control record

The 33 bytes are the same in the file and in memory; the file is a template
and the runtime fields are simply zero on disk.

| off | type | meaning |
|---|---|---|
| 0 | u16 | control id |
| 2 | u16 | *runtime* — zero on disk |
| 4 | u8 | *runtime* **value / state**: 0 normal, 1 pressed, 2 disabled |
| 5 | u8 | *runtime* **dirty** flag |
| 6 | u16 | x |
| 8 | u16 | y |
| 10 | u8 | width — **0 means size to the text** |
| 11 | u8 | height — likewise |
| 12 | u32 | *runtime* far pointer to the control's **text**; zero on disk |
| 16 | u16 | *runtime* — zero on disk |
| 18 | u8 | *runtime* last-drawn value, to suppress repaints |
| 19 | u16 ×2 | source x, y — state 0 (normal) |
| 23 | u16 ×2 | source x, y — state 1 (pressed) |
| 27 | u16 ×2 | source x, y — state 2 (disabled) |
| 31 | u16 | **bitmap id** the three source rects are cut from |

Verified across all 428 shipped controls: every field marked *runtime* is zero
in every record, and only the id, rect, source rects and bitmap id carry data.

### There is no control-type field

The type is **implied at paint time**, not stored. `1a0a:0005` branches on the
text pointer at +12:

- **+12 non-zero → a text control.** `1a0a:02be` measures the string
  (`21e2:0ab8`), fills width/height from it when either is 0 (`text + 4` and
  `text + 5`), paints the background, and draws the string **centred** in the
  rect. The state at +4 picks the colours: 0 and 1 both draw colour 15 on 3,
  state 2 draws colour 2 on 3 — so *disabled* is the only visually distinct
  state for text.
- **+12 zero and +31 > 0 → a sprite control.** `1771:0001` blits from bitmap
  +31 using the source rect for the current state.
- **Both zero → nothing is painted.** The control is a hit target and a value
  holder only; whatever occupies it is drawn by game code. The eighteen army
  and movement-bar slots on the main screen are all of this kind.

So a remake does not need a type enum — it needs the same rule. What a control
*does* on a click is not in the file either; that lives in the dialog's own
code, reached through the command table (`docs/re/ui.md`).

### Bitmap ids

The id at +31 indexes a runtime **bitmap registry** at `4125:1f56`, 12 bytes
per entry (flags, a far pointer to the pixels, a group id, a mask pointer),
filled on demand from `PICS/*.PCK`. The id is therefore a registry slot, not a
file number, and the id → filename mapping has to be read from the loader —
**still open**. Only 11 distinct ids appear in the whole file, and 4 covers 84
of the 156 sprite controls.

### The main game screen

Group 0's 43 controls, with `AREA.DAT` screen 2, are the whole in-game
interface:

| ids | rect | what |
|---|---|---|
| 173–178 | 24 × 22 at y 367, x 408–536 | five-button toolbar |
| 186, 187, 188 | 56 × 22 and 24 × 22 at y 396 | three wider buttons |
| 179–182 | 32 × 29 at y 426, x 408–528 | four buttons |
| 183, 184, 185 | 48 × 48 at (568, 415) | one button, three variants |
| 320–327 + 177 | 16 × 16 at x 568–600, y 361–393 | **3 × 3 pad** |
| 224–231 | 32 × 41 at y 404, x 24–304 | **eight army slots** |
| 232–239 | 40 × 24 at y 445, x 24–304 | movement bar under each |
| 240, 241 | 32 × 58 at (336, 409) | one indicator, two variants |

Ids 224–241 have no bitmap and no text: they are the stack panel the game
draws itself, inside `AREA.DAT`'s bottom-bar region `(16, 408, 360, 56)`.

### Cross-check against the art

`PICS/SCREEN0..3.PCK` are four 320 × 240 quadrants that assemble into the
640 × 480 background. Drawing the `AREA.DAT` regions and `BUTTON.DAT` control
rects over that image puts every one of them exactly on a recessed panel in
the art — the map viewport, the strategic map, the bottom bar, and the button
cluster with its 3 × 3 pad. The layout data, the code and the artwork all
agree.

## What is still missing

- **How a bitmap id (control +31) maps to a `PICS/*.PCK` file.** The registry
  is filled on demand by the loader; the id → filename table has not been
  read. This is the last thing needed to draw a dialog from data alone.
- Which control ids get text, and from which `STRING.DAT` group. The text
  pointer is assigned by each dialog's own code, so this is per-dialog work
  rather than one table.
- What the region and dialog ids mean individually, beyond the main screen.
