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
| 19 | u16 ×2 | source x, y — state 0 (**active**) |
| 23 | u16 ×2 | source x, y — state 1 (**normal**) |
| 27 | u16 ×2 | source x, y — state 2 (**disabled**) |
| 31 | u16 | **bitmap id** the three source rects are cut from |

The sprite painter indexes the pairs arithmetically — `1771:0001` reads
source x from `+0x13 + 4·state` and y from `+0x15 + 4·state` — so pair *n* is
state *n*, with no table.

**State 1, not 0, is the resting appearance.** Cropping the three rects of one
button out of `BUTTON.PCK` shows state 0 as a red icon on a pressed frame,
state 1 as a black icon on a raised frame, and state 2 as a grey icon on a
flat frame. The order holds for every button checked, including the wide
`Drop It` / `Take It` text buttons, whose three variants sit side by side
rather than stacked. Since the shipped records all carry `+4 = 0`, a dialog
that did not set its controls' states would come up entirely lit — the state
is always assigned by the dialog's own code.

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

### Bitmap ids — `DATA/FILE.DAT` group 3

The id at +31 indexes a runtime **bitmap registry** at `4125:1f56`, 12 bytes
per entry (flags, a far pointer to the pixels, a group id, a mask pointer),
filled on demand by `1997:03a9`. That loader gets the file name from
`get_file_string(3, id)` — so the id is simply an index into **`FILE.DAT`
group 3, which holds exactly 79 `.pck` names**.

`FILE.DAT` has the same layout as `STRING.DAT`, so `tools/string_dat.py`
reads it unchanged (89 groups; group 3 is the bitmaps, and the rest name the
game's other data and music files).

The directory comes from the registry's flag byte, not the name:

| flags & 3 | directory |
|---|---|
| 1 | a fixed path constant |
| 2 | `TERRAIN<n>\`, with *n* the scenario's terrain set |
| else | a second fixed path constant |

which is why `a0.pck`, `scenery0.pck`, `road.pck` and `movebar0.pck` resolve
into `TERRAIN0/` while `button.pck` and `popup.pck` resolve into `PICS/`.

**Validated:** for all 428 controls, resolving the bitmap id through group 3
to a real file and checking all three source rects against that file's
dimensions gives **468 rects in bounds, none out of bounds, and no name that
fails to resolve**.

Of the 11 ids the file uses, 4 (`button.pck`) covers 84 sprite controls,
63 (`citybu.pck`) 16, 31 (`setupbu.pck`) 13 and 61 (`dbutton.pck`) 11.

### The main game screen

Group 0's 43 controls, with `AREA.DAT` screen 2, are the whole in-game
interface:

| ids | rect | what |
|---|---|---|
| 173–178 | 24 × 22 at y 367, x 408–536 | **the army cycle**: walk on, next, quit army, fortify, deselect (`../re/ui.md` › The army cycle) |
| 186, 187, 188 | 56 × 22 and 24 × 22 at y 396 | three wider buttons |
| 179–182 | 32 × 29 at y 426, x 408–528 | four buttons |
| 183, 184, 185 | 48 × 48 at (568, 415) | one button, three variants |
| 320–327 + 177 | 16 × 16 at x 568–600, y 361–393 | **3 × 3 pad** |
| 224–231 | 32 × 41 at y 404, x 24–304 | **eight army slots** |
| 232–239 | 40 × 24 at y 445, x 24–304 | the **tick or cross** under each |
| 240, 241 | 32 × 58 at (336, 409) | the **Grp** button, two directions |

Ids 224–241 have no bitmap and no text: they are the stack panel the game
draws itself, inside `AREA.DAT`'s bottom-bar region `(16, 408, 360, 56)`.
What it draws there, and what each of them does to the group that moves, is
in [`../re/ui.md`](../re/ui.md) › The army slots.

### Cross-check against the running game

`PICS/SCREEN0..3.PCK` are four 320 × 240 quadrants that assemble into the
640 × 480 background. Compositing that background and then blitting every
sprite control at its state-1 source rect **reproduces the real screen**: the
five-button toolbar, the three-button row, the 3 × 3 order pad and the crossed
swords all land where a DOSBox screenshot has them, in the same art. The
`AREA.DAT` regions land exactly on the recessed panels in the same image.

Two things in that screenshot do **not** come from this data, and are drawn by
game code: the menu bar's text, and the turn counter and player shields to its
right. The four 32 × 29 buttons on the cluster's bottom row have deliberately
blank art here — their icons are overlaid separately.

## What is still missing

Nothing now blocks drawing a screen from this data. What is left is per-screen
detail:

- Which control ids get text, and from which `STRING.DAT` group. The text
  pointer is assigned by each dialog's own code, so this is per-dialog work
  rather than one table.
- The icons drawn on the four configurable buttons (see below) — their art is
  blank because the icon depends on what is assigned, and where that icon
  comes from is not yet known.
- The turn counter and the row of player shields at the right of the menu bar.
  They are drawn by game code, not controls, and their art is not the obvious
  candidate: in a screenshot they measure about 16 px, while `SHIELDS.PCK` is
  on a 40 px stride and `BSHIELD.PCK` on a 32 px one.
- What most controls **mean**. How a control becomes an action is now decoded
  — a jump table of its own at `17be:0b0c`, see `../re/ui.md` — and several
  share handlers with keyboard commands, which names them. Most still do not
  have a name.
- What the region and dialog ids mean individually, beyond the main screen.

## `UDB/UDB.DAT` and `UDB/UDB.CUR` — the configurable buttons

Four of the main screen's buttons (control ids 179–182) are **user
assignable**, which is why their art in `BUTTON.PCK` is blank and why they
resist being named from the layout alone.

Clicking one reaches `545c:0072(n)`, whose segment is labelled
`auto_str_menu_shortcuts`. It reads the menu item assigned to button *n*,
turns it into a command code with `7ae8:0000`, and runs that through
**`17be:0064` — the same dispatcher a key press uses**. So a shortcut button
is exactly a key press by another route.

**`UDB.CUR`** is 8 bytes: four u16, the menu item on each button. As shipped:

| button | control | item | name |
|---|---|---|---|
| 0 | 179 | 521 | Search |
| 1 | 180 | 507 | Move All |
| 2 | 181 | 517 | Heroes |
| 3 | 182 | 535 | End Turn |

**`UDB.DAT`** is 21 records of 68 bytes, listing every item that may be
assigned:

| offset | type | meaning |
|---|---|---|
| `+0` | u16 | 21 on the first record, 29 on the rest — unread |
| `+2` | u16 | menu item id |
| `+4` | `char[50]` | NUL-terminated name |
| `+54` | u16 × 4 | the resting icon: x, y, w = **64**, h = **29** |
| `+62` | u16 × 3 | the greyed-out icon: x, y, w = **32** |

The rects are into **`PICS/MENUBUTT.PCK`**, 320 × 197 — a 10 × 7 grid of
32 × 29 cells, 32 apart across and **28** apart down, so vertical neighbours
share a border row. The resting rect is 64 wide because it spans a *pair*:
the resting icon and, one cell to its right, the lit one. So an item's three
states are at `(x, y)`, `(x + 32, y)` and the greyed pair.

That accounts for the sheet exactly. The 21 items name 63 distinct cells, three
each, and no cell twice; cells 42–48 — the gap between the 21 pairs and the 21
greyed icons — are the flat blue filler you can see in the decoded sheet.

`545c:030a` is what draws them: for each of the four buttons it looks the
assigned item up in `UDB.DAT` and blits bitmap **43** (`MENUBUTT.PCK`, via
`FILE.DAT` group 3) at that rect. The icon is a whole button, frame and all,
which is why the `BUTTON.PCK` art the layout points at is blank — the icon is
painted over it. An unassigned button falls back to a constant rect pair in
`4125:049e`.

The 21 names, and which cell each rests on:

| id | name | cell | id | name | cell | id | name | cell |
|---|---|---|---|---|---|---|---|---|
| 507 | Move All | 4,3 | 516 | Quest Report | 4,1 | 527 | Vectoring | 8,0 |
| 508 | Disband | 6,3 | 517 | Heroes | 0,1 | 528 | Ruins | 2,1 |
| 510 | Army Rpt | 2,2 | 518 | Plant Flag | 8,1 | 529 | Stack | 8,3 |
| 511 | City Rpt | 4,2 | 521 | Search | 0,2 | 530 | City History | 6,1 |
| 512 | Gold Rpt | 6,2 | 524 | Build | 2,0 | 531 | Event History | 8,2 |
| 513 | Prod Rpt | 0,3 | 525 | City Info | 4,0 | 534 | Triumphs | 0,4 |
| 514 | Win Rpt | 2,3 | 526 | Production | 6,0 | 535 | End Turn | 0,0 |

(column, row of the resting icon; the lit one is the next column.) The shipped
four are the `?` of Search, the two chevrons of Move All, the armoured figure
of Heroes and the shield of End Turn — which is how the assignment was checked
against a screenshot of the original.

These ids are **menu item** numbers, not the command codes of `docs/re/ui.md`
and not control ids; `7ae8:0000` is what maps between them, and that mapping
has not been read yet. The names are also useful on their own: five of them
(Army, City, Gold, Prod, Win) confirm that the reports dialog has exactly the
five tabs its `6ef3:0000(n)` argument implies.

## Dialog 6 — the city screen

`7204:0000` pushes **6** to the dialog opener when a click lands on a city, so
dialog 6 is the city screen: button group 6 and area screen 3. Checked against
a screenshot of the running game, its rect is `(80, 60)` 480 × 320.

| what | rect | from |
|---|---|---|
| the strategic map | `(80, 60)` 224 × 312 | area screen 3, region 6 |
| production choices | `(312/360/408/456, 142)` 32 × 70 | controls 197–200 |
| Stop | `(504, 140)` 40 × 36 | control 202 |
| action buttons | `(312/360/408/456, 327)` 40 × 40 | controls 193–196 |
| Done | `(512, 327)` 40 × 40 | controls 192, 201 |

The left panel is the **strategic map** — 224 × 312 is the 112 × 156 map at two
pixels a tile, the same as on the main screen — not a map-selection widget as
this document first guessed from the rect alone. Screens 4 and 6 place the same
rect, so they are probably the same thing.

`CITYBU.PCK` (416 × 120) is the dialog's buttons: a 40-pixel grid, three rows
for the three states, holding the four action buttons, Done, Stop, Raze,
Rename, Build Prod and See All.

The picture beside the statistics is **`BIGARMY.PCK`** — one 128 × 128 image
used as the symbol for *this city is producing*, the same whatever is being
built, not a per-type illustration. `CITY.PCK` (320 × 312) is a separate
illustration of a castle gateway and does not appear on this dialog.

**`ABITS.PCK`** (480 × 40) opens with **nine rings, each 32 × 30 on a 32-pixel
stride** from x = 0: grey, then white, yellow, orange, red, green, blue, light
blue and black. A production choice sits on the grey ring, and the one being
built on its owner's — so the ring is the side's colour. Cell 0 being grey puts
a side's ring at its index plus one, which matches a screenshot where the
second shield's side rings in yellow, cell 2.

The stride is **not** 40, and the rings are **not** 40 tall: the rest of the
sheet holds unrelated bits, and below y = 30 there is a strip of small pieces —
"Group Move", digits, terrain marks. A 40 × 40 cell drags both into the ring
and the clipping is obvious on screen.

The name is drawn in `CHANCE36`, centred over the right panel; everything
else on the dialog is `CHANCE17`.

### The dialog's four modes

The row of buttons along the foot (193–196) does not act on the city: it
**switches the dialog between four modes**, each showing a different set of
controls over the same frame. The button for the mode you are in is drawn lit.

| mode | button | shows |
|---|---|---|
| Info | 193 | income, defence, owner, the city's description, and the production list |
| City | 194 | Rename (203), Raze (205), Build Prod (204), each with a line of text |
| Production | 195 | the production list, Stop (202), `BIGARMY.PCK` and the chosen type's numbers |
| Vector | 196 | Current / Next turn / Turn after rows, vector and change-destination buttons (210, 211), See All (212) |

That is why 203–205, 210–212 and 214–216 never appear together: they belong to
different modes. 214–216 share their rects with 210–212 and are the second
variant of each.

The production list (197–200) is shown by the Info and Production modes only.

### The rings, again

The ring beside **Current** is always grey. Only the entry in the *list* that
is being built takes the owner's colour.
