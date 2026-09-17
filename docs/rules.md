# Game rules

Everything here is read out of `WARLORD2.EXE` unless a section says otherwise;
the manual (*Warlords II Deluxe*, Appendix C and chapter 8) was the starting
point and agrees with the code except where noted. Function names and
addresses are Ghidra addresses in the project built by `tools/exe.py` and
`tools/ghidra/` — see `docs/re/README.md`. Offsets like `.SCN 0x110` are both
file offsets and live memory addresses, because the scenario file *is* the
game state (`docs/formats/scenario.md`).

| section | |
|---|---|
| [Derived values: ownership and city defence](#derived-values-ownership-and-city-defence) | ownership, city defence |
| [Game setup](#game-setup) | sides, options, difficulty |
| [Start of a side's turn](#start-of-a-sides-turn) | income, upkeep, production, movement reset |
| [Production](#production) | what a city builds and its stats |
| [Starting garrisons and neutral production](#starting-garrisons-and-neutral-production) | first army per city, neutral cities |
| [Movement](#movement) | terrain costs, stack modes, bonuses |
| [Moving a stack](#moving-a-stack) | stacking limit, costs, what stops a move |
| [Combat](#combat) | bonuses, terrain classes, resolution |
| [Capturing a city](#capturing-a-city) | loot, occupy, pillage, sack, raze |
| [Heroes](#heroes) | offers, allies, levels, experience, death |
| [Ruins, temples and sages](#ruins-temples-and-sages) | contents, guardians, rewards |
| [Quests](#quests) | types, targets, completion, rewards |
| [Diplomacy (computer attitudes not decoded)](#diplomacy-computer-attitudes-not-decoded) | states, proposals, ratings |
| [End of the game](#end-of-the-game) | victory, elimination, surrender |
| [Still unknown](#still-unknown) |  |

## Derived values: ownership and city defence

Neither is stored in the scenario file; both are computed at game start.

**City ownership at scenario start.** Each side owns exactly its capital; every
other city is neutral. See `docs/formats/scenario.md`.

**City defence.**

```
tower            -> 1
special location -> 2
city             -> 1 if it produces fewer than 3 army types
                    2 if it produces 3 or 4
halved when the city being attacked is neutral
```

Verified against the running game: Mirea (4 types) → 2, Axbridge (4) → 2,
Largash (3) → 2, all displaying `Defence: 2`. The count is taken **after** the
game removes Navy from every city at start (see Production). Across Erythea's
80 cities that gives 34 at defence 1 and 46 at defence 2. It also explains the
help file's
`- Pillage -` → "Reduce defence for gold": pillaging removes production types.

## Game setup

### Sides

Three `u16` arrays with one entry per side, in the `.SCN` header:

| offset | meaning |
|---|---|
| `0x0137` | side is in play |
| `0x00d0` | controller: **0 = human, 1 = computer** |
| `0x00c0` | computer level: **0 Knight, 1 Lord, 2 Warlord**; **3 = side not playing** |
| `0x00f0` | **Enhanced**: this side's newly produced armies get +2 strength (max 9) |
| `0x0147` | **Observe**: show this computer side's turn as it plays |

The side setup screen's four columns are Name, Human, Enhanced and Observe
(`64d2:0137`); human sides show "Human" in place of a level.

When a game starts (`start_game_from_setup`, Ghidra `7bab:0cfe`), a side set to
level 3 is removed and **its capital becomes neutral**. Human sides are given
level 2.

### Options

The options dialog stores its ten settings at these `.SCN` offsets (not in
menu order):

| option (`STRING.DAT` group 4) | offset | option | offset |
|---|---|---|---|
| Neutral Cities | `0x11a` | Intense Combat | `0x126` |
| Diplomacy | `0x11c` | Quick Start | `0x128` |
| Quests | `0x11e` | View Enemies | `0x12a` |
| Random Turns | `0x122` | Military Advisor | `0x12c` |
| Hidden Map | `0x124` | View Production | `0x132` |

`0x12e` is the separate tutorial flag, and `0x130` is cleared on start.

### Difficulty rating (`difficulty_rating`, `7bab:0bab`)

```
options = min(20, 4·NeutralCities + 4·Diplomacy + 3·Quests + 4·HiddenMap
                  − ViewEnemies + ViewProduction)
ai      = 80 × Σ(level+1) / (3 × computer sides)      (80 if there are none, or if the result is ≥ 78)
rating  = options + ai        shown as "Difficulty Rating %d%%"
```

Only computer sides that are playing count. Neutral Cities isn't a plain
on/off: it has several strengths (`STRING.DAT` group 5: Average, Strong,
Active).

## Start of a side's turn

`start_of_turn` (Ghidra `8cc6:0000`) runs these steps in this order:

1. Reports and diplomacy messages.
2. **A side with no cities is eliminated** (`8cc6:0952`); nothing below runs.
3. **Hero offer** (see Heroes), then **hero promotions**.
4. **Gold:** `gold += income − upkeep`, never below 0 (`apply_income`,
   `8cc6:0827`).
   - **income** = sum of each owned city's income (city record `+42`)
     + number of cities × the side's "gold per city" items (item type 7,
     carried by any of its heroes)
   - **upkeep** = sum of each army's upkeep byte (`+0xb`, set to half the
     slot's cost when the army is built). Armies in transit don't pay; an
     army at sea pays at least 4.
5. **Production** (`city_production_turn`, `6f8c:0000`). Each producing city's
   countdown (city `+0x2d`) drops by 1. At 0 the army is built, but **only if
   the side has more than 0 gold** after step 4. The build itself costs
   nothing up front; the slot's cost is only its upkeep. A vectored army
   leaves in transit and arrives **two turns later**. If it can't be placed
   (destination full at 8 armies, or no longer the side's), it's sent back
   home, taking another two turns. If it was already heading home, it's
   **disbanded**. Vectoring itself is free and has no range limit
   (`vector_city_to`, `623c:0f10`); it only sticks while the city is building.
   A destination of `-2` means the side's standard rather than a city.
6. **Movement reset** (`reset_movement`, `8cc6:05fb`):
   - new moves = army's maximum + **min(unused moves, 2)**
   - an army **at sea** gets **20 + min(unused, 2)** instead
   - a hero carrying a **double movement** item (type 6) adds each army's
     maximum again for every army on its tile
   - remaining moves are capped at 99
7. Quest checks (`4976:1ded`), then the side plays.

### Item types

| type | effect | used by shipped scenarios |
|---|---|---|
| 1 | +n battle (hero strength) | yes |
| 2 | +n command (stack bonus) | yes |
| 5 | allows flight | via `.ITM` |
| 6 | doubles movement | via `.ITM` |
| 7 | +n gold per city | via `.ITM` |
| 8 | standard: +1 command | yes |

Types 5, 6 and 7 don't appear in the `.SCN` files but **are** handed out in
play: the item pool comes from the scenario's `.ITM` file
(`docs/formats/itm.md`), which has all of them.

## Production

At game start, `setup_capitals` (Ghidra `79fa:07ca`) gives each side its
capital; every other city is neutral (owner 15). With **Quick Start** on
(`.SCN` `0x128`), all neutral cities are then dealt out round-robin:
each side in turn takes the neutral city nearest its last city (with half
chance measured from its capital instead) until none are left. Then
`setup_city_production` (`79fa:0a75`) rebuilds every city's production. **The
per-slot values stored in the `.SCN` file are ignored.**

1. **Navy (type 5) is removed** from every city's production list. The list
   closes up, leaving an empty slot at the end.
2. Each slot's stats are copied from `ARMYTYPE.DAT`: strength `+22`, time
   `+24`, cost `+26`, **move `+28`**.
3. Slots are sorted by the type's purchase price (`+30`), cheapest first.
4. Each slot's stats then get a random nudge:

| stat | chance | effect |
|---|---|---|
| strength | 10% | 60%: +1 (max 9), else −1 (min 1) |
| move | 20% | 1d100 < 10: +4 · < 60: +2 · < 95: −2 · else −4 (never below 2), **then at least 6** |
| cost | 10% | 60%: −¼ of cost, else +¼ |
| time | 10% | 60%: −1 (min 1), else +1 |

So the same army type can have different stats in different cities, which is
why the production screen's numbers differ per city.

**City defence** (`city_compute_defence`, `7087:0930`) is stored in city record
`+20`: **1 with fewer than 3 production types, otherwise 2**. It's recomputed
at setup and whenever a type is bought.

**A new army** (`city_produce_army`, `6f8c:0dd7`) takes its move and strength
from the city slot. Upkeep is half the slot's cost. Strength gets **+2 (max 9)**
when the owning side's **Enhanced** flag (`.SCN` `0x00f0 + 2·side`) is set.

**Buying a type** (`buy_production_type`, `7087:1299` / `7087:0ee3`) copies
the unmodified `ARMYTYPE.DAT` stats into the slot, with no random nudge, and
subtracts the purchase price (`+30`) from the side's gold (`.SCN`
`0x185 + 20·side`).

## Starting garrisons and neutral production

### One army per city (`setup_garrisons`, Ghidra `6bd8:00cc`)

Every city gets **exactly one** starting army:

| city | garrison level |
|---|---|
| a side's city (its capital) | 3 |
| neutral, *Neutral Cities* option = 0 | none; a placeholder **Scouts** army of strength 1 is placed instead |
| neutral, option *k* > 0 | `min(3, 1d4 + k − 2)` |

The level picks a **purpose** (`DS:0d60` = `1 6 2 3` for levels 0–3). The city
builds its best production type for that purpose immediately
(`best_production_for`, `623c:103d`, then `produce_garrison`, `6bd8:0381`). The
new army has 0 moves left.

### Choosing the best type for a purpose

Used for garrisons and, most likely, by the computer players' production
decisions. For each of the city's production slots (with that city's
randomised stats):

```
str   = min(9, strength + 2 if the side's bonus toggle is on) + 2 if the type has Siege
time  = time + 1 if str < 3 and purpose ≠ 6, capped at 10
score = (10 − time)·Wtime + str·Wstr + (move·Wmove)/2
```

| purpose | Wtime | Wstr | Wmove | note |
|---|---|---|---|---|
| 1 | 10 | 4 | 1 | quick and cheap |
| 2 | 10 | 10 | 1 | balanced |
| 3 | 5 | 10 | 1 | strongest |
| 4 | 5 | 10 | 1 | flying types only |
| 5 | 5 | 10 | 1 | |
| 6 | 10 | 1 | 10 | fastest |

The highest score wins. Slots are scanned from last to first with a strict
`>`, so ties go to the later slot.

### Neutral production during play — dead code

`neutral_production_turn` (`6bd8:025a`) runs every turn and would let a neutral
city keep building (up to 4 armies) when *Neutral Cities* is 2 or higher. It
only acts on cities whose flag at city `+0x30` is set — and **nothing in the
game ever sets that flag**: the only writes are the zeroing in the city-init
routine (`623c:0000`), and the `.SCN` byte lies in the record's zero padding.

So neutral cities never produce during play, and the *Neutral Cities* option
only affects how strong their starting garrison is. A faithful remake should
leave this out.

## Movement

The pathfinder lives in resident segment `0555` (Ghidra `1555`). It builds a
112×156 grid with one byte per tile, then runs a wavefront search from the
destination. Each grid byte holds a **cost in the low 3 bits** plus flags:
`0x08` water, `0x10` crossing (bridge, city, some tiles), `0x20` hills,
`0x40` forest.

### Terrain costs (`DS:1274`, static)

| Road | Bridge | Water | Shore | Forest | Hills | Mountains | Plain | Marsh | Tower | City |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 1 | 1 | 2 | 4 | 6 | **impassable** | 2 | 5 | 2 | 1 |

(Terrain type 11 costs 2.) **Any tile with a road on it costs 1**, whatever
its terrain.

A second table at `DS:01e0` (Water 3, Shore 3, Mountains 7, …) is only used
when the random map generator routes with pseudo-player 14, probably to lay
roads.

### How a stack moves (`stack_movement_mode`, Ghidra `1c8c:0529`)

The stack's mode comes from its armies (`ARMYTYPE` fields via the per-type
flag table built by `build_army_move_flags`, Ghidra `7715:0000`):

| mode | when | movement |
|---|---|---|
| **at sea** | any army is already on water | land rules, sea flag set |
| **boat** | any army is a boat (`+60`) | water, shore, bridges and cities only; pays the table cost |
| **flying** | every army flies (`+54`), *or* every non-hero flies and at least one army does, *or* a hero carries a flight item | **2 per tile** (water and mountains included); 1 where the tile costs 1 |
| **land** | otherwise | table costs; mountains impassable; can only cross between land and water at a crossing tile (`0x10`), and entering water costs an extra 10 (20 if the destination is water) |

A land stack gets a **move bonus** if **any** army in it has one. Forest or
hills tiles then cost **2** instead of 4 or 6:

- **`ARMYTYPE +56` = woods move bonus**: Scouts, Orcish Mob, Archers
- **`ARMYTYPE +58` = hills move bonus**: Scouts, Orcish Mob, Dwarves, Giants.
  It doesn't cover mountains, which stay impassable on land.

The stack's movement points are the **lowest** of its armies' remaining moves
(army record `+7`). The manual's "one army lacking MPs stops the group" follows
from that. The 2-MP carry-over is confirmed (see Start of a side's turn) and so is the
8-army stack limit (see Moving a stack).

A path is stored as up to 200 compass directions (0 = north, clockwise).

## Moving a stack

`move_stack_to` (Ghidra `1a8b:0001`) drives both the player's and the
computer's moves. It repeatedly pathfinds to the destination
(`docs/rules.md` › Movement) and walks the path (`walk_path`, `1a8b:07f9`)
until something stops it.

Walking the path, step by step:

- **A city not owned by the mover** stops the walk and starts an attack on it
  (`attack_tile`). If the city is taken, the walk continues into it.
- **A tile occupied by another side** stops the walk and attacks, but only if
  the two sides are **not at peace** (diplomacy state ≠ 0).
- **The 8-army limit:** the stack may only *stop* on a tile where
  `armies already there + stack size ≤ 8`. Steps that would breach it are
  passed over — the walk keeps going and lands on the first later step that
  fits.
- **Cost:** when the walk ends, the cumulative cost of the steps taken is
  subtracted from **every army in the stack**, floored at 0. A multi-army
  stack spends from its shared pool (the minimum of its armies' moves).
- **Stopping:** the walk ends when fewer than **2** movement points remain,
  or when the next step costs more than what's left.

`armies_at_tile` (`1b62:007d`) lists up to 8 armies on a tile and returns the
one highest in its owner's **fight order**, which is the army the map shows
for that tile.

## Combat

`combat_setup` (Ghidra `6a89:008b`) builds both lines, `combat_terrain_class`
(`6a89:0000`) classifies the tile, and `combat_resolve` (`67cc:08a6`) fights.
Where the code and the Deluxe manual disagree the code wins, and the
difference is called out.

### The two lines

- **Attackers:** the selected stack.
- **Defenders:** every army not owned by the attacker on the target tile, or
  on any of the 2×2 tiles when the target is a city.
- Each line is sorted by its owner's **fight order**: a per-player table of
  29 values (one per army type), lowest first. Neutral defenders use row 8.
  Ties keep stack order.
- On water, shore or mountain tiles, a side that includes a Hero sends one
  qualifying army (by a per-type flag, probably *can fly*) to the back of its
  line. That keeps the hero's carrier alive longest.
- Every army starts with **2 hit points**.

### Terrain class of the battle tile

| class | tiles |
|---|---|
| **city** | city tiles, terrain type 11, any tile with the tower flag |
| **woods** | Forest |
| **hills** | Hills, Mountains |
| **open** | everything else: Road, Bridge, Water, Shore, Plain, Marsh, the Tower terrain type |

Army bonus fields are indexed in the order **city, open, woods, hills**
(`ARMYTYPE.DAT +32..+38` individual, `+40..+46` stack).

### Side modifiers

Computed once per side. "Any" means any army in that side's stack:

```
hero_bonus(side) = HERO_TABLE[min(9, strongest hero's strength + its battle items)]
                 + sum over heroes of command items
                   HERO_TABLE = 0 0 0 0 1 1 1 2 2 3   (index = strength 0..9)
stack_bonus(side) = max over stack of ARMYTYPE +40..+46 [class]
subtract(side)    = min over stack of ARMYTYPE +50       (0 or negative)

fortify = only if class == city and the attacker has no Siege:
            1                 tile has the tower flag
            2                 terrain type 11
            city defence      otherwise (city record +0x14)
          halved (integer) if the tile's owner is neutral

attack_mod  = min(CAP, [hero_bonus(atk)  unless any defender has Negate Hero]
                     + [stack_bonus(atk) unless any defender has Negate Non-Hero])
            + subtract(def)
defend_mod  = min(CAP, [hero_bonus(def)  unless any attacker has Negate Hero]
                     + [stack_bonus(def) unless any attacker has Negate Non-Hero]
                     + fortify)
            + subtract(atk)
```

Abilities are `ARMYTYPE +52`: 1 = Siege ("Cancel city bonus"), 2 = Negate Hero,
3 = Negate Non-Hero.

- **Battle items** are item type 1 ("+n in battle!"). They raise the hero's
  strength for the table lookup *and* its own fighting strength.
- **Command items** are item type 2 ("+n to command!"). Every item of type 8
  (the eight **standards**) also adds +1.
- **CAP** is stored in the scenario (`.SCN` offset `0x112`). It's **5** in
  all six shipped scenarios, matching the manual.

### Each army's strength

```
strength = army's current strength (army record +8, includes medals)
         + side modifier (attack_mod or defend_mod)
         + battle items           (heroes only)
         + ARMYTYPE +32..+38 [class]   (individual terrain bonus, never negated)
capped at 15; combat_resolve then treats anything below 1 as 1
```

**Boats:** an army flagged as at sea (army flags `0x1000`) fighting on a Water
or Shore tile gets strength **exactly 4**, replacing the formula. The manual's
"4 or natural strength, whichever is lower" is **not** what the code does.
`ARMYTYPE +60` isn't read here.

`ARMYTYPE +48` (magical flag) and `+54` (flying) aren't read by the strength
calculation.

Resolution, army against army — **verified in `WARLORD2.EXE`**
(`combat_resolve`, Ghidra `67cc:08a6`; see `docs/re/dice_callers.md`):

- both roll a **d20** (**d24** if the *Intense Combat* option is on)
- attacker hits if it rolls ≤ its modified strength **and** the defender rolls
  above his
- defender hits on the mirror condition; if neither, re-roll
- after 10000 throws with no hit, the defender wins by default

What the code adds to the manual:

- strengths reaching the loop are already fully modified; a strength below 1
  is treated as **1**
- every combatant starts with hits = 1 and dies when it drops below 0, so it's
  **2 hit points for every army type**, heroes included
- the front attacker fights the front defender. When one dies, the next in
  that line steps in against the damaged survivor. Damage isn't healed
  between fights.
- the battle ends when one line is empty; the attacker wins only if **every**
  defender is dead. A battle with nobody on one side counts as an attacker win.
- the 10000 counter is per fight. Past it, every throw is forced to a
  defender hit.
- **tutorial exception:** in the tutorial scenario (tutorial flag,
  `.SCN`/`2c04` offset `0x12e`, is 1 only in `TUTORIA.SCN`) defender hits
  are **ignored** when:
  - the attacker's current army is a Hero,
  - the attacking player is flagged `0` at `2c04:00d0 + 2·player`
    (probably *human*), and
  - the defender is neutral.

  So a tutorial hero can't die attacking neutral cities.
- every fight result is logged in order (`0` = a defender died, `1` = an
  attacker died), and the battle animation plays it back

Because the die is d20 and strength caps at 15, a group of weak armies can
always beat one strong army.

**Military Advisor** (`military_advisor`, Ghidra `67cc:1f19`, verified). Only
available when the *Military Advisor* option (`.SCN` `0x12c`) is on. It runs
**19** simulated battles through the real combat code (the manual says 20),
counts the wins, and shows `STRING.DAT` group 126 entry **wins ÷ 2**:

| wins | verdict |
|---|---|
| 0–1 | complete and utter suicide! |
| 2–3 | sheerest folly! Thou shouldst not attack! |
| 4–5 | a foolish decision! |
| 6–7 | a brave choice! I leave it to thee! |
| 8–9 | difficult but not impossible to win! |
| 10–11 | very evenly matched! |
| 12–13 | a hard-fought victory! But we shall win! |
| 14–15 | a comfortable victory! |
| 16–17 | an easy victory! We cannot lose! |
| 18–19 | as simple as butchering sleeping cattle! |

The greeting (group 124) and lead-in (group 125) are picked at random.

## Capturing a city

### Loot (automatic, `67cc:0a6b`)

Winning a city from another side (not a neutral one):

```
loot = (loser's gold / loser's city count) / 2      (all its gold / 2 if it had 1 city)
attacker gold += loot
loser gold    -= 2 * loot                            (clamped to 0 at the loser's next income step)
```

The city's previous owner is recorded in city `+0x2f` (set to neutral if the
side retakes its own city). The production countdown is cleared.

### What to do with it (`63fa:0000` dialog)

Production types are sorted by purchase price, cheapest first (see
Production), and "value" means **half a type's purchase price**
(`ARMYTYPE +30`).

| choice | needs | effect | gold | atrocity |
|---|---|---|---|---|
| **Occupy** | – | nothing | – | – |
| **Pillage** | ≥ 1 type | remove the **most expensive** type | its value | +1d5 |
| **Sack** | ≥ 2 types | remove **all but the cheapest** type | sum of their values | +1d10+5 |
| **Raze** | – | city becomes ruins: owner neutral, city `+0x2f` = 15, map tiles replaced, vectoring to it cancelled | – | +1d15+10 |

Pillage and sack recompute the city's defence (fewer types can drop it from
2 to 1). The **atrocity** score is a `u16` per side at `.SCN` `0x10e3 +
2·side`; see Diplomacy.

## Heroes

### When a hero offers to join (`hero_offer_check`, Ghidra `7563:0000`)

Checked once per turn for the current side:

- **Turn 1:** a hero always appears, free, at the side's capital, carrying
  the side's **standard** (item record = side number).
- **Later turns**, all of these must hold:
  - fewer than **40** heroes in the whole game
  - the side has fewer than **5** heroes (**6** once it owns 40+ cities)
  - the price is affordable: **1d400+300** gold for a side with no hero on
    the map, **1d600+1000** otherwise
  - a **20%** roll (`1d30 < 7`)

  The hero then appears in a random city the side owns. Computer players
  decide with a `1d100` check in `5db9:0919` (not yet read).

### What a new hero is (`hero_recruit`, `7563:031b`)

A hero is an army of type 28 with **strength 5** and **14 movement**. Its name
is a random entry from `TERRAIN<n>\HERONAM<side>.DAT` (`load_hero_name`); each
entry also carries a flag, probably gender. Hero state lives in the `.SCN`
image: name `0x223 + 20·hero` (40 slots), in-use flags `0x543`, experience
byte `0x5e3`.

**Allies:** every hero hired after turn 1 arrives with **1–3 allies** (1d100:
<70 → 1, <95 → 2, else 3) of one random **magical** type (`ARMYTYPE +48`,
Dragons if none qualify). The price is paid first. `hero_brings_allies`
(`7563:01fc`) does have a "do allies come?" flag, but it is **hard-coded to 1**
and the test is dead code, so allies always arrive. Worth watching for in a
real game.

### Death (`hero_drop_items`, Ghidra `67cc:16cd`)

When a hero dies (in battle, or to a ruin guardian), its slot is freed and
**every item it carried is dropped on the tile where it died** (item status 1,
at that x, y). If that tile is **Water**, the items are **lost for good**
(status 0, with a splash sound). If the side's own standard was among them,
cities vectoring their production to the standard stop doing so.

### Levels (`hero_check_promotions`, Ghidra `7563:0579`)

The hero's experience byte (`.SCN` `0x5e3 + hero`) holds the **level in its
low 2 bits** and experience in the upper 6 (max 60). A side's heroes are
checked for promotion one step at a time:

| promotion | needs experience |
|---|---|
| Hero → **Cavalier** | ≥ 15 |
| Cavalier → **Champion** | ≥ 30 |
| Champion → **Paladin** | ≥ 60 |

Each promotion gives **+1 strength (max 9)** and **+2 maximum movement**
(`hero_promote`, `7563:0672`). The announcement uses `STRING.DAT` group
101/102/103, with the heroine wording when the hero's flag at `.SCN`
`0x593 + 2·hero` is set.

Experience comes from (all call sites of `hero_add_experience`):

| source | experience |
|---|---|
| surviving a battle as **attacker** | +2 if the target was a city, else +1 |
| surviving a battle as **defender** | +1, **but see the bug below** |
| ruin search | +3 |
| sage visit | +3 |
| temple blessing | +1 |
| completed quest | +10 |

**Original bug:** after a battle the game only credits a surviving defending
hero if `combat_atk_type[i]` is a Hero, i.e. it checks the **attacker's**
army at the same position in the line (`67cc:0c8e`:
`cmp [si+424c], 1Ch`). In practice a defending hero gains experience only when
the attacker's line has a hero at the same index. A faithful remake should
copy this; a "fixed" remake should check `combat_def_type`.

## Ruins, temples and sages

### What each site holds (rolled at game start)

Scenario files only mark each site as **temple** (content 1) or **ruin**
(content 2). `setup_random_sites` (Ghidra `66d4:0000`) fills in the rest.

**Rich ruins.** First, `mark_rich_sites` (`66d4:091e`) picks
`sites × 3 / 10` non-temple sites at random and marks them **rich** (site
`+0x1b`); every site's "revealed to" mask (`+0x1d`) starts full, and with the
*Quests* option on the rich ones are cleared so they can be pointed out — by
a quest, or by a sage (below).

**Magic items.** The pool comes from the scenario's `.ITM` text file
(`docs/formats/itm.md`), not from the `.SCN`: item records 8–21 are refilled
with 14 random entries from it. The first `sites × 2 / 10` of those get
**reserved** items (flight, double movement, standards, command items worth
2+ — `item_reserved`, `66d4:08f4`), the rest ordinary ones.

The number of items hidden in ruins is `sites/3 + 1d5 − 3`, at most 14.
A reserved item can only be hidden in a **rich** ruin, an ordinary item only
in an ordinary one. Items in the band that isn't placed (indices
`8 + min(2d3+1, sites/5)` up to `8 + sites/5`) stay out of play, probably for
quest rewards.

**Every other ruin** rolls its content from a small table (3 = sage, 4 =
gold, 5 = allies):

| ruin | sage | gold | allies |
|---|---|---|---|
| rich | – | 1/3 | 2/3 |
| no capital within 15 tiles | 2/5 | 2/5 | 1/5 |
| a capital within 15 tiles | 1/3 | 1/3 | 1/3 |

**Guardians:** a monster from the scenario's monster table (1d9; temples
have none). Ally ruins store the ally *type* instead:

| ruin | ally type (equal odds) |
|---|---|
| rich | Dragons, Wizards, Devils, Archons |
| far from capitals | Ghosts, Giant Worms, Demons |
| near a capital | Elementals, Giant Worms |

### Searching (`site_search`, Ghidra `6536:0000`)

A searched site is marked explored (tile flag `0x40`) and can't be searched
again. Only a stack **with a hero** can search a ruin. Any stack can visit a
temple.

- **Temple:** each army in the stack gets **+1 strength (max 9)**, once per
  temple per army. Only the first four temples in the site list can bless,
  one flag bit each. A blessed hero also gains 1 experience. A human player
  whose stack includes a hero gets the temple dialog instead (blessing or
  quest).
- **Sage:** no fight. The hero gains 3 experience and gets the sage dialog
  (below).
- **Ruin:** the hero gains 3 experience. If there's a guardian, it fights
  once:

  ```
  hero survives if 1d100 <= 90 + 5 * (hero strength + battle items − monster strength)
                               + 3 * (armies on the hero's tile, hero included)
  ```

  Monster strengths are a `u16` table at `.SCN` `0x1007` (Erythea: Troll 5,
  Giant 7, Wolf 4, Goblin 3, Dragon/Demon/Devil/Wizard 8, Ghost 7). A slain
  hero is removed and drops its items (see Heroes › Death). With no
  guardian, or after a win, the ruin gives up:
  - **item:** the hero finds it
  - **gold:** **3d500+500** (**3d1000+1000** if rich)
  - **allies:** **1d2** armies of the stored type (**3–4** if rich) join on
    the spot, placed within ±1 tile if the site is full

Hero experience is capped at 60 (hero record byte `.SCN` `0x5e3 + hero`,
upper 6 bits).

### The sage (`sage_visit`, Ghidra `6536:0aa0`)

The sage offers three things, and the visit ends after one choice:

**Knowledge** (`sage_offer_knowledge`, `6536:0e85`), offered only if the
list below is non-empty. `6536:1610` builds it from every site that is
**rich**, not yet revealed to this side (reveal mask `+0x1d`) and **within 35
tiles** of the sage:

- one entry per rich **item** ruin, named after the item,
- one entry "A huge pile of treasure" if any rich **gold** ruin qualifies,
- one entry "Powerful allies" if any rich **allies** ruin qualifies.

Because `mark_rich_sites` fills every site's reveal mask with `0xff` and
only clears it for rich sites **when *Quests* is on**, this option is dead
with *Quests* off: the list is always empty and the button is greyed out.

Picking an entry names the nearest qualifying site ("*The Firesword* can be
found at *<site>*!"), reveals it on the map and sets the side's bit in that
site's reveal mask, so each side can be told about a site only once.

**A gem** (`sage_gem`, `6536:0b1a`): **3d500+500 gold**, no conditions.

**A map** (`sage_offer_map`, `6536:0c50`), offered only with *Hidden Map*
on: the player points at the map and `6536:0cd6` uncovers a rectangle whose
top-left corner is `(x − 1d5 − 8, y − 1d5 − 8)` and whose size is
`(1d10+15) × (1d10+15)`, clipped to the map.

**The computer** (`5e97:0080`) takes the map if *Hidden Map* is on and it can
find a target, otherwise the gem. Its target is the neutral city with the
most other neutral cities within 20 tiles (at least 4 of them), ties going to
the one furthest from its own cities; it then reveals the area around that
city, jittered by `1d11−6` on each axis. It never asks for knowledge.


## Quests

Quest state is 12 bytes per side at `.SCN` `0x1103`: active flag, type, hero
(army index), target.

### Quest types (`quest_assign`, Ghidra `4976:0d7a`)

From a temple, the type comes from the table `DS:00a0` =
`0 1 2 3 4 5 6 4 5 6` (1d10). The quest record's target field means
different things per type; `+8`/`+10` are "required" and "done so far" for the
counting quests.

| type | quest (`STRING.DAT`) | target | chance |
|---|---|---|---|
| 0 | slay the enemy hero (21) | an army index | 10% |
| 1 | retrieve an item (22) | an item index | 10% |
| 2 | slay a unit of an enemy army type (23) | an army type | 10% |
| 3 | slaughter *n* armies of a side (24) | a side | 10% |
| 4 | force a city into submission and **occupy** it (25) | a city | 20% |
| 5 | conquer a city and **raze** it (26) | a city | 20% |
| 6 | sack and pillage *n* gold (27) | – | 20% |

The second entry point (used without the temple dialog) picks type 5 (1 in 5)
or 4, then falls back to 3, then 6.

**Targets** (read from raw disassembly; Ghidra mis-disassembles this switch).
If a type finds no valid target, the type is rolled again.

| type | how the target is picked |
|---|---|
| 0 | a random hero belonging to any other side |
| 1 | a random non-reserved item lying in a ruin within 50 tiles of the hero (30 from the second entry point), by both x and y distance. That ruin is marked as revealed to the side. The counting loop wants items in ruins, but the picking loop accepts any item that's in play, so the picked item can differ from the counted set. |
| 2 | up to 5 tries: a random magical army type (`ARMYTYPE +48`) that some other side has on the map |
| 3 | a random other side that's still alive (up to 200 tries); **n = 1d12+10** armies |
| 4 | a random city not owned by the side, within map distance 60 (40 from the second entry point); after 100 failed tries any such city will do |
| 5 | as type 4, always within 60 |
| 6 | **n = 3d300+500** gold |

Types 3, 4 and 5 are skipped once the game has been won (`.SCN` `0x15b`, see
End of the game).

### Completion and failure (`quest_check`, `4976:1ded`)

The game calls the checker with an event code. "With the hero" means the quest
hero was in the stack that did it.

| event | quest | result |
|---|---|---|
| battle won | 0 slay hero | **done** if the target hero died, with the hero |
| battle won | 2 slay unit | **done** if a dead defender was of the target type, with the hero |
| battle won | 3 slaughter | dead defenders of the target side add to the count, with the hero; **done** at *n* |
| item picked up | 1 retrieve | **done** if the quest hero now carries the item; **the item is taken away** |
| pillage / sack | 6 pillage gold | the gold adds to the count, with the hero; **done** at *n* |
| pillage / sack | 4 occupy, 5 raze | invalid if it's the target city ("was not to pillage", group 36) |
| occupy | 4 occupy | **done** with the hero; otherwise invalid (group 34) |
| occupy | 5 raze | invalid ("quest was to raze", group 35) |
| raze | 5 raze | **done** if the hero's stack razed the target; otherwise invalid (group 37) |
| raze | 4 occupy | invalid ("quest was to keep", group 38) |
| start of turn | any | cancelled if the hero is dead or changed hands (group 32) |
| start of turn | 4 occupy, 5 raze | impossible if the city was razed (33); invalid if the side now owns it without the hero (42) |
| start of turn | 0 slay hero | invalid if the target is no longer a hero (41) |
| start of turn | 3 slaughter | invalid if the target side is gone (39) |
| start of turn | 1 retrieve | invalid if the item is out of play (40) |

On completion the quest is cleared, a reward is chosen and given (below), and
the hero gains 10 experience.

### Reward (`quest_choose_reward`, `4976:1909`)

Checked in order:

1. The side owns **fewer than 10 cities** and it's past **turn 15** →
   **1d3+5 allies** (6–8).
2. The side has **less than 100 gold** → **2d1000+1000 gold**.
3. An unclaimed magic item exists (the side's own standard also counts) →
   1 in 3: **that item**, otherwise go to step 5.
4. No such item, but an unexplored rich site the side hasn't been shown →
   2 in 3: **the priests reveal the nearest one**, otherwise step 5.
5. 1 in 2: **1d3+2 allies** (3–5), otherwise **2d1000+1000 gold**.

Completing a quest also gives the hero **+10 experience**.

## Diplomacy

For every ordered pair of sides there's a byte at `.SCN`
`0x153b + 8·side + other`:

- **bits 0–1:** current state, **0 = peace, 1 = intermediate, 2 = war**
- **bits 2–3:** this side's **proposal** (the state it wants)

At game start (`diplomacy_init`, Ghidra `484e:11bd`) every pair is at **war**
if the *Diplomacy* option is off, and at **peace** if it's on.

**Attacking** a side you're at peace with (state 0) is refused with
`STRING.DAT` group 140 ("Milord! Thou art attacking without first having
declared war"); state 1 is refused in some cases too (`attack_tile`).

**Proposals are applied at the start of the proposing side's turn**
(`diplomacy_apply`, `484e:0db3`):

- **Escalating** (proposal more hostile than the current state) takes effect
  **at once, for both sides**. The other side's proposal is raised to match,
  and a move to war announces "War declared with %s!".
- **De-escalating** only takes effect when the **other side's proposal is no
  more hostile** than this one; then both move to it and "Peace negotiated
  with %s!" is shown.

**Diplomatic score** (the same `u16` per side at `.SCN` `0x10e3` that
pillage/sack/raze raise; see Capturing a city). `484e:1063` also adds to it
for each side whose proposal is more peaceful than both the current state and
the other side's proposal:

| proposal → from current | added |
|---|---|
| 0 from 1, or 1 from 2 | 1d2+1 |
| 0 from 2 | 1d10+10 |

**Diplomatic Rating** (`diplomatic_rating`, Ghidra `484e:0aed`, verified).
Ratings are *relative*: the sides in play are sorted by diplomatic score,
**lowest first** (ties keep side order), and each rank gets a title from
`STRING.DAT` group 106 (0 Statesman, 1 Diplomat, 2 Pragmatist, 3 Politician,
4 Deceiver, 5 Scoundrel, 6 Turncoat, 7 Running Dog):

| sides in play | titles by rank, best first |
|---|---|
| 1 | Statesman |
| 2 | Statesman, Running Dog |
| 3 | Statesman, Politician, Running Dog |
| 4 | Statesman, Diplomat, Scoundrel, Running Dog |
| 5 | Statesman, Diplomat, Politician, Scoundrel, Running Dog |
| 6 | Statesman, Diplomat, Politician, Scoundrel, Turncoat, Running Dog |
| 7 | Statesman, Diplomat, Politician, Deceiver, Scoundrel, Turncoat, Running Dog |
| 8 | all eight in order |

So the worst-behaved side is always the Running Dog, however low its score.

A computer player uses the score when it picks whom to declare war on: the
**absolute difference** between its own score and the candidate's, divided by
8, is one term of the target score in `ai_pick_enemy` — so a side whose
reputation differs sharply from its own, in either direction, is a more
likely enemy. See `docs/re/ai.md` › Diplomacy.
Bits 4 and 5 of the side's own diagonal byte flag "has pending
proposals" and "has only de-escalation offers" (`484e:0cc7`).

## End of the game

`end_game_check` (Ghidra `8065:1aed`) counts sides in play by controller, and
counts the cities that still exist (razed ones excluded):

- **No side left in play** → "Alas! No more players are left!" (group 12)
  and the game ends.
- **All human sides gone** (and there were some) → "No further human resistance
  is possible! But the battle will continue!" (group 13). The computers play on.
- **One human side, no computer sides**, and it owns **more than half** of the
  existing cities → **victory**. The *game won* flag (`.SCN` `0x15b`) is set.
- **No human sides, one computer side left** → that side has triumphed
  (group 15). It's switched to human control so the game can be inspected,
  and the *game won* flag is set.
- **One human side with computer sides still playing**: if it owns more than
  half of all cities *and* more than the largest computer side + cities/8,
  the *surrender offered* flag (`.SCN` `0x15d`) is set. That leads to the
  "Surrender!" message (group 17).

Once *game won* is set, quests that need an enemy (types 3–5) aren't handed
out.

## Still unknown

- **Heavy Infantry "Move 16/20"** read off the production screen in DOSBox.
  The code can't produce it (base 8, at most +4 of variance), so one of the
  two readings is wrong — recheck in the game (`docs/formats/armytype.md`).
- **Computer players:** how they use the diplomatic score, and most movement
  and evaluation phases (`docs/re/ai.md`).
- **Random map generator:** the terrain phases (`docs/re/random_map.md`).
