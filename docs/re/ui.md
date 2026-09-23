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

### The assault

A city is **not walked into**. `walk_path` stops the moment the next step is a
city the mover does not own and hands that tile to **`attack_tile`**
(`67cc:0000`), so the stack always fights from one of the eight tiles around
it, diagonals included. `attack_tile` refuses outright unless the target is a
city or a tile with armies on it, and unless the mover has a movement point
left to spend.

What the player then sees is a **replay**: `combat_resolve` decides the whole
fight first and records a byte per casualty in `combat_log` — **1** an
attacker fell, **0** a defender — and the window plays that log back. Nothing
on screen can change the outcome.

| step | what |
|---|---|
| `67cc:1836` | the fire cloud from `WAR.PCK` over the tile — the rect at `4125:0cfa` is `(0, 0) 128 × 120`, three tiles across — with `WAR.8SN`. Skipped when no human can see the tile |
| `6a35:041c` | opens **popup 8**, `(160, 60) 320 × 312`, and draws the two sides' shields from `BSHIELD.PCK` (32 × 36 cells, one per side across the sheet): the defender's at `(176, 86)`, the attacker's at `(176, 246)` |
| `6a35:0160` | draws both lines |
| `6a35:0094` | plays the log back, one army struck off at a time, with a sound and a wait each; **space** runs it through (it polls `kbhit`) |
| `6a35:04c5` | writes how it ended, centred on x = 320, each line 20 below the last |
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

The bottom bar is not the screen's own art either. `8065:0aeb` blits
`MARBLE.PCK` over the rect at `4125:2a9c` — **(16, 403) 360 × 66**, from the
marble's (0, 60) — before `89e0:05a3` draws the side's standing or
`89e0:0356` the army slots.

### Default and cancel buttons

Enter and Escape reach the first live control in one of two lists of control
ids (`4125:155c` and `4125:1514`), whichever dialog is up; "live" is state 1.
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
