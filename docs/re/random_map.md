# Random map generator — structure

How "A Random World" is built in `WARLORD2.EXE`. The overall flow and the
parameters are decoded; most phases aren't read in detail. Addresses are
Ghidra addresses.

## Entry (`random_map_setup`, `7bab:10e8`)

1. A random 6-character name, each character `1d90+32`.
2. Four sliders from the setup screen (`DS:28d0`, `STRING.DAT` group 2 order:
   **Water, Hills, Cities, Forest**), each 0–6. A slider set to 7 means random
   (`1d7−1`).
3. The terrain set goes to `.SCN` `0x161`; the "Cities can produce allies"
   choice (group 3) is `DS:28cc`.
4. `random_map_load_params` (`4bed:0000`) loads `RANDOM\RANDOM.DAT` into a
   `0xa560`-byte buffer, applies the sliders and runs the generator. The
   result is written through the normal scenario files (`RANDOM\RANDOM.SCN`,
   `.MAP`, `.RD`, `.SGN`, `.SPC`, `.CTY`) and `SAVE\RMAPTEMP.DAT`.

## `RANDOM.DAT`

42336 bytes. The first `0xcc` bytes are parameters; the rest is working space
the generator fills in, including a 112×156 terrain-type map at `+0x6120`
(empty in the file).

**Slider tables** (`u16` × 7, indexed by the slider value) are added to base
parameters:

| slider | table | adds to | base (file) | adjustments for settings 0–6 |
|---|---|---|---|---|
| Water | `+0x52` | `+0x38` and `+0x3a` | 5, 5 | −3 −2 −1 0 +1 +2 +3 |
| Hills | `+0x44` | `+0x34` and `+0x36` | 16, 16 | −6 −4 −2 0 +4 +8 +12 |
| Cities | `+0x6e` | `+0x2a` | 80 | −10 −5 0 +5 +10 +15 +20 |
| Forest | `+0x60` | `+0x3c` | 15 | −6 −4 −2 0 +2 +4 +6 |

The units of these parameters (counts, seeds or percentages) depend on the
phase code and aren't confirmed.

Other parameter data seen: map corner coordinates at `+0x7c`, diagonal step
directions at `+0xac`, and the 8 neighbour offsets `(dx, dy)` at `+0xbc`,
used by `terrain_near` (`4c49:11f5`: "is terrain type *t* on any of the 8
tiles around (x, y)").

## Pipeline (`random_map_generate`, `4bed:011c`)

Progress is reported at each step (the percentage passed to `4bed:01ff`):

| % | function | what's known |
|---|---|---|
| 0 | `auto_file_x_scn`, `4bed:036b`, `4c49:0000` | load the template scenario, initialise |
| 10 | `4d71:0000` | uses random map coordinates (`1d112`, `1d156`) |
| 20 | `4f5f:0000` | random coordinates |
| 30 | `4eb7:0000` | random coordinates |
| 40 | `4e47:0000` | random coordinates, `1d100` checks |
| 50 | `4f5f:06a0` | |
| 60 | `513d:0000` | cities: per-city setup below |
| 70 | `513d:003a` | sites (writes "%03d\|%s is\|inhabited by monsters and\|full of treasure!\|" descriptions) |
| 80 | `5311:0000` | roads: builds the path grid as pseudo-player 14, using the map-generator cost table `DS:01e0` |
| 90 | `4fef:0014` | |
| 100 | | done |

## Cities (`random_map_cities`, `513d:0ce0`)

For each city the generator records which terrain types touch its 2×2
footprint (shore, forest, road, hills, marsh), then computes a **value 0–9**
(`513d:1104`):

```
owned city (a capital): 9
neutral: 1d4 − 1, +4 next to shore, +2 next to road,
         −1 next to forest, −2 next to marsh, clamped to 0..9
```

The value drives the city's economy and production (`513d:1171`,
`513d:161d`; not decoded). With "Cities can produce allies" on, 2d3 cities also
get a magical army type added to production (`random_add_production`,
`513d:1b3f`). `513d:1c1d` finishes up.
