# Random map generator — structure

How "A Random World" is built in `WARLORD2.EXE`. Every phase is identified,
and the flow, the parameters and each phase's rules are decoded. What is
summarised rather than read line by line: the exact shapes the ridge and
water routines draw (`4d71:0884`…`098c`, `4eb7:005d`) and the road and sign
phases. Addresses are Ghidra addresses.

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

| % | function | what it does |
|---|---|---|
| 0 | `auto_file_x_scn`, `4bed:036b`, `4c49:0000` | load the template scenario, initialise: the whole map starts as **plain** |
| 10 | `random_map_highlands`, `4d71:0000` | mountains and hills (below) |
| 20 | `4f5f:0000` | `+0x40` **erosion** walks and `+0x3e` **pass** walks (below) |
| 30 | `4eb7:0000` | water: `+0x3a` then `+0x38` runs, each from a random hill or mountain tile to a random water or shore tile — rivers and lakes down from the highlands — then cleanup |
| 40 | `random_map_forests`, `4e47:0000` | forest, until it covers `land tiles / 100 × param +0x3c` (the Forest slider) |
| 50 | `random_map_marshes`, `4f5f:06a0` | **1d3** marshes (below) |
| 60 | `513d:0000` | cities: per-city setup below |
| 70 | `513d:003a` | sites (writes "%03d\|%s is\|inhabited by monsters and\|full of treasure!\|" descriptions) |
| 80 | `5311:0000` | roads: builds the path grid as pseudo-player 14, using the map-generator cost table `DS:01e0` |
| 90 | `4fef:0014` | finish: city production, signs, save the files |
| 100 | | done |

The map the generator works on is a plain 112×156 byte grid of terrain type
ids at `RANDOM.DAT + 0x6120` (`docs/rules.md` › Movement lists the ids); it is
converted to real tiles at the end.

### Mountains and hills (`4d71:0000`)

1. Place `+0x34` **mountain** seeds (type 6) and `+0x36` **hill** seeds
   (type 5) on random plain tiles. Each seed becomes a node in a table at
   `+0x5dd6` (stride 14: x, y, kind, link count, up to 4 links).
2. Give each node links to its **nearest** other nodes: `1d2−1` for a hill,
   `1d4−1` for a mountain (`4d71:057b`).
3. Draw a ridge along every link (`4d71:06d2`) — mountain-to-mountain,
   mountain-to-hill and hill-to-hill use different routines, and a node with
   no links gets a blob of its own.
4. Clean up (`4d71:002e`, `4d71:033a`):
   - a plain tile whose four orthogonal neighbours are all mountain (or all
     hill) becomes that type,
   - a mountain tile next to a **shore** tile reverts to plain,
   - two mountains touching only diagonally get a third tile filled in on one
     side (1d10, evens each way), so ridges stay connected.

Both the water phase and the pass phase re-run this cleanup.

### Marshes (`4fc9:010a`)

Each marsh starts on a random plain tile at least 5 tiles from the map edge,
preferring one **near shore** — it retries up to 4 times for a plain tile with
shore among the 8 neighbours of its 2×2 footprint, then settles for any plain
tile. That tile becomes marsh (type 8), and five seeds (the tile and its four
diagonal neighbours) each spread `1d5+3` more: for every spread, step 1–3
tiles along one of the 8 neighbour offsets from the seed, and turn the tile
marsh if it is still plain.

### Erosion and passes (`4f5f:0044`)

Each run picks two random points and walks from one to the other along the 8
neighbour offsets at `+0xbc`:

- **kind 0** (erosion): every **mountain** tile on the walk, plus its two
  flanking tiles and their extensions, becomes **hill**.
- **kind 1** (pass): every mountain or hill tile on the walk, and the same
  flanking tiles, becomes **plain** — this is what cuts passes through a
  range.

## Cities (`random_map_cities`, `513d:0ce0`)

For each city the generator records which terrain types touch its 2×2
footprint (shore, forest, road, hills, marsh), then computes a **value 0–9**
(`513d:1104`):

```
owned city (a capital): 9
neutral: 1d4 − 1, +4 next to shore, +2 next to road,
         −1 next to forest, −2 next to marsh, clamped to 0..9
```

### Income and description (`513d:1171`)

```
income = value * 2 + 1d8 + 14
```

Everything else in that routine is flavour text: the city's name and its
`.CTY` description are assembled from word tables inside `RANDOM.DAT` (ten
16-byte entries per table), with the adjective table chosen by two rolls of
`1d3 + value − 2` clamped to 0–9, so richer cities get grander descriptions.

### Production (`513d:161d`)

```
slots = clamp(value / 2 + 1d4 − 1 + (touches forest) + (touches hills), 0, 4)
```

The candidates are a 29-entry table of 16-byte records in `RANDOM.DAT` at
`+0x1d82` — army type id, a 1d10 chance, a region class and a terrain class —
walked in order until the city has its slots. A type is taken when its chance
roll passes **and** it is either generic (class 7), matches a flag the city
has (forest, hills or shore), or matches the **region** class: the map is cut
into 8 regions, `(x·4)/112 + ((y·2)/156)·4`, each with a preferred class in
the table at `+0x1d72`. A shore type is refused unless the city touches
shore. Each accepted slot copies the type's strength, time, upkeep and
movement from `ARMYTYPE.DAT` into the city record.

Unless "Cities can produce allies" is on, army types flagged as allies
(`DS:0668 + 6·type`) are skipped here; instead 2d3 cities get one added
afterwards (`random_add_production`, `513d:1b3f`). `513d:1c1d` finishes up.
