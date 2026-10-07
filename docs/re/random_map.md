# Random map generator

How "A Random World" is made in `WARLORD2.EXE`, decoded in full: the start
menu's settings, every phase of the generator, and the files it writes. The
web port follows it phase for phase (`web/src/warlords/randommap.js`, which
cites the addresses below). Addresses are Ghidra addresses.

Where Ghidra's decompiler is wrong here it is wrong quietly: it drops
arguments passed as one 32-bit push (`PUSH 0x10010` is two words, 0x10 and
1) or left on the stack by an earlier call (`4d71:0962` pushes `0x60001`
before calling `dice(1, 2, 1)`, and the 6 stays behind as the next call's
last argument), and it skips code behind a jump it cannot follow
(`4fef:005c`, part of `4fef:0464`). Read the disassembly before trusting a
call's arguments.

## The start menu

Random Map (control 102, `7f77:05f5`) sets `DS:28c8` and the menu shows, in
place of the scenario's picture, the random world's settings (`7f77:0332`)
on colour 3 in (328, 200) 264 x 225, with *A Random World* (group 0) on the
name bar:

| control | what | where |
|---|---|---|
| 106-109 | the Water, Hills, Cities and Forest sliders (`DS:28d0`, 0-6) | `STARTBU.PCK` (496, 80 + 20 x value) 120 x 20 at (384, 216 + 30i); name (group 2) ending at x = 376; value from x = 504 |
| 110-113 | a slider's "?" (`DS:28d8` cleared) | lit while "?"; the slider is drawn bare, (496, 220), and reads *(?)* |
| 104 | the terrain set (`DS:28ca`), from `DATA\TERRAIN.DAT` | (336, 340) |
| 105 | *Cities can produce allies* (`DS:28cc`, group 3) | (336, 365) |

A click on a slider (`7f77:0512`) sets it to `(x - 384) x 7 / 120`, or, if
it was "?", just sets it again. What a value means is shown beside it from
`4125:28e0`: Water and Hills 5-17%, Cities 70-100, Forest 9-21%. They start
at 3, 3, 2, 3, all set, terrain set 0, no allies. New Scenario's choice
(`7f77:067c`) clears `28c8`; coming back from a random game sets it again.
`TERRAIN.DAT` is a `u16` count and 52-byte records, the name at +2; it has
one set, *Terrain type: Grassland*, so 104 does nothing visible.

**The "?" does nothing.** `random_map_setup` rolls a slider only when its
value is 7 (`1d7 - 1`), and no click can make one 7; the "?" flag is only
drawn. The web port passes 7 for a "?" slider.

Begin (`7f77:060f`) has the advisor say *One moment...* (`6dda:026f(0)`,
`VMOMENT`), pushes popup 23 (the `BSCROLL.PCK` scroll) with group 136's
*Creating a new random map* / *Please wait...* (`8065:1471`), and calls
`random_map_setup`. The progress (`4bed:01ff`) is `RMAPBAR.PCK` (0, 0) cut
to `(p + 10) / 10 x 16 + 16` wide, 21 high, at (232, 257) on black, with
"p%" centred on (328, 237). A preview of the map as it is drawn, two pixels a
tile from (400, 30) (`4bed:03aa`), is switched off (`DS:26c2` = 0).

## Entry (`random_map_setup`, `7bab:10e8`)

1. A random 6-character name, each character `1d90 + 32` (`DS:5da8`).
2. The four sliders (7 rolled `1d7 - 1`) go to `random_map_load_params`.
3. The scenario becomes `random\` (`DS:1672`), its terrain set `.SCN`
   `0x161`; `.SCN` `0x120` marks the game as a random one
   (`auto_ui_options_menu`).

`random_map_load_params` (`4bed:0000`) reads `RANDOM\RANDOM.DAT` whole into a
`0xa560`-byte buffer, adds the slider tables to the parameters and runs the
generator. That buffer is the generator's memory: the parameters, its tables,
its working space and the 112 x 156 terrain grid at `+0x6120` are all in it.

## `RANDOM.DAT`

| offset | contents |
|---|---|
| `+0x00` | 12 colours for the preview, by terrain type |
| `+0x2a` | cities, 80 |
| `+0x2c` | points along each of the 4 edges of the coastline: 5, 6, 5, 6 |
| `+0x34`, `+0x36` | mountain and hill seeds, 16 each |
| `+0x38`, `+0x3a` | wide and narrow rivers, 5 each |
| `+0x3c` | forest, per hundred land tiles, 15 |
| `+0x3e`, `+0x40` | passes and erosion walks, 4 each |
| `+0x42` | 10, not read |
| `+0x44`, `+0x52`, `+0x60`, `+0x6e` | slider tables, 7 `i16`: Hills (added to `+0x34`, `+0x36`) −6 −4 −2 0 4 8 12; Water (`+0x38`, `+0x3a`) −3 … 3; Forest (`+0x3c`) −6 … 6; Cities (`+0x2a`) −10 −5 0 5 10 15 20 |
| `+0x7c` | the map's 4 corners |
| `+0x8c` | the 4 edges as start and end points, clockwise from the top |
| `+0xac` | 4 diagonals, outward from each corner |
| `+0xbc` | the 8 neighbours: S, SE, E, NE, N, NW, W, SW |
| `+0xdc` | the 8 again, N first and clockwise, for road shapes |
| `+0x264` | 256 bytes: an 8-neighbour mask to a tile variant, −1 for none |
| `+0x364` | 256 bytes: a mask to a road id − 1, −1 for none |
| `+0x464` | per terrain type, 64 bytes: 16 variants x 2 tiles, by checkerboard |
| `+0x764` | side names, 5 a side, 20 bytes each |
| `+0xa86` … `+0xe6e` | city name syllables: 20 first, 20 middle, 20 last; 10 endings each for forest, shore, marsh, hills and any |
| `+0xed2` | 5 ruin name formats (*%s's Tower* …) and the words they take |
| `+0x1000` | 10 temple words (*Dark*, *Ancient* …) |
| `+0x1068` … `+0x17e8` | the city descriptions' word tables, 16 bytes a word |
| `+0x1888`, `+0x189c` | signpost formats *%s* and *%d leagues %s* |
| `+0x18b0` | 9 signposts of its own, two 30-byte lines each |
| `+0x1d62` | each side's army class: 4 1 1 3 2 4 4 3 |
| `+0x1d72` | each region's army class, filled in |
| `+0x1d82` | 29 production records, 16 bytes: type, chance (of 10), region class, terrain class, then strength, time, cost and move, filled in from `ARMYTYPE.DAT` |

The rest is working space: the coastline's points (`+0xfc` … `+0x264`), the
forest count (`+0x1f52`), the `.CTY` and `.SPC` text (`+0x1f56`, `+0x3e96`,
their lengths at `+0x1064`, `+0x1066`), the hill and mountain nodes
(`+0x5dd6`, 14 bytes each, the count at `+0x611e`) and the grid.

The grid holds terrain type ids (`docs/rules.md` > Movement). The generator
reads a tile or two off its edges here and there, which land in the bytes
before or after it.

## Pipeline (`random_map_generate`, `4bed:011c`)

| % | function | |
|---|---|---|
| 0 | `auto_file_x_scn`, `4bed:036b` | Erythea's `.SCN` (the scenario whose `SCENARIO.DAT` +82 is the terrain set) is loaded as the template; the grid starts as **water**, roads cleared |
| | `4c49:0000` | the coastline |
| 10 | `4d71:0000` | mountains and hills |
| 20 | `4f5f:0000` | erosion and passes |
| 30 | `4eb7:0000` | rivers |
| 40 | `4e47:0000` | forest |
| 50 | `4f5f:06a0` | marshes |
| 60 | `513d:0000` | city sites |
| 70-79 | `513d:003a` | ruins and temples |
| 80-89 | `5311:0000` | tiles, bridges, crossings, roads |
| 90-98 | `4fef:0014` | road shapes, the cities, sides, signposts; the files |

Common pieces: `4c49:0dd0` is the step from one tile towards another, one of
8 by the slope, cut at tan 22.5° and tan 67.5° (`4125:0292`); `4c49:0fed`
is the Chebyshev distance; `4bed:0470` clamps to the map; `terrain_near`
(`4c49:11f5`) asks whether a type is on any of the 8 neighbours; `dice` with
no sides gives the bonus, with fewer than none `n + bonus`.

A **wandering line** (`4c49:0a0e`, and the ridges and walks below) re-aims at
its end each step: within 3 it steps straight on; otherwise `1d3` — 1 aims,
2 repeats the first step, 3 turns the step by −1 to +3 eighths (`4c49:104a`,
`1d5 + i − 2`) and makes that the new first step.

### The coastline (`4c49:0000`)

1. Each edge's two ends (`4c49:0087`): the corners drawn in 10 along the
   diagonals, each pushed by `(1d3 − 2) x 1d16` either way and, 30% of the
   time, moved to the nearest map edge (`4bed:04ae`). An edge's end and the
   next one's start swap if that turns the outline round the middle
   (`4c49:0206`).
2. The points between (`4c49:02f9`): an edge's count of points less one
   steps from its start to its end, each pushed by up to half a step; an
   edge whose ends both lie on the map's edge is just its two ends.
3. The points joined (`4c49:05c7`): two on the map's edge run along it,
   round a corner where they are on different sides (`4c49:0b0e`); the rest
   wander. The outline is drawn as plain.
4. The sea floods in from the map's edge, two rounds of four sweeps
   (`4c49:0867`, `10bd`), and stops at the outline: everything it does not
   reach is land.
5. Water beside land is shore (`4c49:0ce7`). Half the time 0-2 channels
   cross the land from the west edge to the east, seven tiles wide (a river
   with `4eb7:005d(1, 0, 1)`), and the water is tidied (`4eb7:060c`); shore
   again.

The continent fills most of the map, and where corners went to the edge it
reaches it: after the coastline the sea is typically about an eighth of the
map, from 3% to a quarter.

### Mountains and hills (`4d71:0000`)

1. `+0x34` mountain seeds, then `+0x36` hill seeds, on random plain tiles;
   each is a node (x, y, kind, links, up to 3 links).
2. Each node links to its nearest others: a hill `1d2 − 1`, a mountain
   `1d4 − 1` (`4d71:057b`).
3. Along each link a ridge (`4d71:09a7`) wanders to the other node, laying
   its terrain on plain or hills: hill to hill is hills, three across (the
   middle and the tiles two out); anything with a mountain is mountains,
   `1d2 + 1` across (2: the middle and the tiles beside it). A node with no
   links gets a short ridge of its own towards `1d5 − 10` up and left.
   Only one of the side tiles is clamped to the map.
4. Tidy (`4d71:002e`, `033a`): plain with mountains (or hills) on all four
   sides joins them; a mountain beside shore becomes plain; a mountain with
   mountains on one diagonal and not the other gets one of the other
   diagonal's tiles too (`1d10`); everything beside a mountain that is not
   mountain or hills becomes hills.

### Erosion and passes (`4f5f:0000`)

`+0x40` erosion walks, then `+0x3e` passes, each between two random tiles.
Where the walk stands on mountain (erosion) or mountain or hills (a pass),
the tile, the tiles 45° to either side and two out become hills (erosion) or
plain (a pass) — whatever they were. Then hills with mountains on three of
four sides become mountain, plain with hills on three sides hills, and the
foothills again.

### Rivers (`4eb7:005d(wide, stops, channel)`)

From a random hill or mountain tile to a random water or shore tile, 200
tries each (the last tile tried if none is found). Each step re-aims and
lays water on the tile and the tiles 45° to either side (`wide`: and two
out; a channel: and three out), then `1d4` — 2 or 3 — sidesteps 90° one way
or the other, unclamped. With `stops` it ends the second time the tile
ahead is water. `+0x3a` narrow rivers, then `+0x38` wide ones; then
`4eb7:060c` — land with water or shore on three of four sides goes under,
shore with no land among its 8 neighbours becomes water, a diagonal of
water gets a shore tile beside it — shore, the mountain tidy, foothills,
`060c` and shore again.

### Forest (`4e47:0000`)

Woods until the forest count reaches `land / 100 x +0x3c` (land is plain,
hills and mountains). A wood (`4e47:00b5`) starts on a random plain tile and
grows eight arms, `1d8 + 2` long (65%) or `1d10 + 5`; off each step of an arm
grow two side shoots at 90°, `1d(4 − k/2) + 2 − k/4` and
`1d(4 − k/2) + 2 − k/3` long at step k (`6`/`4` for the big woods), so they
shorten along the arm. Only plain turns to forest. After each wood
(`4e47:03d9`) plain with forest on three sides is forest, and a diagonal of
forest gets a tile beside it (over hills, shore or plain).

### Marshes (`4fc9:010a`)

`1d3` of them. Each on a random plain tile at least 6 from the edge; up to 4
such tiles are tried for one with shore beside its 2 x 2 footprint, then any
will do. The tile is marsh, and round it and its four diagonals
(`4fc9:0029`) `1d5 + 3` tiles each, `1d3` steps along one random
neighbour's x and `1d3` along another's y, turn to marsh if plain.

### City sites (`513d:0331`)

`+0x2a` of them: a 2 x 2 footprint at least 6 from the edge, none of it
shore, water, city, mountain or hills, and no city among the neighbours of
any footprint tile or of the tiles diagonally beyond. Up to 4 such places
are tried for one on the coast, then any will do. The footprint becomes
city.

### Ruins and temples (`513d:003a`)

Forty sites; each keeps the template's kind (Erythea's sites 0, 15, 20 and 35
are temples). A place (`513d:0a77`): the map is cut into 16 cells, taken in
turn from a random one, and 50 tries in the cell for plain clear of cities
and sites and of sites 3 tiles off each way; then 50 anywhere for plain
clear of both; then 50 for anything not a city or site. A temple is *%s
Temple* (*The %s can bless your / armies or give you / quests*); a ruin is
one of the five formats with two syllables (*Kirok's Tower*, *Ruins of
Volaz*; *%s is / inhabited by monsters and / full of treasure!*).

### Tiles, bridges, crossings, roads (`5311:0000`)

**Tiles** (`4fef:005c`): every grid type its variant 0 by checkerboard (x + y
even takes the second); a city's top-left tile the castle block 96, 97, 112,
113; a site 10 or 12, and the first eight sites tile 11. Then twice
(`4fef:0464`) each kind takes the variant its neighbour mask gives
(`+0x264`; a mask bit for each neighbour of the kind, or off the map): water
and shore together (bridges count as water) — and where no variant fits it
becomes **marsh** and spreads marsh round it (`4fc9:0029`); forest; mountains
(no fit: hills); hills, counting mountains (no fit: plain half the time,
else hills variant 13); forest with no fit is plain half the time, else
variant 13. Marsh is variant 0 seven times in ten, else one of 1-4. Water and
shore are written back as **water** — no shore is left in the grid after
this.

**Bridges** (`5311:05b4`): tiles 0x26 or 0x16 with 0x28 or 0x18 to the east
(a river two tiles wide running north–south), or 0x21 or 0x11 with 0x24 or
0x14 below, up to 50 of them. As compiled, only a 0x26 tile is ever taken,
and only as an east–west bridge, with plain, forest or hills at x − 1 and
x + 2 (or at y − 1 and y + 2). They are chosen by `5311:08f6` — each still
free and at least 10 from those chosen scores `1d15 + 1`, plus `30 −`
distance to the nearest city when that is under 31, and the best is taken
until none is left — and become tiles 0x85, 0x86 (a north–south one would be
0x84 over 0x94), terrain bridge, with a road mark either end.

**Crossings** (`5311:01f6`): half the shore tiles 0x20-0x25 (each checked on
`1d2`) that face three tiles of water (0x20 and 0x23 to the west, 0x21 the
north, 0x22 and 0x25 the east, 0x24 the south) and lie more than 14 from any
bridge or crossing are candidates; up to ten are chosen as bridges are,
without the spacing, and get the map word's bit 15.

**Roads**: pairs of cities (`5311:0e22`): a random one not yet served (80
tries, else done), and of the unserved ones 30 to 70 away the one rolling
highest on `1d1000`; both are served. Each end is one of the 12 tiles round
the city's footprint that is plain, forest or hills (8 tries, `5311:0ad0`).
The road follows the game's own path from one to the other
(`5311:0c1c`), found as pseudo-player 14:

- `path_build_cost_grid(14)` costs from `DS:01e0` — road 1, bridge 1, water
  3, shore 3, forest 4, hills 5, mountains 7, plain 2, marsh 5, tower 2,
  city 1, site 7 — so nothing is impassable but the cities, which belong to
  nobody; a tile with a road costs 1, as for any side;
- land rules: water is entered or left only at a bridge or crossing;
- one pass of the wavefront (`DS:00e0` stops the second), spreading in the
  interior to the 4 straight neighbours only (class 9);
- the trace steps diagonally only onto water that is not a bridge, and
  takes a first neighbour no nearer than where it stands.

Every tile the road steps onto (not the first) gets a road mark unless it is
water, shore or bridge; every six steps the cities within 10 count as
served. At the end sites lose their road marks.

### Finishing (`4fef:0014`)

1. **Road shapes** (`4fef:0a4b`): each road tile's mask of neighbouring roads
   and bridges, N first and clockwise, gives its piece (`+0x364`); where none
   fits the tile below becomes plain or hills variant 13 and the mark stays.
   Then `1d10 + 10` straight north–south pieces become 17 and as many
   east–west ones 16: the same road with standing stones beside it.
2. **The cities** (`513d:05e3`): read back off the tile map in rows, top-left
   tile 0x60: owner 15, `+20` = `1d3 + 2`.
3. **Capitals** (`513d:0689`): the map is 8 regions, 4 across (28 tiles) and
   2 down (78). Each side in turn takes a region of its own at random and up
   to 100 tries at a random city there (`513d:08f0`, 202 tries, else the last
   city tried anywhere) at least 12 from the map's edge and at least 20 from
   every other capital in **both** x and y. Only five columns fit that
   spacing, so the eighth side never finds one: after 100 tries — a find on
   the hundredth counts as none — the whole choice starts again, and in the
   tenth round each side takes its last try. A region takes its side's army
   class (`+0x1d62` into `+0x1d72`); the capital is stamped in the side's
   castle (`513d:09b3`).
4. **Sides** (`5132:0014`, `006b`): each side one of its five names, and
   `3d50 + 20` gold.
5. **City details** (`random_map_cities`, `513d:0ce0`), for each city, from
   what touches its footprint in the grid:
   - **value** (`513d:1104`): a capital 9; else `1d4 − 1`, +4 by shore, +2
     by road, −1 by forest, −2 by marsh, within 0-9. Since the grid holds no
     shore after the tiles were laid and never holds a road, the +4 and +2
     never apply, and neither does anything else that asks for shore: no
     city makes a navy, and no name ends in a shore word.
   - **name** (`513d:1468`): first syllable, a middle one 6 times in 10, and
     7 times in 10 a word for what lies round it — marsh, forest, (shore),
     hills, else 4 in 10 any — or else a last syllable.
   - **income** (`513d:1171`): `value x 2 + 1d8 + 14`.
   - **description**: *%s is a %s / %s, %s / %s %s* — an adjective and a noun
     picked by `1d3 + value − 2` (within 0-9) each, so richer cities sound
     grander; then by the land (*located near / an ancient / forest*: 1-4 by
     forest, 5-8 by hills, 9-10 by marsh) unless it has none of those or
     `1d100` > 79, when it is what it is renowned for or built with.
   - **production** (`513d:161d`): `value / 2 + 1d4 − 1` slots, +1 by
     forest, +1 by hills, at most 4. The 29 records are taken in order: an
     ally (magical, `ARMYTYPE +48`) is skipped unless allies are on; the
     type must roll under its chance on `1d10 − 1`, and suit the city — its
     terrain class 7 (anywhere), 4 by forest, 5 by hills, 3 by shore, or its
     region class the region's — and a navy needs shore. None at all gives
     the first record's type. A city of value 7 or more, half the time,
     with room and no flier yet, adds the first suitable flier that is no
     ally. The slots carry the type's strength, time, move and cost.
6. **Allies** (`513d:1b3f`): with *Cities can produce allies* on, `2d3`
   random cities with room also make a random magical type
   (`random_magical_type`).
7. **Capitals' armies** (`513d:1c1d`): each capital puts in its highest
   empty slot (or the last) a type of strength 5 or more it does not make,
   no navy and no ally, from up to 40 draws of `1d28 − 1` — one found on the
   fortieth is dropped.
8. **Signposts** (`4fef:113d`): `1d30 + 40`, each on a random plain tile
   with no road and no city beside it; the tile becomes 0. Three in ten take
   the next of RANDOM.DAT's own while they last; the rest name the nearest
   city and *%d leagues %s*, twice the map distance and the compass
   direction.
9. **Files** (`4fef:027c`, `10cf`): `RANDOM\RANDOM.MAP`, `.RD`, `.CTY`,
   `.SPC`, `.SGN` and `.SCN` — the template's, with the sides' names, gold
   and capitals, 40 sites and the cities rewritten; then the scenario is
   loaded from them as any other, and `SAVE\RMAPTEMP.DAT` written. A save of
   a random game carries the `.SPC` and `.CTY` (`docs/formats/save.md`).

A random world has 70 to 100 cities; the `.SCN` has room for exactly 100
65-byte records.
