# Computer players — structure

What's known about the AI in `WARLORD2.EXE`. Every turn phase is identified,
and the rules behind its diplomacy, production, garrisons, city roles, hero
expeditions and assaults are decoded; what is left is the ordering inside the
assault and move routines. A remake can reasonably write its own AI; this
page is for matching the original's behaviour where that matters.

Addresses are Ghidra addresses.

## Who is a computer

`is_computer_turn` (`5db9:09ab`) is true when the side's controller
(`.SCN` `0x00d0 + 2·side`) is 1, or while the AI turn itself is running (a
human side is briefly flagged as computer during one phase so shared code
takes the automatic path). Levels live at `.SCN` `0x00c0 + 2·side` (0–2).

## Levels and characters

A level does nothing by itself. At game start (`ai_init_side`, `59bf:084d` →
`59bf:0d7b`) it picks built-in defaults for the side's AI data, and then the
side's **character card** (`CARDS/K|L|W nnn.CRD`, number at `.SCN`
`0x00e0 + 2·side`) overwrites every one of them, and the side's fight order
too. So a Knight, Lord or Warlord is whatever its card says: how many
assault groups run at once, whether it attacks sides it is not at war with,
how carefully it takes neutrals, whether it razes, sacks or pillages what it takes, how soon it
turns on a human who is winning, and whether it stands with the other
computers. The card layout and all 27 cards are in
[`../formats/crd.md`](../formats/crd.md).

After that, the level is read in only two places, each picking which of
the card's per-level values to use: a constant added to every candidate in
`ai_pick_enemy` (so it changes nothing), and a bonus to the three
raze/sack/pillage chances (`5f19:06a0`).

In outline, the Standard cards:

| | Knight | Lord | Warlord |
|---|---|---|---|
| assault groups at once | 1 | 3 | 4 |
| attacks a side it is not at war with (`558d:0851`) | no | yes | yes |
| careful with neutrals, no *quick attack* | yes | no | no |
| raze / sack / pillage what it takes (‰) | 20 / 0 / 0 | 1 / 2 / 3 +1 per city | 5 / 10 / 20 +5 per city, +50 sack/pillage when poor |
| razes or sacks a human's city before turn 10 | no | no | yes |
| declares war on a human holding | 81–90% | 51–60% | 31–40% of all cities |
| won't fight a computer that fights a human | no | no | yes |

## I am the Greatest

The setup button (`7bab:0ee8`) makes every side in play a computer Warlord,
resets the card of any side that wasn't one, and sets `DS:3c04_0116`. The
flag stays set when sides are changed afterwards, so the player turns their
own side back to Human; only *No! I really am Normal* clears it. It's a
global, not in the `.SCN`. With it set:

- at game start every **human** side's diplomatic score is 1d8 **+ 400**
  (`79fa:0000`; computers get 1d8), so `ai_pick_enemy`'s
  `|score difference| / 8` term adds about 50 against every human;
- in *diplomacy* (`558d:0000`) and in `ai_pick_enemy` (`5f19:0e04`), when
  either this side or the other computer is at war with a human, the
  computer is **dropped**: no war proposed on it, no score as an enemy.

So the computers gang up on the human.

## Turn pipeline (`ai_turn`, `5db9:0000`)

Each phase is announced with a debug string (at `DS:0838`…) before it runs.
The phases always run in this order:

| # | debug name | function | notes |
|---|---|---|---|
| – | `Begin player %d` | `5db9:0386` | clears own diplomacy proposal bits, locks the side's AI data |
| 1 | `Diplomacy` | `558d:0000` | |
| 2 | `move Hero` | `6087:0000` | uses `ai_choose_target` (`6087:0efc`: 4 candidates scored `100 − distance + 1d20`) |
| 3 | `move Search` | `5ad0:0284` | only with *Hidden Map* on |
| 4 | `move Explore` | `5ad0:0458` | |
| 5 | `assault` | `563e:0000` | |
| 6 | `move #1` | `5ad0:0000` | |
| 7 | `rescue` | `5ad0:0888` | |
| 8 | `evaluate` | `59bf:0000` | |
| 9 | `clean city` | `5ca7:01f1` | |
| 10 | `neutral` | `57ea:0000` | |
| 11 | `move #2` | `5ad0:0000` | same routine as `move #1` |
| 12 | `quick attack` | `5e97:04ba` | |
| 13 | `update hide` | `59bf:0c1c` | only with *Hidden Map* on |
| 14 | `assault XX` | `5f19:0000` | |
| 15 | `specials` | `5ad0:10e3` | |
| 16 | `rebuilding` | `5db9:0af2` | buys new production types, below |
| 17 | `last rescue` | `5ad0:0b4d` | |
| 18 | `production` | `5db9:06d4` | picks a production purpose per city role, below |
| 19 | `vectoring` | `5db9:085f` | vectors member cities to group targets, below |
| – | | `diplomacy_score_update` | see `docs/rules.md` › Diplomacy |

## AI data

Each side has a `0x42c`-byte block, held in a memory handle at
`DS:40ca + 4·side` and saved as block 10 of a save file
(`docs/formats/save.md`). Fields seen so far:

- `+0x02`, `+0x04`, `+0x06`, `+0x08`: cities owned by this side, by an enemy,
  neutral, and not yet seen — recounted by *evaluate*
- `+0x56 + city`: a per-city **role** byte, 1–13; drives production purpose
  and vectoring (below)
- `+0xba + city`: turns this side has held the city
- `+0x11e + city`: per-city flags — **bit 0** "not seen yet" (cleared by
  *evaluate* once any tile within the city's 4×4 neighbourhood is explored,
  and only ever set with *Hidden Map* on), **bit 1** "produces a flier",
  **bit 2** cleared for own cities at the start of the turn
- `+0x0a`, `+0x0e`, `+0x10`, `+0x12`: used by *rebuilding*, below
- `+0x46`: the city the side is currently working from (skipped as a
  neighbour candidate)

A shared `0x4b0`-byte block follows the eight side blocks in a save.

### Battle statistics (`ai_record_battle`, `5db9:09d7`)

Seven `u16[8]` arrays, indexed by the **other** side, are kept in each side's
block and updated after every battle on that side's tile — so they record
what each opponent has done to this side:

| offset | counts, per attacking side |
|---|---|
| `+0x3bc` | this side's **heroes** killed |
| `+0x3cc` | this side's **armies** killed |
| `+0x3dc` | **battles** fought |
| `+0x3ec` | battles **lost** (every defender died) |
| `+0x3fc` | battles fought **in a city** |
| `+0x40c` | **cities lost** |
| `+0x41c` | unused by anything read so far |

These are what the diplomacy phase and `ai_pick_enemy` mean by "threat": a
side that has killed this side's armies and taken its cities scores highest.
They are zeroed for everyone at game start (`59bf:084d`).

### Assault groups

The side's attack plans live at `+0x24a` (count) and `+0x24c` (entries of
`0x5c` bytes). Both the `assault` and `vectoring` phases walk them:

| offset in entry | meaning |
|---|---|
| `+0x00` | active; also counted up each turn the assault continues |
| `+0x02` | the **side being attacked** |
| `+0x04` | the group's **rally city**, one of this side's own — given role 7 |
| `+0x06` … | up to four **member cities** (role 6), which vector production to the rally city |
| `+0x0e` … | up to six **enemy cities** the plan is aimed at |
| `+0x46` … | up to six of this side's cities taking part |

### City roles (`+0x56 + city`)

| role | meaning |
|---|---|
| 1 | just captured (set when the AI takes a city, and by *evaluate* for any own city with no role yet) |
| 2 | taking a neutral city: armies are on their way to one (`57ea:00b5`) |
| 3 | has a neutral city among its six neighbours |
| 4 | garrison below 2 armies, or no neutral neighbours left |
| 5 | garrison below the wanted size (below); also set on a city that has just been given a new production type |
| 6 | member of an assault group; vectors production to the group target |
| 7 | an assault group's target |
| 8 | stop producing here (set on own role-7 cities at the start of `assault`, and when the garrison is full enough) |
| 9, 10, 12 | **never assigned** — the production switch has rows for them, but nothing writes these values |
| 11, 13 | explorer: set on every own city on turn 1 (13) and turn 2 (11) when *Quick Start* and *Hidden Map* are both on, and on a role-8 city when enemy cities are known, fewer than 5 assault groups are running and the city can build a flier |
| 14 | nothing left to vector to (`5db9:0c83`) |

Roles are set all over the AI, not only by *evaluate*: the `neutral` phase
assigns 2, 3 and 4 by whether the city still has neutral neighbours to take
(`57ea:00b5`), and the garrison check (`5ca7:023f`) sets **4** below two
armies, **5** below the garrison it wants, and **8** when the city is full
enough to stop producing.

Roles 11 and 13 are temporary: at the start of the side's turn
(`ai_turn_setup`, `5db9:0386`) they fall back to **4** before turn 5 and to
**8** from turn 5 on.

## Decisions decoded so far

**Accepting a hero.** The computer **always** hires an offered hero it can
afford (the offer's own 20% chance and price still apply; see
`docs/rules.md` › Heroes). `ai_hero_city` (`5db9:0919`) only picks where the
hero appears: each owned city scores by its role byte, `7` → 1d100+100,
`2` → 1d100+50, `3` → 1d100, anything else 0. The best score wins.

**Rebuilding** (`5db9:0af2`). With `limit = max(5, data[+0x0a] × city_count
/ 80)`, the side considers buying a production type when
`data[+0x0e] > 4` or `data[+0x02] ≤ limit × data[+0x0e]`, and it has **at
least 500 gold**. It wants type `data[+0x10]` (or `data[+0x12]` once it has
more than 2000 gold), finds a city for it (`5db9:0c83`) and buys it there
(`5db9:0bd1`), provided it still has 500 gold.

**Production** (`ai_production`, `5db9:06d4`). Per owned city, in reverse
order: clear its vectoring, skip it if it's already building, and **stop the
whole phase** if the side has less than 40 gold *and* its income is below its
upkeep. Otherwise the city's **role** byte picks a purpose for
`best_production_for` (`docs/rules.md` › Starting garrisons):

| role | purpose |
|---|---|
| 2, 13 | 4 — flying types (after a per-city flag check at AI data `+0x11e`) |
| 3 | 2 if *Neutral Cities* is on, else 1 |
| 4 | 2 — balanced |
| 5, 6, 7, 9, 10, 12 | 3 — strongest |
| 8 | stop producing in this city |
| 11 | 4 — flying types |
| anything else | 3 |

**Wanted garrison** (`ai_wanted_garrison`, `5ca7:0a3d`). Counts the city's
six neighbouring cities held by another side: **8** armies if this side is at
war with any of their owners, otherwise **4** for two or more such
neighbours, **3** for one, and **2** for none. The garrison check
(`5ca7:023f`) compares the city's army count with it: fewer than 2 → role 4,
fewer than wanted → role 5, otherwise role 8 (stop producing).

**Evaluate** (`ai_phase_evaluate`, `59bf:0000`). Housekeeping, not scoring:
it marks which cities can build fliers (`59bf:09cf`), clears the "not seen
yet" flag for cities whose surroundings are explored (`59bf:0b55`, only with
*Hidden Map* on), then walks every city to recount the four city totals
above, bump `+0xba` for its own cities and clear the role and counter of
cities it no longer owns. Cities it owns with no role yet get role 1, and
role-1 cities are resolved: **role 3** if the city has a neutral city among
its six neighbours (`57ea:01f6`), otherwise **role 5**.

**Assault** (`ai_phase_assault`, `563e:0000`). Re-evaluates first
(`59bf:01b3`), turns its own role-7 cities into role 8, then runs each active
assault group (`563e:00ca`): give up if the side's capital has fallen and no
group targets its captor, otherwise it runs the group (`563e:00ca`):

1. `563e:041c` re-checks the plan, dropping cities that changed hands; if
   nothing is left the group is cancelled (`563e:066d`, which puts its
   member cities back on role 8).
2. The group acts when no other group shares its rally city and the side
   still owns it, or when `563e:02ed` finds a nearby own city to work from.
3. `563e:0996` gathers armies — four passes, each calling `563e:1251` to walk
   the staging list at `+0x3e`.
4. The rally city's garrison is re-checked (`5ca7:023f`), then `563e:06e9`
   takes the stack east of the target city and sends it in; if it reports an
   attack, the garrison is re-checked again.
5. `563e:16fd` follows up from the participating cities at `+0x46`.

A group that acts has its turn counter incremented.

**Vectoring** (`ai_vectoring`, `5db9:085f`). For every active group whose
rally city the side still owns, the rally city is marked **role 7** and each
of the four member cities that is **role 6** has its production vectored to
it (`623c:0f10`).

**Diplomacy** (`ai_phase_diplomacy`, `558d:0000`). The phase only sets the
side's **proposals** (bits 2–3 of the diplomacy byte, `docs/rules.md` ›
Diplomacy); the proposals themselves are applied at the start of the turn.
It starts by proposing **peace with everyone**, then walks the cities to
count, per side: cities owned, cities whose flag bit at AI data `+0x11e` is
clear, and `swapped[j]` — cities that have changed hands between this side
and `j` (city `+0x1b` holds the previous owner).

With `grudge[j] = 2 × (j proposes peace to us) − 2 × (j proposes war on us)`:

| test | proposal toward `j` |
|---|---|
| `swapped[j] ≥ grudge + 4` | intermediate |
| `threat[j] ≥ grudge + 5`, where `threat = battles lost to j + 4 × cities lost to j + 2 × heroes killed by j` | keep, at least intermediate |
| `j` is the enemy chosen by `ai_pick_enemy` | **war** |
| `j` is the side an active assault group is aimed at | **war** |
| `j` owns more than a threshold share of all city tiles (50% for a computer, AI data `+0x44` for a human) | **war** on `j`, peace with everyone else |
| `j` has no unflagged cities (`+0x11e` bit set on all of them) | peace |

Both the `swapped` and `threat` tests have a **dead branch**: the "escalate
to war" comparison is nested *inside* the failing half of the enclosing test
(`if (swapped < grudge+4) { if (swapped >= grudge+8) …}`), so it can never
run. War therefore only ever comes from the last three rows.

Computer sides also hold back from each other: a proposal against another
computer is dropped when `558d:0a6e` says so, and when any human is at war
with a computer, computers with a non-zero `DS:40f2` entry are left in peace.
Finally, assault groups aimed at a side the phase has just made peace with
are cancelled (`563e:066d(group, 0x14)`).

**Picking an enemy** (`ai_pick_enemy`, `5f19:0e04`) — this is where the
**diplomatic score** is used. Each side `j` scores:

| term | weight |
|---|---|
| random | 1d10 |
| `j` holds our capital city | +20 |
| we hold `j`'s capital city | +15 |
| cities we hold that `j` used to own | +4 each |
| heroes `j` has killed | ×4 |
| armies `j` has killed, battles fought with `j` | ×1 |
| battles lost to `j`, city battles with `j`, cities lost to `j` | ×2 |
| a constant for our own controller: human `+0x18`, else level 0 `+0x1c`, 1 `+0x1a`, 2 `+0x1e` | +1 |
| **&#124;our diplomatic score − `j`'s&#124; / 8** | +1 |
| &#124;our city count − `j`'s&#124; / 4 | +1 |

A term meant to weight enemy cities next to ours is **dead**: it accumulates
into `score[owner of our own city]`, which is always this side's own slot,
and that slot is zeroed before the pick.

A side's score is then zeroed if `558d:0a6e` vetoes it, if we already propose
war on somebody and have no proposal toward it, if there are no assault
groups and it has no unflagged cities, before **turn 8** (turn 4 with *Quick
Start*) unless this side has already fought it, or if more than one other
side is in play and it is already the target of more than a quarter of its
own cities' worth of assault groups. The highest remaining score wins;
whoever holds our capital overrides that unless an assault group already
aims at them. A target with no unflagged cities is dropped.

**Movement phases** (`5ad0:*`). Two routines do the work.
`ai_collect_stack` (`623c:13b2`) gathers up to **8** of the side's armies
standing on one tile — filtered by the two halves of the army record's order
byte `+14` and by a minimum movement allowance, then sorted by `623c:14d3`.
`ai_send_hero_party` (`5ad0:11a6`) then picks where that stack should go:

1. Find the **hero** in the stack and the first army that **flies**
   (`army_move_flags`, `DS:0664 + 6·type`); with a hero but no flier it gives
   up (returns 1).
   The stack is cut down to exactly those two, and both armies' orders are
   cleared.
2. Score every **site**: it must be reachable (`623c:16ae`), stand on a site
   tile, not be a temple, not already be explored (tile flag `0x40`), and not
   already be some other hero's destination. With `d` the distance,
   `score = 215 − d` for `d < 15` and `90 − d` for `d < 40`; anything further
   is ignored. The best site wins.
3. Otherwise score the side's own **cities** the same way — `115 − d` inside
   15 tiles, `40 − d` inside 40 — with an assault rally city (role 7) counted
   as 80 tiles closer than it is.
4. A chosen site becomes order **3** toward that site with flag `0x100` set.
   `623c:0ae7` loads the two armies into the shared selected-stack array
   (`DS:1ede`) and moves them with the same `move_stack_to` the human
   interface uses. `5ad0:15c3` then searches the site if the party is
   standing on it and still has movement left — and, on a temple with
   *Quests* on and no quest running, takes a quest (`quest_assign`).
   A chosen city more than 2 tiles away becomes order **1**, the ordinary
   vectoring order.

So the hero phases amount to: send each hero, with one companion, to the
nearest unexplored ruin, and home to a city when there is none.

- *move Explore* (`5ad0:0458`) drives this for **heroes only** — an army of
  type 28 whose flag `0x100` is set (the flag is cleared as it is picked
  up) — while the hero has **3 or more** movement points left and is still
  making progress.
- *move Search* (`5ad0:0284`) is the same walk restricted to *Hidden Map*
  games.
- *rescue* (`5ad0:0888`) tidies up armies that still have **all** their
  movement. An army sitting outside a city with a vectoring order (order
  nibble 1) whose destination is no longer a city has that order cleared —
  for itself and every army of the side stacked with it heading the same
  way. An army outside a city with no order at all, and flag `0x20` clear,
  is sent off by `5ad0:0c5b`.
- *specials* (`5ad0:10e3`) looks at own cities flagged `0x20` in the AI
  data's per-city byte that aren't an assault target: if the stack on the
  tile east of the city can be sent and arrives, and the city isn't building
  anything, the city becomes an **explorer** (role 13).

## Found while porting it

The engine's port is `love2d/warlords/ai.lua` and `love2d/warlords/ai/`.
Writing it turned up the following, most of it read from the disassembly
where the decompile had lost arguments.

- **Distances** are `map_distance` (`2012:1199`): `floor(sqrt(dx² + dy²))`,
  not the larger of the two steps.
- **Neighbours.** The shared block (`0x4b0` bytes) is each city's six
  neighbours and their path lengths, built once at game start
  (`623c:0398`): a land flood from the city (radius 45, then 60 if that
  finds nothing) with every other city blocking, nearest first, except
  that once three lie west, east, north or south of it, more on that side
  count 50 farther (`623c:0749`).
- **Claims.** City `+0x2f` is not a previous owner: nothing writes it on a
  capture. At game start every city gets its owner's index and then the
  computers share out the rest, round the table, each taking the city
  nearest its last pick (or, half the time, its capital) (`623c:010b`); each
  diplomacy phase claims one more within 40 of the capital (`558d:0917`);
  razing clears it. Diplomacy's `swapped` count and `ai_pick_enemy`'s
  "cities we hold that it used to own" read it.
- **Odds.** Every "chance of winning" is the battle fought `+0x20` = 10 times
  without effect (`623c:15c5`).
- **Flood distances.** `1555:1ab9` runs the pathfinder's wavefront from a
  point within a square radius; a city's distance is the least over the
  twelve tiles round its footprint (`59bf:0a85`), cost + 1, unreached 30001.
- **Fog.** `path_prepare_grid` (`1555:08bf`) blocks unseen tiles only on a
  human's turn: computer players path straight through them.
- **The walk.** A computer stack's `move_stack_to` (`1a8b:0001`) re-plans at
  an enemy city (`623c:1771`): it attacks only a neutral city, a side it is at
  war with, or -- one time in four -- a human's, and only with better than 51%;
  otherwise it turns to the best city in 15 (`623c:1885`: `400 − d + 10 ×
  odds` within this turn's move, `100 − d + 5 × odds` with a fair chance).
  A group's stack with no path to its target is **disbanded** (`563e:0f1b`),
  as is an idle army out of town with nowhere to go (`5ad0:0c5b`).
- **Gathering goes round on a strike.** `563e:0996` runs each staged stack
  again for as long as `563e:0b49` returns 2 — "an enemy stack was found and
  gone for" (`563e:0c32`), whatever the move came to. It only ends because
  the move spends the stack's movement or changes the board. A stack that
  could not take a single step would be found again every time, until the
  odds' dice fell to 75% or less, which for a strong stack is never. The
  remakes once hit this for real: their walk stopped dead before a stack of a
  side at peace instead of passing over it (`1a8b:07f9`), so a computer turn
  could go round for ever, armies "walking" and nothing changing. With the
  walk put right it takes a rare map to get there, but the remakes also end
  the round of a staged stack when a pass has changed nothing.
- **A broke conqueror can stall.** Production needs gold above 0 and stops
  while the side is under 40 gold and losing money; assault groups are fed
  only by what their member cities build (vectoring), never by marching
  garrisons in; and a strike needs better than 75% or a full stack of 8. A
  side whose upkeep has outgrown its income therefore keeps its rally cities
  at a handful of armies, and a well-held last enemy city can outlast it
  indefinitely — seen in an all-computer Erythea (*Diplomacy*, *Hidden Map*,
  *Quests*, seed 12): 72 cities and 377 armies, 48 gold a turn short, against
  one city of five. That is the original's own logic, not a remake bug; the
  original has no turn limit either, and its way out is Shift or Alt during a
  computer's turn, to hand a side to a human in Settings.
- **Clean city** (`5ca7:023f`) moves the armies between the city's four
  tiles: the "keepers" (by `DS:0824`, chosen by `5ca7:0b5c`) on (x+1, y),
  then eight to a tile on (x, y), (x, y+1), (x+1, y+1). For a rally city
  the keepers are the strike force -- a hero, fliers and the special types
  first -- and `563e:06e9` launches exactly that tile. It also disbands
  beyond 24 armies, and beyond heroes + magic + 4 when the side is broke.
- **`+0x0c` bit 0** is read only by `558d:0851`: it lets the side attack a
  side it is at peace or uneasy with, provided it is at war with nobody else.
  The Lord and Warlord defaults set it and a card can only add it, so only
  Knights hold back.
- **Dead code.** `558d:0a6e` reads the solidarity mark of *human* sides,
  which only a computer turn ever sets. `563e:16fd` collects the follow-up
  stack by passing the group's wanted size as the order number, so no
  stack ever matches. `ai_pick_enemy`'s capital override compares the
  holder with the per-side group counts rather than the groups' targets.
- **Diplomatic score.** `diplomacy_score_update` (`484e:1063`) runs at the end
  of every turn -- `ai_turn` for a computer, `8065:2074` for a human -- and
  proposals are never used up: they stand until changed.
- **The sage** gives a computer the map round the unseen neutral cluster
  next to its cities (`5e97:0080`), or else the gem.

**Debug output.** `5db9:0dc9` receives every phase string. It's a 5-byte stub
in the shipped game, so the debug log is compiled out. The `auto_str_*` AI
functions (`Scan dist …`, `Quest city … GOING !!`, `Legal site …`,
`Enemy city … combat …`) are leftover debug-format strings in the same style.
