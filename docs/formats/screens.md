# `AREA.DAT` and `BUTTON.DAT` — the screen layout

The original's interface is **data-driven**. Where a screen's clickable areas
are, and where every button sits, is not compiled into `WARLORD2.EXE` — it is
in two files in `DATA/`. Both use the same 14- or 33-byte record shape with a
`0x7d00` magic word marking each group's header.

Coordinates are in the game's 640 × 480 screen (`docs/re/ui.md`).

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

| off | type | meaning |
|---|---|---|
| 0 | u16 | control id |
| 2 | u16 | zero as shipped — the runtime's **value/state** |
| 4 | u16 | zero as shipped — the runtime's **dirty** flag |
| 6 | u16 | x |
| 8 | u16 | y |
| 10 | u8 | width |
| 11 | u8 | height |
| 12 | 7 bytes | **not decoded** — control type and flags live here |
| 19 | u16 ×2 | source x, y — state 1 |
| 23 | u16 ×2 | source x, y — state 2 |
| 27 | u16 ×2 | source x, y — state 3 |
| 31 | u16 | bitmap id the three source rects are cut from |

The three source rects are a button's **normal / pressed / disabled** art, cut
from one sheet at `width × height`. Most differ only in source y, by exactly
the control's height — three stacked rows in the sheet — which is what
identifies them as states rather than three separate pictures.

The offsets at +2 and +4 line up with the value and dirty bytes the running
program reads at `+4` and `+5` of a control; the file's own +2/+4 are zero in
every shipped record, so the on-disk record is a template that the loader
expands. **The 7 bytes at +12 are the main gap** — they must carry the control
type (push button, checkbox, radio, list, static text, slider), and nothing
useful can be drawn without them.

## What is still missing

- The 7 flag bytes at control +12 — the control **type**.
- How a bitmap id (control +31) maps to a `PICS/*.PCK` file.
- Which control ids carry text, and how they reach `STRING.DAT`.
- What the region and dialog ids mean individually, beyond the main screen.
