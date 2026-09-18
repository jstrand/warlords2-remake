# The original's user interface

How `WARLORD2.EXE` draws and drives its screen, for rebuilding the interface at
the original's own resolution. Everything here was read out of the decompiled
corpus; addresses are Ghidra addresses (flat segment + 0x1000), as everywhere
else in `docs/re/`.

This was the part deliberately skipped while the rules were being decoded, so
it is newer and thinner than `ai.md` or `random_map.md`. What is marked
**uncertain** below is uncertain.

## The screen

**640 × 480, 16 colours, planar VGA** — mode 12h.

Three independent confirmations:

- `.PCK` art is 4bpp in four 1-bit planes, and the `.PAL` files hold 16 entries
  (`docs/formats/pck.md`).
- The drawing code writes through the EGA/VGA sequencer and graphics
  controller: write mode 2, map mask `0x0f`, bit mask per pixel, row stride
  **80 bytes = 640 pixels** (`5c52:0144`, `216d:02d8`). A 40-byte/320-pixel
  stride exists in the same code behind a mode flag (`4125:34e7` = 1, 2 or 4
  selects 640), but the shipped game runs 640.
- The tile popup in `740d:131a` clamps its anchor to x ∈ [128, 512] and
  y ∈ [37, 441] before placing a 256 × 75 box, which lands flush against 640
  and 480 at both extremes.

`.GFX` layout scripts are already documented as being in 640 × 480 space
(`docs/formats/gfx.md`); this confirms it from the code side.

### Layout of the main game screen

| area | screen rect | from |
|---|---|---|
| **map viewport** | `(16, 30)`, **9 × 9 tiles of 40 px = 360 × 360** | `8611:15ff`, `8611:15da`, `8961:0000` |
| **strategic map** | `(400, 30)`, 2 px per tile → 224 × 312 for a 112 × 156 map | `8611:15ff` |
| view box on it | 18 × 19 px (the 9 × 9 viewport at 2 px/tile) | `8961:0000` |
| **menu bar** | across the top, laid out left to right | `7ae8:0052` |

Screen ↔ tile, with `3c04:0177` / `3c04:0175` the scroll origin in tiles:

```
tile_col = (screen_x - 16) / 40          screen_x = 16 + (col - scroll_x) * 40
tile_row = (screen_y - 30) / 40          screen_y = 30 + (row - scroll_y) * 40
```

`740d:131a` writes the same mapping as a tile *centre*, `(col - scroll_x) * 40
+ 36` and `(row - scroll_y) * 40 + 50` — the +36/+50 being 16+20 and 30+20,
which is what pins the origin at (16, 30) rather than somewhere else.

The 9 × 9 viewport is confirmed three ways: the on-screen tile cache
`451b:1f74` is indexed `col * 9 + row` and cleared with two 9-iteration loops
(`8611:15da`, `8611:113b`); the initial scroll is the capital minus 4 in each
axis, which centres a 9-wide view; and it is clamped to 103 / 147, which with
a 112 × 156 map leaves exactly 9 tiles.

## The graphics layer

| segment | role |
|---|---|
| `216d` | **point/rect library and the blitter.** `002e(r,x,y,w,h)` builds a rect (117 calls — rects are built everywhere), `000e(p,x,y)` a point, `01b1(r,x,y)` is the point-in-rect **hit test**. `02d8` is the low-level planar blit. |
| `2133` | line drawing: `02fe` horizontal, `0344` vertical. |
| `1e68` | plane-at-a-time VGA access — `0018(n)` selects read plane *n* and sets the write mask to `1 << n`, `003a` opens all four planes, `000a` waits for vertical retrace. This is how four-plane `.PCK` data gets into video memory. |
| `5c52` | text in the **BIOS 8 × 8 font** (`F000:FA6E`), normal or inverted. Used for plain/debug text, not the game's own lettering. |
| `24d0` | clipping and bitmap blits (called by the control painter and the text engine). |
| `22bf` | **software mouse cursor** with save-behind. `0757` hides and `07a5` shows it — 147 and 149 calls, because every drawing operation brackets itself with them. `0944` reads the buttons, `0af4` the position. `1ec0` wraps INT 33h. |

### Fonts

Three proportional fonts ship in the game's root: `TEXT`, `CHANCE17` and
`CHANCE36`, loaded together at startup by `78a8:0000`.

**The `.FNT` files are `.PCK` images** — plain glyph sheets, no new codec. They
decode with the existing `tools/pck.py` untouched:

| font | sheet | line height | rows | copies |
|---|---|---|---|---|
| `TEXT` | 320 × 135 | 15 | 9 | 3 |
| `CHANCE17` | 416 × 102 | 17 | 6 | 2 |
| `CHANCE36` | 560 × 148 | 37 | 4 | 1 |

The sheet height is an exact multiple of the line height in all three. Glyphs
run in ASCII order from `0x20`, wrapping across rows, and one full 96-glyph
set takes about three rows — so a sheet holds **several stacked copies of the
whole set**, not one. `TEXT` holds three and `CHANCE17` two; `CHANCE36` holds
one.

The copies are style variants: in `TEXT`, colour 15 and colour 4 are used for
exactly 3385 pixels each, one copy drawn in each. Colour **3 is the
background** (two thirds of every sheet), 15 is the glyph and 0 its outline.
Both `CHANCE` fonts draw every copy in 15, so their copies differ in
something other than colour, or are simply duplicates — unresolved.

The companion `.FIN` holds the metrics:

```
+0   u8   number of characters (0x60 = 96)
+1   u8   first character (0x20 = space)
+2   u16  sheet width, BIG-endian (0x140 = 320, matching the .FNT header)
+4   u8   line height          (15 / 17 / 37)
+5   u8   baseline or ascent   (12 / 12 / 25)
+6   3 × u16   small spacing values, exact meaning unconfirmed
+12  96 × u8   per-character width
+108 96 × u8   a second, byte-identical width table
+204 17 bytes  trailer, not decoded
```

The two width tables are identical in all three fonts, so the difference
between them (advance vs. ink width, most likely) cannot be told apart from
the shipped data. They are not one table per stacked copy — the copy counts
are 3, 2 and 1, and there are always exactly two tables.

The text engine is segment `21e2`: `0401` draws a string, `0ab8` measures one,
`0a8e` sums character widths, `0a53` looks up one character's width, `01f7`
blits the glyphs. `0ae3` maps a character to its glyph and passes it through
`toupper` on the way — but all three sheets carry real lowercase, so that is
presumably a fallback for characters outside the 96, not a case fold.

## The widget toolkit

The game is built on a small retained-mode GUI, not on immediate-mode drawing.
`451b:0ce0` points at the **current screen or dialog**, and everything else
works through it.

### Controls

A control is **33 bytes** (`0x21`), the same shape on disk and in memory. The
full record is in [`../formats/screens.md`](../formats/screens.md); the fields
the toolkit itself touches are the id at `+0`, the state at `+4` (0 normal,
1 pressed, 2 disabled), the dirty flag at `+5`, the rect at `+6`, and a far
pointer to the control's **text** at `+12`.

There is **no control-type field**. `1a0a:0005` decides what to paint from the
text pointer: non-zero paints a centred, auto-sized text control; zero with a
bitmap id at `+31` blits one of three state sprites; both zero paints nothing
and leaves the area to game code.

The accessors are the most-called functions in the program:

| function | does | calls |
|---|---|---|
| `18a9:066e(id, v)` | set value, mark dirty, repaint — **returns early if unchanged** | 211 |
| `18a9:03b3(id)` | clear the dirty flag | 60 |
| `18a9:05f0(id, v)` | set value and repaint unconditionally | 17 |
| `18a9:05ab(id)` | read the value | 5 |

Each walks the control array linearly comparing ids, so ids are opaque
constants rather than indices.

`1a0a:0005(control)` paints one control: `1771:0001` when it has no bitmap,
`1a0a:02be` when it does, and then — only in keyboard mode (`4125:166a`) — a
double rounded-rect **focus ring** drawn as about two dozen short line
segments.

### The event loop

`18a9:000e`, reached from `main` as the last call, and it never returns until
the game quits.

`21b5:0003` fetches an event; `21b5:01f4` builds the record:

| offset | meaning |
|---|---|
| `+0` | event type |
| `+2` | u32 payload |
| `+6` | `clock()` timestamp |
| `+10` | mouse position (point) |
| `+14` | modifier and button flags |

Flag bits, from `22bf:0944` (mouse) and `bioskey(2)` (keyboard):

| bit | meaning |
|---|---|
| `0x80` / `0x40` | left / right mouse button |
| `0x200` | shift |
| `0x100` | ctrl |
| `0x800` | alt |
| `0x400` / `0x1000` / `0x2000` | caps / scroll / num lock |

The loop reads events, ignores any type above 7, and dispatches through a
jump table on the type: mouse move, button, key, and an idle case. An idle
counter of 30000 spins triggers `7ecb:0a1b` — the attract or animation tick.

### Commands

`17be:0064(code, fromMenu)` is the command dispatcher. Before the general
path it handles two defaults: **Esc** (`0x1b`) walks a list at `4125:1514`
and **Enter** (`0x0d`) a list at `4125:155b`, each looking for the first
control that answers — the cancel and default buttons. Letters `A`–`Z` are
excluded from the general path and handled as menu accelerators.

Everything else goes through a **46-entry table of (command code, handler)**,
two parallel arrays 92 bytes apart, scanned linearly.

Ghidra puts that table in DGROUP at `4125:0337`. **It is wrong** — this is the
CS-relative jump table failure the README warns about. The disassembly reads

```
mov cx, 0x2e            ; 46 entries
mov bx, 0x337
.loop:
mov ax, word cs:[bx]    ; CS, not DS
cmp ax, [bp-2]
jz  .match
add bx, 2
loop .loop
.match:
jmp word cs:[bx+0x5c]   ; near jump, same segment
```

so both arrays live **in the code segment** `17be`, at offsets `0x337` and
`0x393`. Each handler offset points at a short stub that pushes any constant
argument and far-calls the real routine.

### The command set

Codes below `0x100` are ASCII. Codes above are `0x100 | BIOS scan code`, which
is what makes `0x11f`/`0x126`/`0x131` come out as Alt-S/Alt-L/Alt-N against
save/load/new — the check that confirms the reading.

| code | key | handler | what |
|---|---|---|---|
| `0x008` | Backspace | `8065:0fe9` | |
| `0x009` | Tab | `8065:0f3f` | |
| `0x011` | Ctrl-Q | `7721:0000` | Game › Quit |
| `0x020` | Space | `89e0:0a55` | |
| `0x02c` | `,` | `6c1b:0000` | Hero › Inspect |
| `0x02e` | `.` | *inline* | View › Ruins |
| `0x035` | `5` | `8611:0565` | map |
| `0x03d` | `=` | `4976:0167` | Report › Quest |
| `0x03f` | `?` | `7721:0084` | version |
| `0x061` | `a` | `6ef3:0000(0)` | Report › Army |
| `0x062` | `b` | *inline* | View › Build |
| `0x063` | `c` | *inline* | View › Cities |
| `0x064` | `d` | `484e:0000` | Report › Diplomacy |
| `0x065` | `e` | `6d51:0000(1)` | History › Events |
| `0x066` | `f` | `7563:09f7` | Hero › Plant Flag |
| `0x067` | `g` | `6ef3:0000(2)` | Report › Gold |
| `0x068` | `h` | `6d51:0000(0)` | History › City |
| `0x069` | `i` | `6a89:0de1` | Order › Fight Order |
| `0x06a` | `j` | `6d51:0000(2)` | History › Gold |
| `0x06b` | `k` | `6ef3:0000(1)` | Report › City |
| `0x06c` | `l` | `6d51:0000(4)` | History › Triumphs |
| `0x06d` | `m` | `1c8c:04c4` | Order › Move All |
| `0x06e` | `n` | `6ef3:0000(3)` | Report › Production |
| `0x06f` | `o` | `89e0:1e3b` | View › Army Bonus |
| `0x070` | `p` | *inline* | View › Production |
| `0x071` | `q` | `1b62:06bf` | Order › Disband |
| `0x072` | `r` | `7721:150d` | Order › Resign |
| `0x073` | `s` | `89e0:0c9c` | View › Stack |
| `0x074` | `t` | `66d4:0c21` | View › Items |
| `0x075` | `u` | `7563:1652` | Hero › Levels |
| `0x076` | `v` | *inline* | View › Vectoring |
| `0x077` | `w` | `6ef3:0000(4)` | Report › Winning |
| `0x078` | `x` | `540d:01a4` | Order › Signpost |
| `0x079` | `y` | `6d51:0000(3)` | History › Winners |
| `0x07a` | `z` | `6536:0000` | Hero › Search |
| `0x112` | Alt-E | `8065:2074` | Turn › End Turn |
| `0x116` | Alt-U | `545c:0000` | Game › Shortcuts |
| `0x11f` | Alt-S | `7721:093b` | Game › Save game |
| `0x126` | Alt-L | `7721:026b` | Game › Load game |
| `0x12c` | Alt-Z | `7721:128a` | Game › Load map |
| `0x12d` | Alt-X | `64d2:0000(1)` | Game › Settings |
| `0x131` | Alt-N | `7721:019d` | Game › New game |
| `0x132` | Alt-M | `7721:1214` | Game › Save map |
| `0x147` | Home | `8065:0f02` | |
| `0x14f` | End | *inline* | |
| `0x153` | Del | *inline* | |

The "what" column is named only where a label or the segment's own contents
Every name comes from the menu data below, which pairs each label with its
accelerator; the handler addresses are what tie the two tables together.

Two handlers take a constant and are family selectors, and the menu (below)
names every one of them:

| n | `6ef3:0000(n)` — reports | `6d51:0000(n)` — history |
|---|---|---|
| 0 | Army (`a`) | City (`h`) |
| 1 | City (`k`) | Events (`e`) |
| 2 | Gold (`g`) | Gold (`j`) |
| 3 | Production (`n`) | Winners (`y`) |
| 4 | Winning (`w`) | Triumphs (`l`) |

The history selector's *n* = 4 takes a different path from the rest, which fits:
Triumphs is its own screen (`auto_ui_triumphs`, `6d51:0a21`) rather than one of
the four graphs.

### Reading the flat image

For this build, a Ghidra address `S:O` is at file offset
`(S - 0x1000) * 16 + O + 0xC600` in `build/WAR2FLAT.EXE`. That holds for 1401
of the 1579 functions in `index.txt` (checked by matching each recorded
prologue); the ~178 that miss are overlay code, which the flattener placed
elsewhere. Always verify against `index.txt`'s `bytes=` field before trusting
an extraction — `docs/formats/exe.md`'s simpler "file 0x2000 + linear" does
**not** hold here.

### What a control does

Clicking a control ends at `17be:04d8(id)`, which is the answer to how a
button becomes an action:

```
mov bx, dx              ; the control id
sub bx, 0x64            ; minus 100
cmp bx, 0x18c
ja  done                ; ids 100..496 only
shl bx, 1
jmp word cs:[bx+0xb0c]  ; 397 entries, again CS-relative
```

So there is **no id-to-command mapping**: controls have a jump table of their
own, parallel to the keyboard one, 397 entries at `17be:0b0c`. Entries share
stubs freely, and 171 distinct stubs cover all 397 ids.

The two tables **meet at their handlers**, which is what gives a button its
meaning: control 186 and Tab both call `8065:0f3f`, control 187 and Backspace
both `8065:0fe9`, control 177 and Home both `8065:0f02`, control 240 and Space
both `89e0:0a55`.

Where a row of controls shares one stub, the stub subtracts the row's base to
get an index — the id is still in DX:

| controls | stub does | meaning |
|---|---|---|
| 320–327 | `8611:0723(id - 320)` | the **3 × 3 pad**: steps the cursor one tile |
| 224–231 | `89e0:0963(id - 224)` | the **army slot**, 0–7 |
| 232–239 | `89e0:0910(id - 232)` | the **movement bar** under slot 0–7 |
| 179–182 | `545c:0072(id - 179)` | the four unlabelled buttons, 0–3 |

The rest of the main screen resolves to one handler each: 174–178 to five
routines in `8065`, 183/184/185 (one button, three variants) to `484e:0346`
in the diplomacy segment, 186/187/188 to `8065:0f3f`/`0fe9`/`104e`, and
240/241 to `89e0:0a55`/`0a99`.

The pad's eight ids run **clockwise from north** when laid out by their screen
positions — 320 is top-centre, 321 top-right, and so on round to 327 top-left
— which is what fixes the direction order, and `8611:0723` does step the
cursor by ±1 in x, y or both.

### Hot regions on the map screen

`1726:0009` handles clicks on the main screen. It walks a region array —
**14 bytes per record**, hit-testing **back to front** so later regions win —
and switches on a region id of 1–15. Region records carry an enabled flag at
`+2`, and a `+12` field that triggers a refresh when zero.

The array is not built in code — it is loaded from **`DATA/AREA.DAT`**, which
is fully decoded in [`../formats/screens.md`](../formats/screens.md), along
with `BUTTON.DAT`, the 36 dialogs and 428 controls behind every other screen.
That file confirms the 14-byte record read here and gives all seven screens'
regions, including the main one:

| region | rect |
|---|---|
| 3 | `(0, 0, 640, 18)` menu bar |
| 13 | `(0, 17, 392, 386)` map panel |
| 2 | `(16, 30, 360, 360)` map viewport |
| 1 | `(400, 30, 224, 312)` strategic map |
| 9 | `(16, 408, 360, 56)` bottom bar |

which is an independent confirmation of the viewport and strategic-map rects
derived from the code above. The handlers this function reaches are known:
`740d:0037` for the map itself (which goes on to `attack_tile`,
`military_advisor`, the army-info panels and the tutorial hooks),
`7204:033b` for cities, and `8065:0b3b` / `8065:0e04` for two drag modes.

### Menus

`7ae8:0052` builds the menu bar from a table at `4125:1adc`, 12 bytes per
entry, into 8 menus of 46-byte records. Item width is the measured text width
**rounded up to a multiple of 8** plus 8 pixels of padding each side, and
items are packed left to right from x = 8 — so the bar's metrics fall out of
the font rather than being hard-coded.

Segment `2372` runs the pulldowns: `00f0` is the tracking loop (it pumps
events and brackets its drawing with cursor hide/show), `0047` maps a key to
a menu command, `102d` and `0ad8` paint items, `132f` handles a scrolling
menu.

## The dialogs

144 functions carry an `auto_ui_*` label, generated from the `STRING.DAT`
groups they reference (`labels.md`). That list is effectively the screen
inventory — city info, army info, production, diplomacy ratings and state,
fighting order, history, triumphs, items, quests, temples, the sage, hero
levels, reports, options, save/load, resign, pillage/sack/raze, the vector
help, the medal effect. `grep auto_ui_ build/ghidra/all_funcs/index.txt` is
the index to them.

Static screens — credits, help pages, the tutorial — are not code at all:
they are the `.GFX` markup scripts already decoded in `docs/formats/gfx.md`.

## What a remake needs, and what it does not

Almost none of this needs reimplementing faithfully. The planar VGA layer, the
software mouse cursor, the save-behind bitmaps and the dirty-flag repainting
all exist to cope with 1993 hardware, and LÖVE redraws the whole screen every
frame for free.

What is worth taking is the part that is **observable to the player**:

- the 640 × 480 screen and the rects above
- the 9 × 9 tile viewport at 40 px, and the scroll clamps
- the strategic map at 2 px per tile with its 18 × 19 view box
- the three fonts, which decode today with `tools/pck.py` and whose metrics
  are in the `.FIN` files
- the screen inventory, from the `auto_ui_*` list
- the Esc/Enter default-button behaviour, which is cheap and makes dialogs
  feel right

## Open questions

- Six commands still have a handler but no name, because they are not on any
  menu: Backspace, Tab, Space, Home, End and Del. Four of those are already
  known by other means — Tab, Backspace, Home and Space are the twins of
  controls 186, 187, 177 and 240.
- Where `7ae8:0000`'s item-to-command table is assembled, which would join
  `UDB.DAT`'s item ids to the command codes.
- Which control ids get their text from which `STRING.DAT` group — assigned by
  each dialog's own code, so it is per-dialog work rather than one table.
- What distinguishes the two identical width tables in a `.FIN`, and the 17
  trailing bytes.
- The three u16 spacing values at `.FIN` +6.

## The menu

The menu bar is built by `7ae8:0052` from a table of eight 12-byte records at
`4125:1adc`: a far pointer to the menu's items, a far pointer to its title, a
number, and the menu's index. Item lists run contiguously from one menu's
pointer to the next's, ending where the bar's own table begins.

An item list is a flat array of **far string pointers**, four bytes each — not
fixed-size records. Reading it in order, `&` means "the next string is this
item's accelerator" and `%` is a separator line. So the whole menu, with its
keys, is recoverable from the executable's data segment.

The eight menus are **SSG, Game, Order, Report, Hero, View, History, Turn**,
and their items are exactly the accelerators of the command table above:

| menu | items |
|---|---|
| SSG | About Warlords II |
| Game | Settings `alt X`, Shortcuts `alt U`, New game `alt N`, Save game `alt S`, Load game `alt L`, Save map `alt M`, Load map `alt Z`, Quit `^Q` |
| Order | Fight Order `i`, Move All `m`, Disband `q`, Signpost `x`, Resign `r` |
| Report | Army `a`, City `k`, Gold `g`, Production `n`, Winning `w`, Diplomacy `d`, Quest `=` |
| Hero | Inspect `,`, Plant Flag `f`, Levels `u`, Search `z` |
| View | Army Bonus `o`, Items `t`, Build `b`, Cities `c`, Production `p`, Vectoring `v`, Ruins `.`, Stack `s` |
| History | City `h`, Events `e`, Gold `j`, Winners `y`, Triumphs `l` |
| Turn | End Turn `alt E` |

This is what names the command table: every accelerator there now has a label,
including the eight letters that had none (`b` Build, `c` Cities, `o` Army
Bonus, `p` Production, `q` Disband, `s` Stack, `v` Vectoring, `x` Signpost).

The item ids in `UDB.DAT` (507–535) are a **third** numbering, distinct from
both control ids and command codes. `7ae8:0000` maps one to a command code by
scanning an 8-byte-per-entry table for the id and returning the code at `+6`,
but that table is assembled at run time and has not been located in the data.
The names in `UDB.DAT` line up with the menu labels above, so nothing is lost
by it for now.
