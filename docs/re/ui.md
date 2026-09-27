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

The companion `.FIN` holds the metrics — two width tables, the glyphs' ink
widths and their advances, and each copy's colours; the full layout is in
[`../formats/font.md`](../formats/font.md).

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
`1a0a:02be` when it does. Then, while a dialog is up (`4125:166a`, set by
each dialog as it opens and cleared as it closes) and the control is one of
the **default buttons** (its id is in the Enter list at `4125:155c`, tested
by `17be:0037`), it draws a **default ring**: two rounded rects in colour 0,
the inner one a pixel clear of the control, each 12 runs of `2133:02fe`
(across) and `2133:0344` (down). There is no focus that moves; every default
button on the dialog wears the ring. Checked pixel for pixel against the
hero offer's OK.

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

### The keyboard on the main screen

Besides the command table, `17be:0064` hands two more ranges to
`17be:0444` while the main screen is up (a 13-entry table at `17be:04a4`):

| key | handler | does |
|---|---|---|
| `1`–`9` | `1c8c:0197` | step the selected stack one tile, laid out like the numeric pad (`8` north, then clockwise); `1c8c:041f` forgets its destination. `5` centres on it (`8611:0565`) |
| arrows | `8611:0723(0/2/4/6)` | the 3 × 3 pad's handler: step the **cursor** and recentre the view on it — it moves the view, not the stack |

and the six keys that are on no menu are known now: **Tab** is control 186
(`8065:0f3f`: centre on where the stack is going, or back on the stack),
**Backspace** 187 (`8065:0fe9`: forget the destination), **Home** 177 (centre),
**Space** 240 (group the whole stack), **End** puts the stack down
(`1b62:08b3`, as 178 does) and **Del** walks on along the route
(`1c8c:01fd`, as 173 does). Enter and Escape are Next Army (174) and Quit Army
(175) through the default and cancel lists; **nothing on the main screen quits
the game but Game › Quit, Ctrl-Q.**

The cursor (`3c04:017d`) is always the tile the view is centred on:
`8611:0629` clamps it to 4–107 and 4–151 and puts the scroll origin four tiles
up and left of it.

The View menu's **Cities, Build, Production and Vectoring** (`c b p v`) are
inline cases that open the city dialog on the city nearest the cursor, in
modes 0, 1, 2 and 3 — any city the side has seen for Cities, one of its own
for the rest (`828e:04fa`) — and **Ruins** (`.`) does the same in mode 4 for
the nearest ruin or temple.

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
| 232–239 | `89e0:0910(id - 232)` | the **mark** under slot 0–7 |
| 179–182 | `545c:0072(id - 179)` | the four **configurable** buttons, 0–3 |

Those four are also painted twice: `545c:030a` blits each one's icon from
`MENUBUTT.PCK` over the blank `BUTTON.PCK` art the layout gives it, at the rect
the assigned menu item carries in `UDB.DAT` (`../formats/screens.md`).

The rest of the main screen resolves to one handler each: 174–178 to five
routines in `8065`, 183/184/185 (one button, three variants) to `484e:0346`
in the diplomacy segment, 186/187/188 to `8065:0f3f`/`0fe9`/`104e`, and
240/241 to `89e0:0a55`/`0a99` — one rect, the **Grp** button, and the two
directions of the same switch (see *The army slots* below).

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

### Which buttons are live

`8065:0174` is the refresh the original runs after **every** action. It walks
the main screen's controls and sets each one to 1 or 2 — normal or greyed —
through `18a9:066e`, and these are its rules:

| control | live when |
|---|---|
| 173 walk on | a selection **and** route left to walk (`4125:2ea6 > 4125:2ea8`) |
| 174 next army | `8c07:09e7()` — some army is still in the cycle |
| 175 quit army | that **and** a selection |
| 176 fortify | a selection |
| 177 pad centre | a selection |
| 178 deselect | a selection |
| 186 | a selection |
| 187 | a selection with a move target |
| 188 | always |
| 240 group all | a selection, and **not** all of it grouped (`89e0:0567`) |
| 241 ungroup | a selection, and all of it grouped |
| 224–231, 232–239 | slot below the stack's size; the rest are cleared outright |
| 320–327 | always |
| 183/184/185 | the diplomacy option, then the side's own flags pick which of the three |

`8c07:09e7` is `8c07:040e` with its side effects taken out: the same filter —
the side's armies, on the map, in the cycle, not done — asking only whether
any is left. The four configurable buttons are refreshed separately, at the
end, by `545c:00aa`: each is greyed when `2372:0f9f` says its assigned menu
item is not available.

### The army cycle

The five buttons above the pad are the turn's rhythm. The first walks the
selection on along the route it already has — `1c8c:01fd` with the army's own
move target — and the other four work the cycle that hands you your armies one
stack at a time:

| x | control | icon | handler | what |
|---|---|---|---|---|
| 408 | 173 | legs | `1c8c:01fd(target)` | **walk on** along the planned route |
| 440 | 174 | arrow | `8065:0ec9` → `8c07:019b` | **next** army |
| 472 | 175 | `!` | `8065:0ed7` → `8c07:0393` | **quit army**: done for this turn, then next |
| 504 | 176 | crossed swords | `8065:0ef4` → `8c07:03a6` | **fortify**, then next |
| 536 | 178 | flag with an x | `8065:0f26` → `1b62:08b3` | **deselect** |

Three bits of the army's flags word at `+0xc` decide who is still to be
offered, and the difference between the middle two buttons is **which bit they
set**:

| bit | meaning | set by | cleared by |
|---|---|---|---|
| `0x0001` | in the cycle | selecting a stack (`89e0:000a`) | **fortify** (`8c07:03a6`) |
| `0x0040` | done for this turn | **quit army** (`8c07:08fb`) | the start of the side's turn (`8c07:0113`) |
| `0x0200` | already offered this pass | selecting (`8c07:06eb`), and Move All | a full wrap of the cycle, and the turn's start |

`8c07:0113` — run from the start-of-turn path in `8cc6` — clears `0x40` and
`0x200` for every army of the side and puts the cycle's cursor back at the
capital. It does **not** touch `0x0001`, and that is the whole difference:
"done" wears off with the turn, while an army that has dug in stays out of the
cycle until it is picked up again, which is what puts the bit back.

Fortify also writes `1` to the army's group byte at `+0x11`, the same byte the
slot bar groups on (› The army slots).

**Choosing the next one**, `8c07:040e`: among the side's armies that are on
the map, in the cycle and not done, it takes the **nearest** to where the
cycle last stopped — Manhattan distance from `4125:3250`/`3252`, which starts
each turn at the capital and moves to each stack as it is offered. An army at
exactly that spot scores `0x2328` instead of 0, so "next" never hands back the
stack you are standing on. Armies already offered this pass are held in
reserve: when nothing unoffered is left, the nearest of them is taken, the
`0x200` marks are wiped, and the cycle goes round again.

### Walking

A stack walks **one tile at a time**, and the map shows where it is going.

The destination is kept in the army record itself — the move target at
`+0x12`/`+0x14`, `-1` for none — so it outlives a walk that could not finish.
`1c8c:041f` clears it for the whole selection when the player steps the stack
by hand; **Order › Move All** (`1c8c:04c4`) does the opposite, walking every
stack that still has one, stepping through the armies with a cursor of its own
so a stack that has run out of movement cannot hold the loop up.

**The route.** `stack_movement_mode` pathfinds to the target and
`1c8c:0963(x, y, dirs)` turns the pathfinder's direction bytes into tiles,
writing them to `451b:2515` (x) and `451b:2517` (y), `4125:2ea6` of them, with
the destination also at `451b:2511`/`2513`. Three counters drive what is
drawn:

| | |
|---|---|
| `4125:2ea6` | how many tiles the route has |
| `4125:2ea8` | how far along it the stack already is — the rings start here |
| `4125:2eaa` | how many steps this turn's movement can afford (`1555:18be`) |

`8611:2ef5` marks them into the on-screen tile cache, and `8611:1a79` blits
them:

- every route tile from `2ea8` up to but **not including the last** takes a
  ring at the tile's `(+16, +13)`: **plain** while its index is below `2eaa`,
  **crossed** once it is not — that is the "I cannot get that far this turn"
  mark;
- the last tile takes a **ghost of the leading army** instead, its type's
  ordinary cell at the tile's `(+8, +5)`.

All three come out of **`ASHADOW.PCK`**, the same ghost sheet the army slots
use: the plain ring is the 16 × 14 cell at `(496, 31)` and the crossed one at
`(496, 48)`, at the right-hand end past the army cells.

**The walk itself**, `1a8b:04c8`: one tile per pass, and each pass moves the
armies, bumps `4125:2ea8` so the route drawn behind the stack shortens as it
goes, **re-centres the view on the stack** (`8611:0565` → `8611:0629`) and
waits a couple of clock ticks. `8611:0629` is the centring everything uses:
it clamps the tile to 4..107 and 4..151, puts the scroll origin four tiles up
and left of it, and remembers it as the cursor tile at `3c04:017d`/`017f` —
which is what the white box is drawn around.

### A stack on the map, and the selection box

`8611:0335(dst, side, type, count, col, row)` draws a stack on its tile: the
top army's figure, 32 × 29 from `((type % 16) · 32, (type / 16) · 30)` of the
side's army sheet, at **(8, 7)** in the tile; then the flag pole, three
40-pixel lines down x + 2, 3 and 4 in colours 14, 13 and 14; then the flag,
48 × 8 from the same sheet at `(464, y)`, bigger the more armies stand there:
y = 29, 38, 47, 56 for one to four (`4125:2d4e`). A stack of more than four
flies the smallest flag 8 lower down as well, and the flag for the rest at
the top.

The selected stack is boxed by **`177b`**, off the clock rather than the map
redraw. `177b:0020(x, y, mode, …)` sets the box up — `828e:0afd` with mode 1
when one army moves, `828e:0b27` with mode 2 for a group (`4125:2bda`) — and
`177b:003d` takes it down, repainting the tile (`8611:0238`). The idle loop
(`18a9:00a2` → `177b:011f`) counts BIOS ticks, and every fourth one
`177b:0161` draws the next frame through `177b:01a1`: frame k = the mode's
base from `4125:125c` (0 for mode 1, 4 for mode 2) plus a counter that runs
0–3, taken from `CURS.PCK` at `((k % 4) · 64, (k / 4) · 40)`, 40 × 40, and
drawn at the tile's corner. The top row's box is 30 × 30 at (10, 10) — round
a lone army's figure — and the bottom row's the whole tile. On the screen the
sheet's colour 1 is white and its 4 black.

### The hidden map's edges

The map is drawn whole and the hidden map laid over it (`8611:24c2`, after
the stacks). Each unseen tile in view gets a cell of `HIDDEN.PCK` —
fourteen 40 × 40 cells, 48 apart in two rows 41 apart — blitted as black
through the cell as a mask (`152a:017f`, mask `4125:551e`), so the fog's
dithered fringe lets the tile show through at its edge. The cell comes from
`8611:0f6f`: a bit for each of the eight neighbours that is unseen or off
the map, north first and then clockwise (`4125:2eda` / `2eea`), looked up
in the 256-byte table at `4125:2d56` — 0–13 the sheet's cells, 14 (nothing
seen round it) black, 255 a shape the sheet cannot draw. At the start of a
turn `8611:1558` (from `8cc6:00cd`) passes over the whole map twice, column
by column, and uncovers every unseen tile whose shape is 255.

### Watching the computer

Whether a computer's turn is shown is **Observe**, not View Enemies (whose
only reader is the right-button tile box, `740d:01b5`). New-game setup
(`79fa:0000`) sets Observe for every side, unless the map is hidden and a
human plays; Settings changes it side by side (`2c04:0147` + 2 · side). At
the start of each side's turn `8cc6:0000` copies the side's Observe into
`2c04:0159`, forced on when no human is left, and with it off the map is not
drawn for that turn (`8611:006c`); with it on, the walk (`1a8b:04c8`) centres
on the stack step by step as it does for the player's own.

During the computer's turns `5db9:045e` watches the keyboard: **Shift or Alt**
held opens Settings (`64d2:0000(0)`), where a side can be handed back to a
human or its watching turned off. When the last human falls, `8065:1c6f` says
so in two boxes (group 13: *No further human resistance is possible! / But the
battle will continue!*, then *Hold down 'Shift' or 'Alt' to stop the war /
and visit the sites of thy old battles*) and the computers fight on; a game
with no human from the start says group 14 instead (`8065:00f1`).

### The pointer

The game draws its own mouse pointer (`22bf`, with save-behind), and
**`18a9:0896`** picks which of twelve it is from what is under it; `18a9:0885`
hands the number to `22bf:036f`. A click on the map then does whatever the
pointer promised: `740d:00ce` jumps on the same number. The pictures are
`STAND.PCK` (`FILE.DAT` group `0x18`), 16 × 16 along one row from
`(k * 16, 0)`, keyed on colour 10 (`79de:0117` → `1709:0000` → `22bf:000a`),
and the hotspot is the same distance across and down, from the table at
`4125:045c`:

| k | picture | hotspot | shown | a click |
|---|---|---|---|---|
| 0 | arrow | 0 | anywhere else, and over every dialog | — |
| 1 | magnifier | 6 | over the strategic map (`4125:2aa8`) | |
| 2 | boat | 8 | a walk onto water or shore, not flying | walk (`1c8c:01fd`) |
| 3 | tower | 6 | a city not to be walked to or fought | the city dialog (`7204:0000`), Production for our own |
| 4 | hand | 8 | out of sight, a tile the stack cannot enter (`4125:1274` cost 0), the map's frame (`4125:2ab8`), an enemy out of reach | drag (`8065:0e04`) |
| 5 | target | 8 | our own stack, when nothing is selected or it is the one selected; with Ctrl, any of ours | select (`1b62:0405`) |
| 6 | legs | 8 | a walk | walk (`1c8c:01fd`) |
| 7 | ruins | 8 | a site, a razed city | `7204:0000` on the city there, else mode 4 |
| 8 | sword | 0 | an enemy **within one tile** (`1a8b:0acc` ≤ 1): a city at war, a stack not at peace, anything neutral or with Diplomacy off | `attack_tile` |
| 9 | ? | 8 | Shift over such an enemy, with the Military Advisor on | the advisor (`67cc:1f19`) |
| 10 | heart | 8 | the same enemy, but at peace | `attack_tile`, which then asks |
| 11 | arrow, slanted | 0 | Alt over the strategic map or our own | `1c8c:0007(1, 1)` |

**Dragging.** The hand's click, `8065:0e04`, is a drag for as long as the
button is held (`4125:1176` keeps the hand up meanwhile): it remembers the
tile the view is centred on, sums the mouse's own motion (`22bf:0b0c`, the
vertical halved — mickeys run 2:1), and recentres on that tile less the sums
over 40, rounded toward zero, through `8611:0629`. The map follows the mouse
a tile at a time.

Two more rules keep boats honest: a land stack at sea (army flag `0x1000`)
standing on a shore gets no sword onto anything but road, bridge, water, shore
or city, and one ashore gets none onto a shore unless it stands on one of
those itself.

### The assault

A city is **not walked into**, and it is **not attacked from afar** either. A
walk (`1c8c:01fd` → `1a8b:0c4f`) that runs into an enemy just stops beside it;
only two callers ever reach **`attack_tile`** (`67cc:0000`): a click with the
sword or heart pointer up (`740d:0179`), which the pointer only shows within
one tile of the stack (› The pointer), and a step by hand (`1c8c:041f`, the
numeric pad), whose one step comes back as 3 or 5 when it meets an enemy. So
the stack always fights from one of the eight tiles around the target,
diagonals included. `attack_tile` refuses outright unless the target is a
city or a tile with armies on it, and unless the mover has a movement point
left to spend.

What the player then sees is a **replay**: `combat_resolve` decides the whole
fight first and records a byte per casualty in `combat_log` — **1** an
attacker fell, **0** a defender — and the window plays that log back. Nothing
on screen can change the outcome.

| step | what |
|---|---|
| `67cc:1836` | the fire cloud from `WAR.PCK` over the tile — the rect at `4125:0cfa` is `(0, 0) 128 × 120`, three tiles across — with `WAR.8SN`. Skipped when no human can see the tile |
| `6a35:041c` | opens **popup 8**, `(160, 60) 320 × 312` of marble from the sheet's origin, and draws the two sides' shields from `BSHIELD.PCK` (32 × 36 cells, one per side across the sheet): the defender's at `(176, 86)`, the attacker's at `(176, 246)` |
| `6a35:0160` | draws both lines |
| `6a35:0094` | plays the log back, one casualty at a time: a sound (`7dda:0181`), then `6a35:0000` masks `ATRANS2.PCK`'s blast, `(32, 0) 32 × 29`, over the fallen army — which stays drawn beneath it — and `7ecb:0000` waits 5, 3 and 5 BIOS ticks (18.2 a second), so about 0.7 s each. **Space** (it polls `kbhit`) cuts the rest to 2 and 3 ticks; any input also ends the wait in hand |
| `6a35:04c5` | writes how it ended, centred on x = 320 from y = 298 (`4125:4366`), each line 20 below the last |
| `6a35:04f6` | closes the popup |
| `67cc:04f7` | and, if a city fell, opens the spoils dialog |

**The lines.** The defender's is on top and may wrap to four rows —
`4125:0d14` gives them as y = 86, 116, 146, 176 — and the attacker's is the
single row at y = 246. Across a row there are eight places 32 apart from
x = 216 (`4125:0d1e`), and seven more sitting between them (`4125:0d2e`), so a
line shorter than eight is centred: it takes `4 - (n + 1) / 2` as its first
place, out of the in-between table when *n* is odd. Both lines centre on the
same middle. Armies are drawn by `8611:08be` with its ring and text arguments
zero — the sprite alone, no ring, no movement number — and an army at sea gets
a patch of `WAR.PCK`'s water at `(0, 162) 32 × 18` under it.

The strings are `STRING.DAT` groups 141-147: a random one of 141 when the
garrison had already fled, then 142 or 143 for a city (with or without a hero
to name), 144/145 for a tile, 146 for a loss, and 147 for the loot.

### The spoils of a city

**Dialog 11**, behind **popup 7** — `(160, 90) 320 × 200`, which is exactly
`VICTORY.PCK` — and four 64 × 23 buttons cut from `DBUTTON.PCK` in order along
y = 250:

| x | control | src | |
|---|---|---|---|
| 168 | 285 | `(0, 0)` | **Occupy** |
| 248 | 283 | `(64, 0)` | **Pillage** |
| 328 | 286 | `(128, 0)` | **Sack** |
| 408 | 284 | `(192, 0)` | **Raze** |

`63fa:0000` greys out Pillage when `city_pillage_value` is 0 and Sack when
`city_sack_value` is — that is, when the city has no production type to strip
and fewer than two to strip down to. Occupy and Raze are always live.

Five lines are drawn centred on x = 320, at y = 93, 140, 160, 180 and 200:
`get_string(65, 0)` — "Victory!" — in the big font, then a **random** line of
group 66 with the victor's name in it, a random line of group 67 with the
city's, and then groups 65's "The city is yours!" and "Will you...". The
victor is the hero leading the assault if there is one (`451b:1efe`, the
selection's hero) and otherwise the name of its best army.

What each choice does to the city is in
[`../rules.md`](../rules.md) › Capturing a city.

### The army slots

With a stack selected the bottom bar shows that tile's armies instead of the
side's standing, and it is where the player decides **which of them move**.

What a click on the map picks up is `8c07:06eb`, and the rule is the whole
point of the grouping. It reads the **group id** of the army the tile shows —
byte `+17` of its record — and selects every army on the tile that is the
current player's *and* carries the same id, as long as that id is not 0. An
ungrouped army is picked up alone. So a stack you grouped comes back grouped
the next time you touch it, this turn or ten turns later; a stack you never
grouped hands you one army and leaves the rest waiting in the bar.

It then makes the highest of them in the fight order the anchor (`451b:1f02`),
calls `stack_movement_mode` to recount the selection, and `89e0:0d30` to build
the slots.

Three arrays run in parallel over the slots, all at `451b:` and eight entries
each, with the count in `4125:30f6`:

| array | meaning |
|---|---|
| `2902` | far pointer to the army in the slot |
| `28f2` | the **group** it belongs to, 0-7 |
| `28e2` | whether that group is the one that moves |
| `28d2` | the mark drawn under it: 0 cross, 1 tick, -1 none |

The selection itself lives elsewhere and is rebuilt from these: `1ede[]` holds
the armies that move (`1e9e` of them), `1f02` the first of them, and
`4125:2bda` is simply `1e9e > 1`. `89e0:000a` writes all of that back after
every change, and `1a8b:07f9` reads it when the stack walks — one army spends
its own moves, a group spends the pool in `451b:1ea0`, which `1c8c:0912` sets
to the **smallest** `moves left` in the group.

**Drawing**, `89e0:0356` slot by slot at `(24 + 40n, 405)`:

- the **ring**, a 32 × 30 cell of `ABITS.PCK` at `((n - 1) * 32, 0)`. Ring 0 is
  grey and 1-8 are the side colours, and a slot takes the ring
  `(current player + group) % 8 + 1` — so the ring says which **group** the
  army is in, not who owns it, and an empty slot is the grey one.
- the **army**, 32 × 29 from the side's own sheet at
  `(type % 16 * 32, type / 16 * 30)` — note the 30, the sheet's rows are not
  32 apart — or from **`ASHADOW.PCK`**, bitmap 42, when the army is not in the
  moving group. That is the ghost in the bar.
- its **moves left** as `%02d` in ABITS's own 8 × 8 digits at `(x + 8, y + 31)`,
  the digits running from `(64, 30)`.
- the **mark**, 32 × 16 at `(448, 0)` for the cross and `(448, 16)` for the
  tick, drawn at `(24 + 40n, 449)`.

and then, in the column at x = 344: the words **Group** `(0, 30)` and **Move**
`(32, 30)`, both 32 × 8; the group's own movement as two more digits at
`(352, 436)`; and the **Grp** button, 32 × 19, **red** at `(288, 0)` or
**green** at `(288, 19)`.

Above them, at **(344, 407)**, `89e0:070d` puts one 32 × 10 icon from
ABITS saying how the moving group travels: **(184, 30)**, the wing, when it
flies (`1c8c:07fa`, the rule of `stack_movement_mode` over the moving
armies); **(424, 30)** when every one of them is at sea; otherwise its move
bonuses -- **(216, 30)** woods and hills, **(248, 30)** woods, **(152, 30)**
hills -- and with none of these a blank in colour 3 (the rects at
`4125:3046`-`3066`, the point at `306e`).

`89e0:17e7` decides the marks, and it is the part worth stating plainly:

- only the **head of each group** is marked at all, the armies below it in the
  same group taking nothing;
- the head of the group that moves takes the **tick**, every other head the
  **cross**;
- but a group is the moving one only when the selection is *exactly* that
  whole group, so a half-selected group shows no tick anywhere.

`89e0:0567` picks the Grp colour: green when the whole stack is in the moving
group (or there is only one army), red otherwise.

**Changing it**, the four handlers behind the controls:

| what | handler | does |
|---|---|---|
| click an army, 224-231 | `89e0:0963(n)` | add it to the moving group, or drop it out into a group of its own — the last army of a group cannot be dropped |
| click a mark, 232-239 | `89e0:0910(n)` | make that army's group the moving one, and nothing else |
| Grp / space, 240 | `89e0:0a55` | one group, the whole stack, all moving |
| Grp, 241 | `89e0:0a99` | break it up again: every army its own group, the first one moving |

All four re-mark afterwards, and the two that change groups re-sort first, so
a group is always contiguous in the bar. The order is by group, and within a
group by the owner's **fight order**, highest first — the same table that
decides which army a tile shows.

The grouping is **remembered in the army record**, at the `+17` byte
`docs/formats/save.md` leaves unnamed. `1b62:0309` gives a newly built army
**0**, meaning no group; `8c07:03a6` writes **1**, which is what an army that
was selected but not really grouped carries; and a real group takes an id of
**2 or more**, shared by its members.

`89e0:000a` writes them after every change, one per group of slots:

- a group of one member becomes 0 — a group of one is not a group;
- the group that is moving takes a real id, **reusing one the stack already
  carries** before asking `1b62:0cde` for a new one, so clicking a stack does
  not renumber it;
- any other group of more than one is written as 1.

That id is what `8c07:06eb` reads back on the next click, and it is what makes
the grouping persist.

### The bottom bar

`89e0:0356` repaints it, and it has two faces. With a stack selected it draws
the eight army slots; with nothing selected (`DAT_451b_1f02` null) it falls
through to **`89e0:05a3`**, which draws the side's standing as four icons with
a number beside each:

| | icon | at | number at | format | from |
|---|---|---|---|---|---|
| cities | `(344, 0)` 40 × 20 | `(32, 425)` | `(72, 425)` | `%d` | `[side * 2 + 0x5dea]` |
| treasury | `(344, 20)` | `(120, 425)` | `(144, 425)` | `%dgp` | `2c04:[side * 0x14 + 0x185]` |
| income | `(384, 0)` | `(200, 425)` | `(232, 425)` | `%dgp` | `[side * 2 + 0x5dca]` |
| upkeep | `(384, 20)` | `(280, 425)` | `(320, 425)` | `%dgp` | `[side * 2 + 0x5dda]` |

The icons are blitted from bitmap **8** — `PICS/ABITS.PCK`, 480 × 40 — out of
the strip past the nine 32 × 30 side rings: a castle, a chest, a pile of coins
and a hand paying them out. The source rects live at `4125:3072`, the icon
points at `4125:3092`, the text points at `4125:30a2` and the four format
strings at `4125:3128`, each read as four, two and two `u16`.

Both points are **top-left** — the text is not baselined — so the numbers sit
flush with the top of their icons, which is the layout of the shipped screens.
The three totals are read from per-side tables rather than recomputed, so they
are as stale as their last update; recomputing them per frame is
indistinguishable on the screens checked.

### Menus

`7ae8:0052` builds the menu bar from a table at `4125:1adc`, 12 bytes per
entry, into 8 menus of 46-byte records. A title's rect is its text width
rounded up to a multiple of 8, plus 8; titles start at x = 8 and each next
one 8 past the end of the last. Segment `2372` runs the pulldowns: `00f0` is
the tracking loop, `0047` maps a key to a menu command, `049b` lays out and
paints a dropdown, `102d` highlights, `132f` paints the bar's titles.

How it looks, all checked pixel for pixel against the original:

- **The bar** (`7ae8:02d8`) is filled white, (0, 0) 640 × 17 — the font's line
  height plus 2. Titles are `TEXT` in black (`7ae8:029c` asks for font 0 in
  colours 0, 15, 15), at the rect's x + 2, y = 1.
- **An open menu's title**, and the row under the pointer in a dropdown, are
  XORed with 7 (`2372:102d` → `2012:044f`): white becomes 8, orange, and black
  7, yellow. A greyed item's glyph is colour 3.
- **A dropdown** (`2372:049b`, `2372:0803`) sits at its title's x, one pixel
  under the bar. Labels are measured as if 12 pixels in and drawn 3 in; the
  accelerators start in a column 5 past the widest labelled item, and the
  dropdown is 5 wider than its widest line. Rows are the line height plus 2, a
  separator 2 (a black line), with 2 above the first row and 1 below the last.
  It is white, outlined in black down its left and along its bottom and
  right, with a second line a pixel further out below and to the right for a
  shadow; there is no line along its top.

### The turn strip

`8cc6:0952` draws the right-hand end of the bar: the strip (437, 0) 202 × 17
filled white, `Turn %d` in font 2, black, right-aligned at x = 508 + 16·(8 − n)
for n sides still in the game, and a 16 × 14 shield for each of them — from
`ATRANS2.PCK` at (112 + 16·(side / 4), 94 + 14·(side % 4)), masked on colour 1
— at x = 512 + 16·(8 − n + i), y = 2. The side whose turn it is sits on a
black box, (x − 3, 1) 17 × 15.

### The strategic map

It is painted into `STRAT.PCK` — bitmap 7, a 224 × 312 buffer — and blitted
to (400, 30) (`834b:0000`). `834b:2785` paints each tile as **four** pixels,
each with its own colour from `MAPCOLOR.DAT`, which is eleven tables:

| bytes | table |
|---|---|
| 4 × 256 | a colour per tile id for the top-left, top-right, bottom-left and bottom-right pixel |
| 3 × 32 | sixteen u16 colours each; the first (the identity) is the one used |
| 4 × 20 | the same four pixels for a tile with a road, by the overlay's road number |

So forest, hills and marsh come out speckled and roads are drawn in. Tile ids
0x50–0x5f take a random grey, `dice(1, 3, 1)`, per pixel.

Every city the side can see is then marked (`834b:0ed7`) with an 8 × 8 shield
from `ATRANS2.PCK` — the owner's at (side × 16, 30), neutral the ninth — at
the city's (2x − 1, 2y − 1). The sheet carries each shield eight times, shifted
a pixel a row, because the blit can only start on a byte; drawing the
unshifted one where it belongs is the same. A razed city gets none. A city
the caller names gets a white box one pixel clear of its shield.

**The view box** (`8961:0698`) is white, 18 × 18 with two-pixel sides, at
(400 + 2·scroll x, 30 + 2·scroll y). **The hero offer's figure** (`834b:1f5f`)
is `ATRANS2.PCK`'s (96, 0) 16 × 15, at the city's 2y − 6 and at 2x − 2
rounded down to a multiple of 8.

### Panels repainted with marble

The main screen's redraw (`8065:0000`) refills two panels with `MARBLE.PCK`
before drawing into them: the bottom bar (`8065:0aeb`, (16, 403) 360 × 66 from
the marble's (0, 60)) and the control panel (`8065:0a9d`, (400, 355) 224 × 114
from its origin). While the turn's opening — the banner and any hero offer —
is up, every control is still greyed: the refresh that sets them (`8065:0174`)
has not run yet.

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

### Popups

Beside the `BUTTON.DAT` dialogs there is a second, simpler mechanism: a stack
of **popups**, pushed by `54f6:0000` and popped by `54f6:069c`, which save and
restore the screen behind them. A popup is an index into a rect table at
`4125:06a8`, eight bytes an entry — `x, y, w, h`:

| # | rect | used for |
|---|---|---|
| 0 | (80, 60) 480×320 | the city dialog's area |
| 1, 7 | (160, 90) 320×200 | |
| 2 | (80, 60) 480×312 | the hero offer |
| 3 | (96, 50) 200×200 | |
| 4 | (120, 50) 400×360 | |
| 5 | (144, 179) 352×64 | |
| 6 | (160, 60) 320×312 | the start-of-turn banner |
| 8 | (160, 60) 320×312 | |
| 9 | (160, 60) 320×280 | |
| 10 | (32, 60) 576×312 | |
| 11 | (80, 60) 480×350 | |

The saved area is the rect grown by 16 either side, 1 above and 3 below.
`54f6:0000` then switches on the popup number for a bitmap id — 6 → `0x1c`,
7 → `0x20`, 9 → `0x2c`, 13 → `0x45`, 23 → `0x46` — which `FILE.DAT` group 3
resolves, so popup 6 is `city.pck` at exactly its own 320×312.

### The start-of-turn banner

`8cc6:0259` is the start of a turn. After the income, upkeep and movement
resets it pushes popup 6 and writes two lines over it:

```
FUN_54f6_0000(6)                                    -- push the popup
FUN_78a8_06ae(1, 15, 0, 3)                          -- white, black shadow, font 3
FUN_7ecb_00d6(320, 85, current_player * 0x14, 0x3c04)   -- the side's name
sprintf(buf, "Turn %d", game_turn())
FUN_7ecb_00d6(320, 130, buf)
auto_sound_turn_8sn()                               -- the fanfare
FUN_7ecb_0142()                                     -- block until any input
FUN_54f6_069c()                                     -- pop, restoring the screen
```

`7ecb:00d6` is **draw centred**: it calls `21e2:04aa(x - width(s)/2, y, s)`, so
320 is the screen's centre line, and 85 and 130 are the tops of the two lines.
`7ecb:0142` is the blocking wait — the banner goes away on *any* key or click,
and that input does nothing else.

The name comes from `0x3c04:(side * 0x14)`. Segment `0x3c04` is the scenario
file loaded verbatim, which is how `.SCN` offsets and code offsets agree:
`0xc0` is the level table and `0xd0` the human/computer table, exactly as
`scn.lua` already had them. The eight 20-byte names therefore end at `0xa0`,
where the **side colour table** begins — one palette index a side, the same
eight in every scenario:

| side | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---|---|---|---|---|---|---|---|
| colour | 15 | 7 | 8 | 9 | 10 | 6 | 5 | 0 |
| | white | yellow | orange | red | green | blue | cyan | black |

There is a **second** colour table right after it at `0xb0`: the colour a side
outlines things in, `0` for everyone except side 7, who outlines in `9`, red.

`54f6:0000` frames popup 6 using both. **Read this one off the disassembly,
not the decompilation** — Ghidra drops the arguments to `216d:002e`, the rect
constructor, so every rect in this routine decompiles as noise. The real
sequence, all in the *picture's own* coordinates:

```
outline (0, 0, 320, 312)              edge colour (0xb0)
fill    (1, 1, 318, 9)                side colour (0xa0)   top
fill    (1, 1, 9, 310)                                     left
fill    (1, 302, 318, 9)                                   bottom
fill    (310, 1, 9, 310)                                   right
outline (10, 10, 300, 292)            edge colour (0xb0)
```

`2012:0bb7` draws a horizontal run and `2012:0f08` a vertical one, so an
outline is four of them; `24d0:0497` fills a rect by drawing one horizontal
run a scanline.

The crucial part is *where* this lands. `1997:034a` and `1997:060a` lock
bitmap `0x1c` and make it the drawing target, so the frame is painted **into
`CITY.PCK` itself, over its outer ten pixels**; `1997:06bb` unlocks it and
`1997:0129` blits the whole 320×312 to (160, 60). The banner is therefore
exactly the size of the picture — the frame eats into the art rather than
surrounding it, which is why the popup rect and the bitmap are the same size.

The drop shadow is separate and does sit outside: `14d0:0002` sets colour 0,
then two horizontal runs at `y + h` and `y + h + 1` and two vertical runs at
`x + w` and `x + w + 1`, taken from the popup rect grown by one — a 2-pixel
black shadow down and to the right.

### The hero offer

`6563:0d5c` (`auto_ui_hero_emerges`) is the dialog a hero is offered through.
It is the clearest worked example of the popup mechanism, because it uses
**both** halves of the toolkit at once: popup 2 for the panel, and dialog 12's
`BUTTON.DAT` controls on top of it.

```
FUN_54f6_0000(2)                      -- push popup 2: (80, 60) 480x312
FUN_834b_0000(1, 1, 0, 0)             -- the map panel
FUN_834b_1f5f(city.x, city.y, 1)      -- centred on the offering city
FUN_834b_04d3(0, 0)
load_hero_name()                      -- rolls the name and the sex
   ... four lines of text chosen by turn and sex ...
FUN_78a8_06ae(1, 15, 0, 3)            -- font 1, white
FUN_7ecb_00d6(432, 63, "A Hero!")     -- get_string(0x5f, 0)
FUN_1997_0129(0x21 or 0x22, rect, at) -- MHERO.PCK or FHERO.PCK
FUN_78a8_06ae(2, 15, 14, 3)           -- font 2 for the body
FUN_7ecb_00d6(432, 190 / 210 / 230 / 250, line)
FUN_7ecb_0058(328, 287, 208, 20, name)  -- the editable name field
FUN_216d_01fd(&rect)                  -- its frame
FUN_1997_0129(8, ...)                 -- the two checkboxes, from ABITS.PCK
FUN_69fa_056a(12)                     -- dialog 12: OK, Cancel
```

Popup 2 has **no bitmap** in `54f6:0000`'s switch, so it is not a picture with
a frame painted into it the way the banner is. It is `MARBLE.PCK` — 480×360,
which is why every bitmap-less popup is 480 wide or less — cropped to the
rect, with a plain black outline round it.

Measured off the original running at its own 640 × 480 (a DOSBox-X capture,
not a scaled window grab), that outline is **one pixel outside the rect on
every side** — at (x − 1, y − 1), w + 2 by h + 2 — and the popup's contents sit
exactly at the rect. The drop shadow is two more pixels, right and below:
rows y + h + 1 and + 2, columns x + w + 1 and + 2. An earlier reading off a
scaled screenshot put the contents a pixel low; the native capture shows no
such offset in the frame, the portrait's frame, the name field or the
buttons.

The name field is 7ecb:0058, the one routine every text field goes through:
it **fills** the rect with colour 3 (so the marble does not show through),
sinks it with a (4, 2) bevel, and draws the text at (x + 3, y + 2).

The coordinates are DGROUP statics from `4125:1112` on, and the controls in
`BUTTON.DAT` group 9 agree with them exactly — which is the cross-check that
the numbers were read correctly:

| what | where | from |
|---|---|---|
| map panel | (80, 60) 224×312 | `AREA.DAT` screen 6, region 15 |
| portrait | (320, 110) 224×170 | `4125:111a`; frame (319, 109) 226×172 |
| title | centred on 432, y 63 | `4125:1126` |
| four lines | centred on 432, y 190/210/230/250 | `4125:112a`…`1136` |
| name field | (328, 287) 208×20 | `4125:113a`; frame (326, 285) 212×24 |
| Male / Female labels | right-aligned at (376, 315) / (480, 315) | `4125:1152`, `1156` |
| their boxes | (384, 313) / (488, 313) 24×20 | `4125:114a`, `114e` |
| Cancel / OK | (320, 341) / (480, 341) 64×23 | `BUTTON.DAT` group 9 |

224×312 is the whole 112×156 map at two pixels a tile — the strategic map's
own scale. The spawn is marked with the white figure at (96, 0) in
`ATRANS2.PCK`, a **mask** sheet: only its colour 15 is drawn, colours 1 and 2
being the ground and the shading. It lands centred on the city's four pixels.

`7ecb:00d6` draws centred, `7ecb:0103` right-aligned (which is why the labels'
x values sit just left of their boxes) and `7ecb:0058` is the edit field.

The four lines come from `STRING.DAT` group 0x61, and the sex chooses between
two blocks of four — male at 0, female at 8:

| | turn 1 | later |
|---|---|---|
| 0 / 8 | *(empty)* | `A Hero in %s offers to` |
| 1 / 9 | *(empty)* | `join you for %d gold.` |
| 2 / 10 | `A Hero emerges in` | `You have %d gold to spend.` |
| 3 / 11 | `%s` *(the city)* | `Will you accept?` |

The first two being empty on turn 1 is why the free hero's caption sits low on
the picture rather than filling it — there is no special case, just two blank
lines drawn at the top two positions.

### Hero names

`load_hero_name` (`6563:0c67`) reads `TERRAIN<set>\HERONAM<side>.DAT` — the
name comes from `FILE.DAT` group 0x14, indexed by the side — and picks a line
with `dice(1, 100, 0)`, walking the file for the n'th `#`. Each line is
`#<sex> <name>`, sex 1 being female.

The hundred is **hard-coded**, not read from the file: `HERONAM4.DAT` ships
with 101 lines, so its last hero, Lady Jorinas, can never be drawn.
`HERONAM0.DAT` has no `#1` line at all, so the Sirians never field a heroine.
The roll settles the name and the sex together, and the dialog's checkboxes
change only the portrait and the wording — ticking Male on a Mystichla leaves
her name in the field.

### Choosing a font, and colouring it

`78a8:06ae(font, glyph, outline, ground)` selects the font for everything drawn
after it. **0 is `TEXT`, 1 is `CHANCE36` and 2 is `CHANCE17`**: titles are
font 1, nearly every other line in the game is font 2, and the bottom bar's
numbers are font 2 as well (`89e0:05a3`), not `TEXT`. The other three
arguments remap the sheet's three colours — glyph 15, outline 0, ground 3 —
through `78a8:0839`, so `(2, 15, 0, 3)` is the font as drawn and `(2, 9, 0, 3)`
draws it in red. The three looks are cached per font, six bytes an entry.

### Opaque art

`1997:0129`, the blit nearly everything goes through, is a **plain copy**: a
bitmap's colour-3 ground replaces what was under it. `ABITS.PCK` is drawn that
way everywhere — the rings, the checkboxes, the bar's icons and digits — and a
capture of the original confirms it pixel for pixel: inside the castle icon's
40 × 20 cell the screen is exactly the cell, flat grey and all. Only the army
sheets, the shadow sheet and the few other bitmaps `8611:08be` and friends
blit through `451b:2a68` with a mask are see-through.

The bottom bar and the control panel are not the screen's own art either —
see *Panels repainted with marble* below.

### Default and cancel buttons

Enter and Escape reach the first live control in one of two lists of control
ids (`4125:155c` and `4125:1514`), whichever dialog is up; "live" means on
the current screen in state 1, the ordinary look (`18a9:05ab` reads `+4`).
The Enter list's controls are the ones ringed (see *Controls*).
Among them: the city dialog's Done (192, 201), Build Production's Done (396),
the text-entry dialog's OK (189) and Cancel (190), the hero offer's OK (287)
and Cancel (288), and Occupy (285) on the spoils dialog, which is on **both**.
On the main screen Enter is **Next army** (174) and Escape **Quit army** (175).

## The city dialog

Dialog 6 over popup 2, **(80, 60) 480 × 312** — the same rect as the hero
offer. The left half is the strategic map; the right is
`auto_ui_city_info` (`7204:06de`), a case per mode. `7204:0000(mode, x, y)` is
every way in: a click on your own city opens it in mode 2, any other city in
mode 0 (`740d:0037`). The mode buttons are 193-196:

| mode | button | for | draws |
|---|---|---|---|
| 0 Info | 193 | any city | shields, income/defence/owner, the production list (or a picture), the capital's shield, three lines from `.CTY` |
| 1 City | 194 | own | shields, income/defence/owner, Rename (203), Raze (205), Build Prod (204), two lines of text each |
| 2 Production | 195 | own | what it builds and its countdown, the list (197-200), Stop (202), the chosen type's numbers |
| 3 Vector | 196 | own | what it builds, then what is on its way here next turn and the turn after |
| 4 | — | a ruin or temple | the site's own info, in the same frame |

`7204:03a9` sets the buttons after every change: the current mode's button
lit, 194-196 greyed on a city that is not yours, Done is 192 except in
Production where it is 201, and Stop greyed while nothing is being built.

Every position is in the table from `4125:0ea0`; text is font 2 except the
name, font 1 centred on (432, 62) in every mode:

| what | where |
|---|---|
| owner's shield, twice (SHIELDS.PCK, 40 × 40) | (312, 102), (512, 102) |
| `Income: %d gold`, `Defence: %d`, `Owner: …` (group 116) | (356, 106), (356, 126), (356, 150) |
| Info: the production list, grey rings | (320, 190), (376, 190), (432, 190), (488, 190) |
| Info: the capital's small shield (BSHIELD.PCK bottom row) | (408, 170) |
| Info: three lines of `.CTY` | (310, 259), (310, 279), (310, 299) |
| City: the two lines for Rename, Raze, Build (groups 119-121) | x 376, y 183/203, 231/251, 279/299 |
| Production: the capital's small shield | (312, 110) |
| Production: `Current:` right-aligned, the army, `%dt` or `-` | (408, 110), (416, 104), (456, 110) |
| … when vectored: `%dt, then to` and where | (456, 102), (456, 122) |
| Production: the list; the chosen type on its side's ring | (312, 142) 48 apart |
| Production: BIGARMY.PCK, then name, Time, Cost, Strength, Move | (320, 182); x 456, y 182/212/232/252/272 |
| Vector: `Current:` right-aligned, the army, `%dt` | (360, 109), (368, 103), (408, 109) |
| Vector: `Next turn:` / `Turn after:` right-aligned | (392, 155), (392, 188) |
| Vector: up to four armies in each row | x 400 + 40n, y 149 / 182 |

Info shows a **picture** instead of the list when the city is razed, or when
*View Production* (`.SCN` `0x132`) is on and the city is not yours: a black
outline at (311, 169) 242 × 88, a (4, 2) bevel round it, `CITYBACK.PCK`'s
240 × 86 at (312, 170), and the city's own four map tiles at (392, 176) 40
apart.

**Choosing production** (`7087:00c8`): the slot's type becomes the chosen one
and the city builds it, the countdown starting again even if it was already
building that. The chosen type is remembered separately from what is being
built, which is why the ring and the numbers follow a click. **Stop**
(`7087:0185`) clears production, the vector and the choice.

**Rename** (203, `7204:2013`) is the text-entry dialog with "Rename City",
"Type the new name for" / "this city", at most 15 characters and 128 pixels.
**Raze** (205, `649c:0000`) asks first — "Raze City", "Are you sure that you",
"want to", "raze %s?", "You won't be popular!" — and costs the side **1d25 +
25** on its atrocity score (`649c:0061`), where razing a city as it falls costs
1d15 + 10; it needs no army in the city. Cancel returns to City mode, OK to
Info. **Build Prod** (204, `7087:0978`) opens Build Production and returns to
City mode.

### Vector mode

The map changes with the mode (`834b:08df`; `834b:1817` for See All): no
owner shields, but a marker from `ATRANS2.PCK`'s row at y = 94, 16 × 10, on
each of the side's cities, at (2x − 2, 2y − 1):

| marker (table `4125:2c76`) | means |
|---|---|
| 0 white, filled / 3 white, empty | one of the side's cities, building / idle |
| 4 black, filled / 6 black, empty | the dialog's city |
| 1 yellow, filled / 5 yellow, empty | where the dialog's city sends its armies |
| 2 orange | a city sending its armies here |

and lines between the cities' (2x + 2, 2y + 2) — colour 7 to the destination,
colour 8 from each city sending here. See All marks every city by what it is
doing and draws every vector, and boxes the dialog's city in white.

The mode has three states (`4125:0e94`), each with its buttons and the two
lines of help beside them (`7087:0638`, groups 155 and 156, at (368, 221) and
(368, 272)):

- **idle** — 210 *vector to a new city* (greyed unless the city is building),
  211 *change the destination of armies* (greyed unless some city sends here),
  212 See All (216 while it is on);
- **choosing a destination** — 214 lit in 210's place; the map shows only the
  cities that could take one more;
- **moving the incoming** — 215 lit in 211's place; the map shows only the
  cities that could take them all.

A click on the map (`7087:072e`) means the side's city **nearest** the click
(`828e:04fa`, by map distance). With Shift, or while choosing a destination,
that city becomes the destination (`7087:028b`): only if this city is building,
clicking the city itself lifts the vector, and **no city takes more than four**.
While moving the incoming, every city sending here is redirected to it, if it
could take them all. Otherwise — and after any click but a send — the dialog
moves to the city clicked, still in Vector mode.

The **Current** row shows the city's two armies on the road: the one sent out
this turn at (432, 103), the one arriving next turn at (472, 103). The rows
below are armies coming here from other cities.

### Build Production

Popup 11, **(80, 60) 480 × 350**, dialog 23; `auto_ui_build_production`
(`7087:09da`):

| what | where |
|---|---|
| "Build Production" (group 112), font 1 | centred on (320, 64) |
| the side's shield, twice | (88, 64), (512, 64) |
| "The %s city of %s" | centred on (320, 104) |
| every type with a price ≥ 0, in ARMYTYPE order, on grey rings | (88 + 120·col, 136 + 31·row), four to a row |
| its price, `%d gp` | 40 right and 6 down of the army |
| "Currently Producing" | centred on (176, 348) |
| the city's four slots, each on its side's ring | (104 + 40n, 370) |
| "Thou hast %d gold" | centred on (352, 365) |
| Done (396) | (480, 380) |

A type is **ghosted** (drawn from `ASHADOW.PCK`) and its button greyed when
the city already builds it or the side cannot pay for it (`7087:0dae`). The
slot being bought into is framed: a colour-0 box at (x − 2, 368) 37 × 35 and a
colour-9 one a pixel up and left; the others get the same two boxes in
colour 3. The screen starts on the first empty slot, or the first of all when
the city has four (`7087:0978`), and the slot buttons (397-400) are live only
then — until the list is full a type always goes into the first gap. Buying
(`7087:0ee3`) copies the type's unmodified stats in, takes the price, moves on
to the next empty slot, and — because the original tracks what a city builds
by **type**, not by slot — stops production and the vector if the type being
built was the one bought over. Done sorts the list by price (`7087:1544`, an
insertion sort) and goes back to the city dialog.

### The text-entry dialog

`7b4c:0000` asks for a line of text and `7b4c:0088` a yes-or-no question, over
popup 1 — **(160, 90) 320 × 200** — and dialog 5: OK (189) at (400, 260),
Cancel (190) at (176, 260), the field's hit area (191). The title is font 1
centred on (320, 92); the prompt is font 2 centred on x = 320 at the y the
table at `4125:2316` gives for the line count (1: 150; 2: 150, 173; 3: 150,
173, 196; 4: 140, 163, 186, 209 — a text entry counts its field as two more
lines). The field is 7ecb:0058 at (184, 197) 256 × 20.

Clicking the field starts an edit (`7b4c:03a6`) **from an empty line**, not
from the old text. A character from 0x20 to 0x7a is taken while the line is
shorter than the limit and narrower than the width limit (measured before the
new character); Backspace removes one; Enter keeps the line and Escape puts
the old one back. The cursor is a "`" — the font's block glyph — blinking
between colours 15 and 9.

## The reports

`6ef3:0000(n)` opens dialog 7 over popup 2 on report *n* — Army, City, Gold,
Production, Winning — with the five as tabs (218-222) along y = 103 and Done
(223). The title (group 73, font 1) is centred on (432, 62), what the report
measures (group 74) on (432, 149), and a summary (groups 75-79) on (432, 322)
in the side's own colours. The left half is the strategic map; the Army
report adds a banner — `ATRANS2.PCK` (owner × 16, 164) — on each stack outside
a city (`834b:158d`), and the Production report shows See All's vectors.

`6ef3:02fb` works out the figures: armies per side, cities, gold, or a
**Winning score** of (gold + 5 × income + upkeep + Σ defence × income over the
side's cities) / 30, kept to 1-500 and shown as a fifth of that against a
fixed scale of 100; the summary gives the side's rank. For the others the
scale is the largest figure, raised by one if it is odd.

`6ef3:05ff` draws a bar per side still in the game at (312, 200 + 14·i), 240
pixels at the top of the scale, tiled 8 pixels at a time from the side's 8 × 8
in `SHIELDS.PCK` at (320 + 8·(i % 4), 40 + 8·(i / 4)) and sunk with a (0, 1)
bevel. The scale is a colour-1 axis at y = 196 with ticks every 60 pixels,
shadowed in black a pixel up and left, and 0, half and the top at (308, 172),
(432, 172) and (556, 172).

The **Production report** (`6f8c:086e`) lists what `city_production_turn`
logged this turn — armies that arrived from vectoring, then each one built —
five at a time: its number at (312, 176 + 30·i), the army on a grey ring at
(328, 170 + 30·i), and at (368, 176 + 30·i) the city, `%s ...` for one sent
away or `... %s` for one that arrived. 206-209 scroll a row or five.

## The hero info dialog

Hero › Inspect (`6c1b:0000`) lists the side's heroes, last army first, and
opens dialog 8 over popup 2 on the one nearest the cursor; with no hero it does
nothing. `6c1b:00f9` draws the map with no city shields but every hero's
figure (`6c1b:10ac`), a black outline at (308, 206) 248 × 129 in a (4, 2)
bevel, the name (font 1) centred on (432, 62), the hero and its stack on grey
rings along y = 110 from x = 304, and — right-aligned at x = 384 and 528, the
figures 8 further on — *In:* or *Near:* the nearest city, *Battle:* the battle
items, *Command:* `4125:0ce4`[min(9, strength + battle)] plus the command items
(a standard counting 1), *Level:* and *Exp:*; *n of m* at (312, 347).

`6c1b:0724` draws the items: the hero's carried ones then those on its tile,
in item order (`list_carried_items` mode 6), three at a time at
(376, 238 + 22·i) in a sunk colour-3 box at (367, 236) 184 × 66, the chosen one
in a raised box at (369, 258) 180 × 21 — colour 7 carried, 10 on the ground,
5 lost — with what it does centred on (340, 260): `com +n`, `bat +n`, `fly`,
`move`, `gld +n`. The title says which kind is chosen; the two `ABITS` boxes
at (312, 311) *Carried* and (400, 311) *Ground* are ticked accordingly and
choose the first of their kind (250, 249). 245/246 step through the list,
247 **Drop It** (`7563:0943`: onto the tile, or lost at sea), 248 **Take It**
(`7563:08c7`), 243/244 the next and previous hero, 242 Done, which recomputes
income and checks the item quest.

Item records 0-7 are the eight standards, and the hero a side is given on turn
1 carries its own.

## Plant Flag, standards on the map, and Hero Levels

**Plant Flag** (`7563:09f7`) puts the selected stack's hero's own standard
(item record = side) on the ground, **planted** — map-tile flag `0x40` — if the
tile is not water, shore, a city or a ruin or temple and has no flag already.
A city can then vector to it: in Vector mode a click nearer the planted flag
than to any city means the flag (`828e:0651`), the city's panel says
*Standard!* (*Nowhere!* once it is gone), and the map draws the flag —
`ATRANS2.PCK`'s (96, 15) — with a line to it. An army sent there
(`6f8c:0000`) stands on the flag's tile if it has room, and otherwise goes home,
two turns more. Taking the standard up again unplants it.

On the main map (`8611:2d7c`, drawn by `8611:1a79` before the stacks, at the
tile's corner) an item on the ground shows as a bag, `ATRANS2.PCK`'s (64, 0)
32 × 29, and a planted standard as its side's flag, army cell 29 of the side's
sheet.

**Hero › Levels** (`7563:1652`) is popup 0 — (80, 60) 480 × 320 — with dialog
28's Done (469): *Hero Levels* in font 1 centred on (320, 63); the heads
(group 104) in the side's colours at y = 105 — Hero and Level from x = 128 and
272, Exp, Needs, Str, Move centred on 392, 440, 488, 536; and a row per hero,
30 apart from y = 128, highest level first: the hero at (88, y) on its side's
ring at the top level and a grey one below it, the name at (128, y + 6), the
title from group 99 or 100 by sex, and its experience, the next level's need
(15, 30, 60, or `-`), strength and moves; grey rings fill the rows to six.

## Hero › Search

`site_search` (`6536:0000`) is reached only from Hero › Search and its
configurable button — walking onto a ruin searches nothing — and says nothing
where there is nothing to search: no site, a site already searched (tile flag
`0x40`), or a ruin with no hero in the stack.

**A ruin** (`ruin_search`, `6536:01ab`) opens popup 4, (120, 50) 400 × 360:
*Searching* (font 1) centred on (320, 53) and `SEARCH.PCK` at (160, 92) in a
black frame a pixel out (`6536:175b`). The story comes a line at a time at
(128, 300), 20 apart (`6536:17c9`), each waiting for a key or a click
(`8065:10fb`): *It appears to be uninhabited!* or *%s encounters a %s...* and
*and is slain by it!* / *and is victorious!* (groups 50, 51), then what was
found (52 an item, 53 gold, 54 allies). Dialog 13 ends it: Done (292) and,
after an item, Take (293). **A found item is left on the ground** at the ruin
for a human player — Take (`6536:186e`) picks up all that lies there; a
computer player's hero takes it at once.

**A temple** (`auto_ui_temple`, `4976:0000`) is popup 9, `TEMPLE.PCK` at
(160, 60), with its name and two lines of group 19 centred on x = 320 at
y = 270, 290 and 310 in font 2 coloured 7 on 6, over dialog 15: Bless (328) —
`temple_bless` (`6536:08a1`), then a message box with group 55 or 56 — and
Quest (329), greyed with quests off, a quest already running, or no hero.

**The message box** (`8065:1160`) is popup 5, (144, 179) 352 × 64, two lines
centred on (320, 190) and (320, 212), gone at any key or click.

`54f6:0000` blits each popup's picture from its own (0, 0) — `MARBLE.PCK`
unless the popup has one of its own (6 `CITY.PCK`, 7 `VICTORY.PCK`, 9
`TEMPLE.PCK`, …) — at the popup's rect, then outlines it a pixel out and
shadows it; popup 2 draws the strategic map in its left 224 pixels itself.

### Sages

A hero who finds a sage (`sage_visit`, `6536:0aa0`) gets the searching popup
with one line, *%s has found a Sage!* (group 58), and after a click the sage
himself (`6536:146f`): popup 2 with the strategic map, *A Sage!* in font 1
centred on (432, 62), the greeting (group 63) centred on x = 432 from y = 120,
20 apart, and dialog 10: **Items** (279), **Money** (280) and **Maps** (281),
and **Done** (282, both default and cancel) hidden until one has been taken.
Items is greyed when the sage has nothing to tell, Maps when the map is not
hidden. What the sage says goes on from y = 240, 20 apart, centred on 432.

- **Items** (`6536:0bf5`) opens the list chooser, titled *Items*, on what
  `6536:1610` finds: every rich site the side has not been shown, less than
  35 tiles from the hero as the crow flies (`2012:1199`) — an item by name,
  and *Gold* and *Allies* once each however many. Chosen (`6536:0e85`), it
  says *The %s* / *A huge pile of treasure* / *Powerful allies*, *can be
  found*, *at %s!* (group 62) — the nearest such site for gold and allies —
  marks the site shown to the side, uncovers round it and draws the way
  there as the Quest screen does, from the hero. Cancel brings the three
  buttons back.
- **Money** (`6536:0b1a`): a gem worth `dice(3, 500, 500)`, paid at once,
  *The sage gives you a gem* / *worth %d gp!* (group 59).
- **Maps** (`6536:0c50`) greys the three and says group 60's three lines; the
  map is then region 15, and the tile clicked (`6536:0cd6`) has a patch
  uncovered from 9–13 tiles up and left of it (`dice(1, 5, 8)` each way),
  16–25 tiles wide and high (`dice(1, 10, 15)`), kept on the map, every tile
  of it through `8611:1298`. The patch is outlined in white on the popup's map.

The site's tile is flagged searched either way, so a sage is visited once.

### The list chooser

`796c:0000` is the game's own list box: popup 3, (96, 50) 200 × 200, dialog
2. Its title in font 2 centred on (196, 52), a colour-3 box at (106, 70)
180 × 110 sunk with a (4, 2) bevel, and five names at (112, 74 + 20 i), the
chosen one in colour 15 and the others in 2. 114–118 choose a row; 121 and
122 scroll a row up and down and 119 and 120 five, the four hidden when the
list fits in five rows; OK (123, default) and Cancel (124, cancel) hand the
chosen entry, or −1, to a callback.

## Order › Disband and Signpost

**Disband** (`1b62:06bf`) asks through the yes-or-no form of the text-entry
dialog (`7b4c:0088`): *Disband* / *Are you sure you* / *want to disband this*
/ *group?* and a fourth line, *It contains heroes* or empty — the dialog
counts its lines by pointer, so there are always four, at y = 140, 163, 186,
209. OK (`1b62:076a`) drops each hero's items where it stood (`67cc:16cd`),
takes the side's quest away if its hero goes (the side's 12-byte record at
`2c04:1103`), deletes the armies, and clears the selection.

**Signpost** (`540d:01a4`) works only on a tile of terrain 9 with a sign in
`CURRENT.SGN`. Popup 1 with *A Signpost!* in font 1 centred on (320, 94),
*Type the new message for* / *this signpost!* in font 2 centred on 320 at
y = 140 and 160, and the sign's two lines in fields at (200, 190) and
(200, 215), 240 × 22 (`540d:02d7`). Dialog 24: Done (421, default and cancel)
and the fields' hit areas 422 and 423. A click on a field types a new line
into it, as the text-entry dialog does, up to 29 characters and 216 pixels
(`540d:03dd`, `0432`). The file is written back when the dialog closes.

## Order › Resign

`7721:150d` (only for a side that holds a city — its count at `4125:5dea`) is popup 1: *Resign!* (group 165) in font
1 centred on (320, 92) and three lines of font 2 on 320 at y = 140, 160, 180;
dialog 33 has three buttons down the middle — *Resign Graciously* (487),
*Resign Ungraciously* (488) and *Keep Playing* (489, default and cancel).
Both resignations end in `7721:1608`: every city of the side is made ruins
(`649c:016b`, the bare routine, with no atrocity score), every army goes, a
hero's items dropping, and with a hidden map the whole map is uncovered. The
gracious way first asks three times, a one-line message box each
(`8065:10fb`, popup 5 with the line centred on (320, 201)), then says *I
don't think so!* / *I'm going to burn the cities anyway!*; the other says
*Ha! Now I've burned everything!* / *Let the enemy come!* after. The turn
goes on; the side is out when it next counts.

## Order › Fight Order

`6a89:0de1` edits the side's row of the fight-order table (`.SCN` 0x60b, 29
bytes a side: each army type's rank) in place, keeping a copy for Cancel.
Popup 11, (80, 60) 480 × 350 (`6a89:0e4a`): *Fighting Order* (group 127) in
font 1 centred on (320, 64) between the side's big shields at (88, 64) and
(512, 64); *Order of combat for %s* centred on (320, 104) and the three lines
of help on 320 at y = 348, 368, 388, in font 2; then the 27 places four to a
row, the type holding rank i on a ring at (88 + 120 (i mod 4),
128 + 31 (i div 4)) — the side's colour for the chosen one, grey otherwise —
and *%d.* at (128 + 120 (i mod 4), 134 + 31 (i div 4)).

Dialog 26: OK (425, default), Cancel (426, cancel; `6a89:111c` puts the copy
back), Reset (427: the neutral row, `2c04:06f3`, copied in), the arrows 428
and 429 (swap the chosen place with the one before or after, and follow it;
greyed at the ends and with nothing chosen), and the places 430–456: a click
chooses one, lets the chosen one go, or swaps it with another
(`6a89:1379`).

## View › Army Bonus

`89e0:1e3b` lists the army types by the side's fight order — only the first
27 places — six at a time. Popup 0, (80, 60) 480 × 320 (`89e0:1fd2`): *Army
Bonus* (group 164) in font 1 centred on (320, 62); a box (124, 146) 434 × 186
with a (4, 2) bevel and a black outline a pixel in; the column heads, the
216 × 22 at `STACK.PCK` (188, 0), at (256, 125). Each row, 30 apart from
y = 148: the army on a ring of the side's colour at (128, y); name, strength
and moves (`7087:14ac`, the ARMYTYPE.DAT record) at x = 168, 280, 328,
y + 5; how it moves at (352, y + 8) from `ABITS.PCK` — (184, 30) flies,
(216, 30) woods and hills, (248, 30) woods, (152, 30) hills, from the table
at `4125:45c4` that `7563:1b20` fills from ARMYTYPE.DAT +54, +56, +58; and its
bonus at (400, y + 6).

The bonus (`89e0:1a07`, group 163) is the first that applies, in this order:
+52 = 2 *+1 & cancel hero*, +52 = 3 *+1 & cancel non-hero*, +48 set
*+%d special* (with +42's value), +46, +44, +42 and +40 all set *+%d to
stack* (+40), +52 = 1 *Cancel city bonus*, then the first set of +50 *%d
enemy stack*, +46/+44/+42/+40 *+%d stack in* hills/woods/open/city,
+38/+36/+34/+32 *+%d str in* hills/woods/open/city, else *-*. For a real army
the boat flag (0x1000) gives *Boat strength of 4* and a hero its command.

Dialog 34: Done (490, default and cancel), 491/492 one row up and down,
493/494 six (stopping at 0 and 21); the up pair greys at the top, the down
pair when the last row is place 27.

## View › Ruins

`.` is an inline case of `17be:0064`: the city dialog (`7204:0000`) in mode 4
at the cursor, on the nearest site — by straight-line distance, `1a8b:0acc` —
that is shown to the side (its bit in the site's mask at +0x1d) and, with a
hidden map, seen (`828e:06fd`). Dialog 6 with the mode buttons 193–196
hidden, so only Done (192) is left.

The map is `834b:05e4`'s, which also draws the sage's: every site shown to
the side, with a hidden map only where seen, gets an `ATRANS2.PCK` 16 × 10 —
(112, 20) a temple, (112, 10) searched (tile flag 0x40), (128, 0) rich
("Stronghold", +0x1b), else (112, 0) — at y = 2y − 1 and x = 2x − 1 rounded
to the nearest multiple of 8; the site asked about gets a white 12 × 10 box
(`2012:0bb7`/`0f08` lines).

The right half is `auto_ui_city_info`'s case 4 (`7204:0cdf`): the name in
font 1 centred on (432, 62); `SPECBITS.PCK`'s 96 × 63 at (328, 104) — (0, 0)
for a temple, else one of five by site number mod 5 (`4125:0ee8`) — framed in
black at (327, 103) 98 × 65 inside a (4, 2) bevel; *Type: …* (group 113, by
content) at (432, 112) and *Explored: …* (group 114) at (432, 136); a colour-2
box at (312, 180) 240 × 48 outlined in black with the four markers at
(320, 188), (432, 188), (320, 208), (432, 208) and their words (group 115) at
16 right and 3 up; and the site's three `.SPC` lines at (310, 259), 20 apart
(`7204:1f06`).

## View › Stack

`89e0:0c9c` lays the selected stack out at length on a copy of the bar's slot
arrays (group, in the moving group, mark; `89e0:0d30`), and its buttons are
the bar's own: an army (336–343, `89e0:157d`) joins the moving group or drops
out into one of its own, a mark (344–351, `89e0:166f`) makes its group the
one, Group (334) and Ungroup (335) are the Grp button's two ways. OK (332,
default) writes it back through `89e0:000a`; Cancel (333) restores the copy.

Popup 0 (`89e0:0e85`): `STACK.PCK` (0, 0) 400 × 23 at (80, 62) for the column
heads, *(max +%d)* — the scenario's combat cap, `2c04:0112` — at (480, 66),
and eight rows 30 apart from (112, 90): the mark (ABITS (448, 0) cross,
(448, 16) tick) at (80, y + 5); the army on a ring of colour
(side + group) mod 8 + 2, as its shadow when not moving; a non-hero's medals
(army +0x0a of them, ABITS 8 × 8s at `4125:2fa6`) from (x + 32, y + 8), two
to a column, unless ARMYTYPE +48 is set; name (x + 48), strength (x + 168,
a hero's with its battle items), for the moving group *(%d)* (x + 184),
moves (x + 232), all at y + 5; the movement icon at (x + 256, y + 8) — a hero
with a flying item flies, an army at sea shows ABITS (424, 30) — and the
bonus text (x + 304, y + 5; a hero *+%d hero bonus*, its command up to 6; at
sea *Boat strength of 4*). Rows past the stack get an empty grey ring.

*(%d)* is `89e0:1b9c`: the group's bonus — every hero's table value plus its
command items, and for everything else what its stack bonus for the tile's
terrain adds over the best so far, armies at sea left out — capped at the
combat cap, on top of each army's own strength with its battle items or
terrain bonus, capped at 9 (4 at sea). Note that it sums **every** hero,
where the battle itself takes only the strongest.

## View › Items, and the help pages

`66d4:0c21` is popup 4, (120, 50) 400 × 360: *Items* (group 166) in font 1
centred on (320, 52), *Items in this scenario* (group 167) in font 2 on
(320, 382), and a row per item from record 8 to 21 — the standards left out —
sorted by kind and then value (an insertion sort, `66d4:0c50`), 20 apart from
y = 90, in colour 7: the name ending at x = 304 and what it does from x = 336
(group 167: battle, command, flight, movement, gold per city). An item of a
kind it does not know reuses the row before's line. Dialog 35: Done (495,
default and cancel) and Bonus (496), which shows `HELP\HITEM.GFX` — the file
name is FILE.DAT group 0x45 (`7ecb:0505` reads FILE.DAT as `7ecb:04f0` reads
STRING.DAT).

A help page is a `.GFX` file (see docs/formats/gfx.md) laid out in a popup
and put away with any key or click. The control panel's small "?" (188,
`8065:104e`) shows `HELP\HMOUSE.GFX` and then `HELP\HKEYS.GFX`, both in
popup 4.

## Report › Diplomacy

`484e:0000` does nothing with the Diplomacy option (`2c04:011c`) off. Both of
its screens are popup 11, (80, 60) 480 × 350, and draw from `DIPLOM.PCK`
(bitmap 47) and `SHIELDS.PCK` — the 16 × 16 at (side × 40 + 24, 46) is
`8611:0bf7`'s size 3, the 40 × 40 at (side × 40, 0) its size 0.

**The Diplomatic Report** (`484e:0039`, dialog 21): *Diplomatic Report*
(group 107) in font 1 centred on (320, 64). A grid in colour 1: vertical
lines at x = 88 (from y = 139, 232 long) and x = 120 + 32 i (from y = 110,
261 long), horizontal at y = 110 (from x = 120, 256 long) and
y = 139 + 29 i (from x = 88, 288 long). Each side in play has its 16 × 16
at (128 + 32 i, 117) and (96, 146 + 29 i); the cell (120 + 32 c, 139 + 29 r)
shows side c's state toward side r — (384, 0) war, (320, 0) peace, nothing
for uneasy — and the diagonal (384, 29), for every side. Beside it, a black
outline at (392, 110) 160 × 262, *Diplomatic Rating* (group 109) centred on
(472, 116), and the sides in rating order (`484e:0aed`), each a 16 × 16 at
(400, 146 + 29 i) and its title (group 106) at (432, 146 + 29 i). Done (368,
default and cancel) and Action (369).

**The Diplomatic Action** screen (`484e:0382`, dialog 22): *Diplomatic
Action* (group 108) centred on (320, 64); the side's 40 × 40 at (96, 111) and
name at (144, 122); group 110's seven lines at x = 104, y = 165, 205, 251,
271, 291, 311, 331. Then for each other side, in order, a column at
x = 272 + 40 i: its 40 × 40 at y = 111; the state between you (its byte
toward you) at y = 151 and its proposal at 191, as `DIPLOM.PCK`
(80 + 120 state, 45) 40 × 40 — a blank box (black outline, colour 2 inside)
when the proposal is the state, and in every row for a side not playing —
and your proposal to it as three buttons at y = 245, 285, 325: (0, 45) peace,
(120, 45) uneasy, (240, 45) war, the one chosen lit, 40 to the right.
372–378, 380–386 and 388–394 set it (`484e:0a69`, the column mapped past
your own side). OK (370, default and cancel) closes; Report (371) goes back.

## History

`6d51:0000(n)` plays back the game's record (docs/formats/history.md); with
the turn at 1 it only says *'Tis only turn one, my liege!* / *There is no
history yet!* (group 93) in a message box. City, Events, Gold and Winners
(n = 0–3) share popup 10, (32, 60) 576 × 312, and dialog 19 (`6d51:0096`):
the strategic map at the popup's corner with the city shields as they stood
(`834b:12d3`), the title (group 91) in font 1 centred on (432, 64), four
tabs (352–355, the showing one lit), *Turn %d* centred on (312, 350), the
arrows 357/358 (a turn back and on, greyed at the ends) and Done (356).

The graphs (`6d51:034a`, the table at `4125:0dc6`: 300, 150, 292, 140): the
largest figure on record — at least 10 cities, 500 gold, 100 score — tops the
scale; axes in white with black shadows a pixel up and left, ticks at the
ends and the middle, 4 long; the top figure and *0* right-aligned at x = 292,
*0*, *Turns* and the last turn under the axis. Every side is a line in its
colour from (301, 288), a step per turn to x = 300 + t·292/n,
y = 289 − ⌊140·v/top⌋. The turn shown is a colour-13 line from y = 148 to 296
(region 10 picks a turn by x), and under the graph, centred on (432, 318),
group 92's line — your own figure, or on Winners the leader.

Events (`6d51:16ba`): that turn's deeds, 17 apart from y = 149, each its
side's turn-strip shield (ATRANS2 16 × 14) at x = 264 and the words at
x = 280, a treachery, war or peace adding the other side's shield after the
text, on the byte grid. Under them the timeline (`6d51:18da`, `4125:0dce`):
a box (272, 319) 320 × 17 bevelled twice and outlined in black, filled from
(275, 322) 11 high in your colour up to the turn shown and your edge colour
past it; region 11 picks a turn.

`kit.popup` fills popup 10's last 96 columns from MARBLE.PCK's next row: the
blit copies (0, 0, 576, 312) out of a 480-wide bitmap. That is read off the
code, not yet seen on screen.

**Triumphs** (`6d51:09eb`, popup 0, dialog 20): *Triumphs* (group 80); eight
tabs from (128, 109), 48 apart, `BUTTON.PCK` (400, 0) 48 × 40 — (400, 40)
for the one showing — with `BSHIELD.PCK`'s 32 × 36 at (side × 32, 0) on it,
8 in and 2 down; five rows 35 apart from y = 168: army types 4, 25, 28, 5, 29
in that side's colours on a grey ring at x = 104, and when the count is not
0 its line at (144, y + 9) — groups 81–85 on your own tab (your losses),
86–90 on another's (what you killed), singular or plural. The counts are
`2c04:1163`, 80 bytes a side and 10 an opponent, raised after every battle
(`67cc:1b43`): a hero, a creature with ARMYTYPE +48 set, or an army; a navy
too when it was at sea; and each standard a fallen hero carried.

## The right button: what is on a tile

A right-button press on a seen tile of the map (`740d:0037`, button 2)
shows a 256 × 75 box until the button comes up (`740d:11cf` puts the screen
back). `740d:131a` centres it on the tile's centre — ((col − scroll x)·40 +
36, (row − scroll y)·40 + 50), x rounded to the byte grid — kept within
x ∈ [128, 512], y ∈ [37, 441]; `740d:1201` puts `POPUP.PCK`'s (0, 0) 256 × 75
behind it. Its lines are centred on the box's middle less 8, at y + 11 and
y + 35. The first that applies:

- **a stack** you may see — your own, or any with View Enemies (`.SCN`
  0x12a) — as its armies side by side at y + 16, 24 apart, starting at the
  middle less 12 per army less 16, on the byte grid (`740d:0bad`). An enemy
  in a tower shows only *Tower* / *Very hard to conquer!* (group 130).
- **a city** (`740d:0c73`): the name in the owner's colours (a neutral city
  in yours); *Razed!* for ruins; else the owner's 16 × 16 shields at x + 24
  and x + 208, y + 10, `ABITS` (424, 0) at (x + 48, y + 36) with the income
  (+42) at (x + 80, y + 33) and (424, 11) at (x + 144, y + 36) with the
  defence (+20) at (x + 176, y + 33); a capital adds that side's 32 × 23 from
  `BSHIELD.PCK` (side × 32, 36) at x + 16 and x + 200.
- **a site** (`740d:0fd7`): the name in your colours, then *Blessings &
  Quests!* for a temple, *Explored!* (at y + 33) or *Unexplored!*.
- **a signpost** (terrain 9, `740d:0a1c`): its two lines over `POPUP2.PCK`.
- **the terrain** (`740d:10e1`): its name (group 128) in colour 7 and what
  it is (group 129) in white; a tile with a road is terrain 0, and a crossing
  (bit 15 of the map word) reads *Port* / *A way for armies to put to sea*.

## The end of the game

`8065:1aed` runs as the round ends. What it finds, and what the player sees:

- **nobody in play**: *Alas!* / *No more players are left!*, then *So I bid
  thee a fond 'FAREWELL'* / *Hit any key to return to DOS.* (group 12), and
  the game exits (`1a4c:02b5`).
- **the last human gone**, computers still in play: group 13's two message
  boxes, and the computers fight on.
- **one computer side left**: *%s, thou hast triumphed!* (group 15); the side
  is turned human so the world can be looked over.
- **a lone human with more than half the standing cities**: the game is won
  (`2c04:015b`), with a *victorious* deed. The remake shows group 15's two
  lines, then the Congratulations picture, and lets play go on.
- **surrender** (`2c04:015d`, offered once): a human with more than half the
  cities, and more than the biggest computer side by an eighth of all the
  cities. `8065:1f68` shows popup 20, `RESIGN.PCK`, with dialog 30 — Reject
  (486) and Accept (485). Accept (`8065:1e4e`) puts every computer side out,
  sets the won flag with a deed, and shows popup 22, `RESIGNYE.PCK`
  (*Congratulations!*, dialog 32, Done 483). Done there (`8065:2004`) turns
  the hidden map off and redraws. Reject (`8065:1ecd`) shows popup 21,
  `RESIGNNO.PCK` (*Peace is not an option!*, dialog 31, Done 484).

Popups 20–22 are (136, 40) 368 × 390, each its own picture with the words
painted in. Group 16 (*At thy leisure / thou mayst inspect thy kingdom*)
fits after Congratulations, but where the original shows it is not traced.

## Game › Save game and Load game

Ten slots, named in `SAVEINFO.DAT`: a line a slot, a three-digit length
counting the name's NUL, then the name — `009Not_Used` for an empty slot.
**Save** (`7721:093b`) lists all ten in the list chooser titled *Save Game*
(group 46), on the slot last used (`4125:128e`). The slot chosen
(`7721:0995`) asks for a name through the text-entry dialog — *Type the name
of the game* / *you wish to save*, at most 15 characters and 160 pixels —
and is written to `SAVE\SAVEn.DAT`. **Load** (`7721:026b`) lists only the
slots in use, titled *Load Game* (group 47); once loaded, *%s, thy turn
continues!*. The remake keeps its slots beside its own save file
(`warlords-save<n>.lua`, `warlords-saveinfo.txt` in the same form).

## SSG › About, and "?"

`7721:0084` pushes popup 23, (160, 55) 336 × 347: `BSCROLL.PCK` (bitmap 70)
through its mask — `54f6:0000` masks popups 13 and 23 (`1997:027e`) and
leaves them unframed. `8065:1471` then writes the lines after the first
(for popup 23 the title is painted on the scroll), centred on x = 328 at
y = 161, 181, 201, 221, 241, 261, in font 2 black edged in colour 7, an empty
one leaving its place: "", *Version 1.02* (`4125:1292`), "", and *Free
memory*, *Largest block* and *Locked memory* in Kb (group 139). Any key or
click closes it. The remake has no DOS memory figures to show and leaves the
last three out.

## Game › Settings

`64d2:0000(n)` — n = 1 from the menu, 0 when setting a new game up — is
popup 4, (120, 50) 400 × 360, dialog 9 (`64d2:0137`, tables from `4125:0ac4`).
Heads in the side's colours: *Name* ending at x = 248, *Human*, *Enhanced*,
*Observe* centred on 280, 396, 468, at y = 60. A row a side from y = 90, 30
apart: its name in its colours ending at x = 248; a side out of play shows
*Deceased!* at x = 280 (nothing if its name is *Not used*); else `ABITS`'
box — (320, 0) ticked, (320, 20) clear — at (256, y) for Human (`.SCN`
0xd0), then *Human* or the level (0xc0: *Knight*, *Lord*, *Warlord*) at
(280, y), the Enhanced box (0xf0) at (400, y), and for a computer side the
Observe box (0x147) at (448, y). Under them Music (152, 340), Effects
(152, 370) and Speech (256, 340), their words 24 to the right. 252–259
toggle Human (`64d2:04cc`), 260–267 Enhanced (`0508`), 268–275 Observe
(`053f`), 276–278 the sounds — Music and Effects greyed without a sound card
(`4125:37a3`, `37a2`) — and OK (251) closes (`64d2:0034`).

## Game › Shortcuts

`545c:0000` is popup 0, dialog 14 (`545c:014a`): *Menu Shortcuts* in font 1
centred on (320, 63); the 21 items of `UDB.DAT` — a u16 count, then 68-byte
records (`545c:04ac`): the item's command at +0, its name at +2, and at +52
its button's rect in `MENUBUTT.PCK`, 64 wide with the lit half on the right
— three to a row from (96, 100), 152 and 30 apart (`4125:04ce`), each its
32 × 29 button (lit when it is on the slot being set) and its name 40 right
and 5 down; *Choose 4 buttons for shortcuts* centred on (320, 320); and the
four slots from (96, 342), 40 apart (`4125:0522`), each its item's button or
the blank at (256, 84), the slot being set outlined in colour 9. 316–319
pick a slot (`545c:00e9`), 295–315 put an item on it (`545c:0053`), OK (294)
writes `UDB.CUR` (`545c:0032`). BUTTON.DAT puts the third column's hit areas
at x = 300, not 400. The remake keeps the choice for the session.

## The tutorial's pages

With the Tutorial option on (`.SCN` 0x12e), eleven help pages (FILE.DAT group
0x19, `TUTORIA\T*.GFX`) are each shown once, through the help box
(`8065:168d`) — each file's `#D` picks popup 14 or 15. The ones with a bit in
`2c04:0134` set it: THERO the hero offer (`7563:11f5`, 0x01); TPROD the city
dialog (`7204:027f`, 0x02); TSELECT closing it (`7204:0321`, 0x04); TMOVE a
stack picked up (`1b62:051d`, 0x08); TFIGHT the same, next to an enemy city
(`1b62:05af`, 0x10); TPROD2 the city dialog with two cities (`7204:02c5`,
0x20); TTURN2 the second turn (`8cc6:05df`, 0x40); TSEARCH a hero picked up
on a site (`1b62:062a`, 0x80). TENDTURN (`1c8c:02db`, on turn 1) and TFRESULT
(`63fa:0283`, the first battle's result) go by other tests. TWARLORD
(`7bab:00d1`) waits on a count of 10 not yet traced.

## The start screens

With no scenario named the remake opens as the game does, on `7f77:0000`
(dialog 1): the four quarters `STARTUP0`–`3.PCK`, with New Scenario (100),
Load Game (101), Random Map (102) and Begin (103). On the right the chosen
scenario's own `PICS\SCENARIO.PCK` — (0, 0) 264 × 225 at (328, 200) — under
a colour-3 bar (336, 166) 248 × 28 with its name centred on (460, 172)
(`7f77:02bf`, `0332`); Erythea to begin with (`4125:2a6e`). Random Map needs
the map generator, which the remake does not have, and is greyed.

The menu bar stays up over the start screens, and live: `7f77:0200` greys
every menu (`2372:0eab(0, 0)` — a menu's enabled items are a bit mask at
`+0x1a` of its 46-byte record) and then lets on, through `2372:0ef8`, only
Game's **Quit** (item 505), **Load game** (502) while `7721:0e25` counts a
used save slot, and **Load map** (504) while `7721:0e42` counts a saved map.
The items are numbered in menu order from About (497) to End Turn (535) —
the same numbers `UDB.DAT` gives the shortcut items. The setup screen paints
the same bar (`7bab:0034` → `2372:132f`).

**New Scenario** (`7f77:058d`, `0725`) lists `SCENARIO.DAT` — 84-byte
records: name +0, directory +20, description +28, and at +76 its cities,
ruins and players — on popup 19 (`NEWSCEN.PCK`), dialog 29: the names in a
black box at (112, 153) 136 × 140, seven rows 20 apart, the chosen one in
colour 15 and the rest in 5 (`7f77:0d45`); on the crystal ball in font 2
colour 5 the name centred on (416, 101), the description on (416, 174), the
cities ending at x = 368 and the ruins from x = 472 at y = 234, the players
centred on (416, 254). 476–482 choose, 470–473 scroll, OK (474), Cancel
(475).

**Begin** (`7bab:0000`) sets the game up on the main screen's own frame
(`8065:0a10`), dialog 3. A box a side, 160 × 70, at (24, 40) and (208, 40)
down two columns 90 apart (`4125:23c8`), framed in the side's colour and
edge colour, its name on a colour-3 tab at (x + 24, y − 8) in its colours,
its face (`SETUPBU.PCK`, `4125:2408`) at (x + 16, y + 16) — a computer's by
level, the two human faces in turn, an empty one for a side not in the
scenario — and its button (`4125:24b8`) at (x + 72, y + 12). A click on the
button (125–132, `7bab:0a4e`) goes Human → Knight → Lord → Warlord → Off →
Human; Off (level 3) leaves the side out, its capital neutral
(`7bab:0cfe`). The right-hand panel: *Options* and *Game* (group 7) centred
on x = 512 at y = 48 and 206; Beginner, Intermediate, Advanced (145–147,
`7bab:1273`, lit when the options are theirs, the table at `4125:2378`),
Edit Options (148), *I am the Greatest* (143, which makes every side a
computer Warlord) or *No! I really am Normal* (144), Begin (141, needing a
side not Off) and Main Menu (142). Under the map *Difficulty Rating %d%*
(group 6) centred on (196, 426): the options' weight — Neutral Cities × 4,
Diplomacy × 4, Quests × 3, Hidden Map × 4, less View Enemies, plus View
Production, at most 20 — plus the computers' strength, 80 × Σ(level + 1) /
(3 × computers), 80 from 78 up or with no computer (`7bab:0bab`).

**Edit Options** (`7bab:12a2`): popup 4, dialog 4. *Game Options* (group 8)
in font 1 centred on (320, 55); *Affecting Difficulty* and *Not Affecting
Difficulty* (groups 9 and 10) on rules at y = 111 and 250 (white, and black
a pixel down and right); the ten options of group 4 in the order of
`4125:23b4` two to a row, names from x = 128 and 320, 30 apart from y = 131
and y = 270, their values in colour 7 128 to the right — Neutral Cities
*Average*/*Strong*/*Active* (group 5), the rest *On*/*Off*. 159–168 change
one, 169–171 the presets, Done (172).

Left out: the Character boxes and Random Characters, which choose the
computer players' personalities (dialog 27, `7bab:16ea`, `2051`), and Recall
Options (`7bab:2229`).

## Report › Quest

`auto_ui_no_quest` (`4976:0167`) is popup 2 with dialog 16's Done (330) and the
map with no city shields: *Quest* (font 1) centred on (432, 62), and with no
quest — or its hero gone — a random line of group 20 on (432, 134). With one,
`SCROLL.PCK` blitted through its mask at (304, 95), and on it in font 2, black
on yellow (`78a8:06ae(2, 0, 7, 1)`), *%s's Quest* on (432, 145), a black rule
at (360, 165) 160 long, and the quest's lines centred on x = 432 at
y = 175, 195, 215, 235, 245, 265, 285 as each type uses them (`4976:0320`,
groups 21-27), with the compass point to the target from `828e:0b51` (the
eight words at `4125:2b7c`). The map shows an orange (8) line from the hero to
the target, both at (2x − 2, 2y − 2), a little orange shield in black at the
target (`828e:08cd`) inside an orange 8 × 8 box (`828e:099e`); a quest to slay
a kind of army, or a side's armies, puts a banner on every such stack
instead. The hero's figure goes on top.

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
