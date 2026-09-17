# Game rules

Sourced from the *Warlords II Deluxe* manual (Appendix C, "Combat Mechanics",
and chapter 8, "Movement"), cross-checked against the shipped data files and the
original game running in DOSBox. Deluxe was a later release with balance tweaks,
so treat exact constants as "very likely" rather than certain for Warlords II
proper — but every rule below that could be checked against the original's data
or UI **did** check out.

## Two values that are derived, not stored

This is why neither could be found in the scenario file.

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
Largash (3) → 2, all displaying `Defence: 2`. Across Erythea's 80 cities this
gives 26 at defence 1 and 54 at defence 2. It also explains the help file's
`- Pillage -` → "Reduce defence for gold": pillaging removes production types.

## Combat

**Everything in this section up to "Resolution" is verified in `WARLORD2.EXE`**:
`combat_setup` (Ghidra `6a89:008b`) builds both lines, `combat_terrain_class`
(`6a89:0000`) classifies the tile, and `combat_resolve` (`67cc:08a6`) fights.
Where the code and the Deluxe manual disagree, the code wins, and the
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
- **tutorial exception:** in the tutorial scenario (option slot 10,
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

The Military Advisor runs **20 simulated combats** with the real routine and
reports the success count.

## Production — verified in `WARLORD2.EXE`

At game start, `setup_capitals` (Ghidra `79fa:07ca`) gives each side its
capital; every other city is neutral (owner 15). With **Quick Start** on
(option 7, `.SCN` `0x128`), all neutral cities are then dealt out round-robin:
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
when the owning side's flag at `.SCN` `0x00f0 + 2·side` is set (meaning not
yet identified).

**Buying a type** (`buy_production_type`, `7087:1299` / `7087:0ee3`) copies
the unmodified `ARMYTYPE.DAT` stats into the slot, with no random nudge, and
subtracts the purchase price (`+30`) from the side's gold (`.SCN`
`0x185 + 20·side`).

## Movement — verified in `WARLORD2.EXE`

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
from that. The manual's 2-MP carry-over and 8-army stack limit haven't been
checked in code yet.

A path is stored as up to 200 compass directions (0 = north, clockwise).

## Heroes — verified in `WARLORD2.EXE`

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
Dragons if none qualify). The price is paid first. The code has no chance of
*no* allies; that surprised me, so it's worth watching for in a real game.

Hero levels (Hero → Cavalier → Champion → Paladin, `STRING.DAT` groups
99–103) and quests haven't been read yet.

## Ruins, temples and sages — verified in `WARLORD2.EXE`

### What each site holds (rolled at game start)

Scenario files only mark each site as **temple** (content 1) or **ruin**
(content 2). `setup_random_sites` (Ghidra `66d4:0000`) fills in the rest.

**Magic items** (item records 8 and up; 0–7 are the sides' standards). The
number handed out is `sites/3 + 1d5 − 3`, at most 14. Most go into random
unassigned ruins. A band of `sites/5 − min(2d3+1, sites/5)` of them is held
back (status 0), probably for quest rewards; not yet traced.

**Every other ruin** rolls its content from a small table (3 = sage, 4 =
gold, 5 = allies):

| ruin | sage | gold | allies |
|---|---|---|---|
| "rich" flag set (site `+0x1b`) | – | 1/3 | 2/3 |
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
- **Sage:** no fight. The hero gains 3 experience and gets the sage dialog.
  Its gem is worth **3d500+500 gold**; the other options (a map of hidden
  locations) haven't been traced.
- **Ruin:** the hero gains 3 experience. If there's a guardian, it fights
  once:

  ```
  hero survives if 1d100 <= 90 + 5 * (hero strength + battle items − monster strength)
                               + 3 * (armies on the hero's tile, hero included)
  ```

  Monster strengths are a `u16` table at `.SCN` `0x1007` (Erythea: Troll 5,
  Giant 7, Wolf 4, Goblin 3, Dragon/Demon/Devil/Wizard 8, Ghost 7). A slain
  hero is removed (its items go through `67cc:16cd`, not yet read). With no
  guardian, or after a win, the ruin gives up:
  - **item:** the hero finds it
  - **gold:** **3d500+500** (**3d1000+1000** if rich)
  - **allies:** **1d2** armies of the stored type (**3–4** if rich) join on
    the spot, placed within ±1 tile if the site is full

Hero experience is capped at 60 (hero record byte `.SCN` `0x5e3 + hero`,
upper 6 bits).

## Still unknown

- The manual's 2-MP carry-over and 8-army stack limit (not yet traced in code).
- What the `2c04:00f0 + 2·side` flag is (+2 strength for newly produced armies).
- Hero emergence, levels, quests and item effects other than battle/command.
  `docs/re/dice_callers.md` lists the likely functions.
- The Heavy Inf "Move 16/20" in-game readings, which the production code can't
  produce (`docs/formats/armytype.md`).
