# Web port vs WARLORD2.EXE — verification findings

A pass over `web/src` against the decompiled original (`decompiled/`, Ghidra
addresses). Each finding says what the original does, what the web does, and
how much it matters. Deliberate modernisations (resolution, scaling, zoom,
the menus for them, the save format) are left out.

Severity: **high** changes play noticeably · **medium** changes outcomes in
some situations · **low** rare edge case, or only visible in detail.

## Summary

The engine port is close: combat resolution and modifiers, movement and
pathfinding, the turn's economy, heroes, sites, quests' checks, diplomacy,
reports and the computer players' phases match the original routine for
routine. What differs:

| # | finding | severity |
|---|---|---|
| T1 | *Random Turns* option has no effect | high |
| C7 | neutral cities that beat off an attack start producing (docs call it dead code) | high |
| C10 | medals (+1 strength) are never awarded | high |
| S1 | allies ruins have a guardian in the web, none in the original | high |
| P2 | production and arrivals spill onto tiles round a full city | medium |
| P3 | a city that fails to build goes idle in the original, retries in the web | medium |
| M1 | the original reuses cached routes, across sides and turns | medium |
| A1 | the AI's distance flood is an exact Dijkstra, the original's a bounded sweep | medium |
| A2 | `city.claim` (taken-from side) is never updated on capture | medium |
| C6 | "won a battle" history deed on every capture | medium |
| C8 | razing doesn't stop production of cities vectoring to the ruin | medium |
| Q1 | computers get the human quest types | medium |
| Q2 | quest reward items: any unused item, other sides' standards included | medium |
| Q3 | quest reward site: random, and its "known" mask cleared | medium |
| Q4 | quest reward allies: random type at home, not Archons at the hero | medium |
| S2 | "near a capital" uses Chebyshev distance | medium |
| … | about 30 low findings below | low |

## Status: fixed

Every finding below is now fixed, in the web, C++ and Lua ports alike,
except two that need no change: **A4** (kept as documented, since the
original's loop can run for ever) and **C12** (the ports already match).
**C9** is fixed too: a non-flying stack attacking onto water or shore now
puts its boats to sea as well. All three ports still play identical games
(`sh web/test/compare.sh`, `sh cpp/test/compare.sh`: every game the same).
Each port's test suite has a new `testVerified` section that checks these
rules one by one.

Found while fixing, and fixed with the rest:

| # | finding | severity |
|---|---|---|
| A5 | a computer attacking a city of a side it is not at war with commits treachery (`diplomacy.attackAnyway`, 67cc:124a → 67cc:20a8); the web did not | medium |
| A6 | a computer's attack is paid for in moves (67cc:0522), as a human's is; the web charged nothing | medium |
| A7 | the computer's army cycle starts each turn from its capital (8c07:0113) | low |
| A8 | the neighbour table is built once at game start over side 0's grid with every tile seen (623c:0398, 8611:113b), and its 50-slot list is the original's (623c:0749) | low |
| S9 | a hero killed by a guardian leaves the ruin to be searched again; a beaten guardian is gone (guardian 0); a found item empties the site | low |
| H5 | a dropped standard cancels vectoring only for the current player's own standard (67cc:16cd) | low |
| H6 | promotions are shown to a human in their own popups (LEVEL2–4.PCK, 7563:0672), and medals in theirs (MEDAL0–3.PCK, 67cc:2274) | low (UI) |
| U2 | Home (control 177) only centres on the selection; with none it does nothing (8065:0f02) | low (UI) |

The save keeps the new state (army slots, medals, the turn order, the
capital each side now has, the cities attacked, the neighbour table). A
loaded game starts with an empty route cache, as the original's does
(623c:0dff), so a save round trip can change which cached route a stack
takes.

Notes on method:

- The web's dice are not bit-compatible with the original's (stated in
  `web/src/warlords/rng.js`), so the *order* of dice draws is not reported.
  The *thresholds* are: the original's `dice(1,100,0)` returns 1–100
  (`trunc(rand()/32767·100 + 1)`, `7ecb:02bf`), so `dice(1,100,0) < 10` is a
  9% chance.
- **Decompiles of segment `6f8c` are unreliable**: Ghidra mis-resolves its
  near calls and cuts `6f8c:06bd` short (see P5). The raw code was read
  wherever it mattered (a capstone disassembler over `build/WAR2FLAT.EXE`).

---

## Turn sequence

### T1. Random turn order is not implemented — high

With the *Random Turns* option (`.SCN 0x122`), the original reshuffles the
turn order at the start of every round: 20 swaps of two random sides
(`turn_order`, `8065:1732`, called from `next_side` `8065:17f6`). The web
offers the option on the start screen (`ui/start.js`) and loads it
(`scn.js`), but nothing in `warlords/` reads `options.randomTurns`:
`game.endTurn` always plays the sides in index order.

### T2. The quest check runs at a different point — low

Original order at a computer's turn start (`8cc6:0000`): diplomacy, hero offer,
promotions, income, production, `1b62:0d85`, movement reset, encamp,
`8c07:0113`, **`quest_check`**, AI. A human's (`8cc6:0259`, then `8cc6:04bd`):
diplomacy, `1b62:0d85`, movement reset, encamp, **income**, `8c07:0113`,
sunk-army check, **`quest_check`**, advisor, then promotions, hero offer and
production.

The web (`game.startTurn`) runs the quest check **before** income for both. A
quest that ends with gold is settled against a different balance:
`apply_income` clamps gold at 0, so adding a reward before or after the clamp
gives different totals when upkeep exceeds income. See also H4 and P4 for the
other ordering effects.

### T3. Several heroes with double movement stack — low

`reset_movement` (`8cc6:05fb`) walks every hero of the side carrying a
double-movement item and adds each army's maximum moves to **every army on
that hero's tile**. Two such heroes on one tile add it twice. The web
(`resetMovement` / `heroWithDoubleMoveAt`) adds it at most once per army.

---

## Combat

### C7. Neutral cities that hold out start producing — high

`docs/rules.md` › *Neutral production during play — dead code* says nothing
sets city byte `+0x30`. It is set: `attack_tile` (`67cc:02b1`) and the
computer's attack (`auto_ui_being_attacked`, `67cc:1437`) do
`city[+0x30] |= 1 << attacker` whenever a city is attacked, and only a capture
clears it (`after_battle`, `67cc:0e9c`). So a **neutral city that beats off an
attack** is flagged from then on, and with *Neutral Cities* ≥ 2,
`neutral_production_turn` (`6bd8:025a`, run at each round's end from
`next_side` `8065:17f6`) makes it build:

- if a build is under way, count it down; at 0, place the army on the city's
  top-left tile (`produce_garrison`, `6bd8:0381`);
- if nothing is being built and fewer than 4 armies stand on the top-left
  tile, start the best type for purpose 2, balanced (`best_production_for`).
  This goes through `set_city_production`, so the current player's gold and
  Enhanced flag apply (P6).

The web has none of this, following the docs.

### C10. Medals are never awarded — high

`auto_ui_medal_effect` (`67cc:2274`) runs after every attack, the human's
(`attack_tile`) and the computer's (`67cc:124a`). When the attacker **won**,
no army of the stack is at sea, the stack has a **hero** (`DS:1efe`) and at
least one ordinary army (not hero, not magical), and the battle counted
(both sides at least 2 armies; or before turn 20, at least one defender and
the first defender's strength above 3), it rolls `dice(1,100) < max(12, 40 −
5 × the side's ordinary armies that already have a medal)`. On success a
random ordinary army of the stack gets a medal (army `+0x0a`, up to 4) and
**+1 strength** (max 9) — or +1 maximum move if its strength is above 14.
A human gets the medal screen (groups `0x9e`–`0xa2`, music cue 8).

The web draws medals (`ui/stack.js`, `ui/infobox.js`) but nothing awards
them; `PLAN.md` notes they are "not modelled".

### C6. "Won a battle" history deed — medium (History screen)

`after_battle` (`67cc:0a6b`, at `67cc:0af5`) records deed 5 only for a **big
battle at a city that the attacker won**: a city on the tile, attackers +
defenders > 7, and more than 3 defenders. When a hero led the attack
(`DS:1efe`), the deed carries the hero's name and `v1 = −1`; otherwise the
side's name and its index. The web (`applyAttack`) records `WON` for **every**
captured city, always with the side's name. The History screen's events
therefore differ.

### C8. Razing a city doesn't stop the cities vectoring to it — medium

`city_make_ruins` (`649c:016b`) treats every city vectoring to the razed one
as `after_battle` does on a capture: vector off, **countdown 0, production type
cleared**. The web's `makeRuins` only clears `vectorTo`, so those cities go on
building at home. (On capture the web does stop them, as the original does.)

### C1. Hero carrier: the wrong flier goes to the back — low

`combat_setup` (`6a89:008b`). On water, shore or mountains, a side holding a
hero has one flier moved to the back of its line. The original picks **the
first flier in collection order** (attackers: stack order; defenders: the army
table walked from the top), **skipping fliers at sea** (army flag `0x1000`),
adds 80 to its fight order and then sorts.

The web (`combat.js` `protectHeroCarrier`) sorts first and then moves **the
first flier of the sorted line**, and does **not** skip fliers at sea. With two
fliers of different fight order, or a flier aboard a boat, a different army
ends up last.

### C2. Defenders of equal fight order line up in a different order — low

`combat_setup` collects defenders by walking the whole army table **from the
highest index down** (over the 2×2 of a city at once), then insertion-sorts
by fight order (stable). The web (`combat.lines`) walks the city's tiles one by
one and `g.armies` **from the start**, then sorts stably. Defenders with the
same fight order therefore fight in a different order. See also G1.

### C3. Stalemate counter is per battle, not per duel — low (unreachable)

`combat_resolve` (`67cc:08a6`) counts throws **per pair of armies**: the
counter resets every time one dies, and after 10,001 throws without a hit the
attacker takes the hit. The web counts throws over the **whole battle**
(`throws` never resets). Strength is capped at 15 on a d20/d24, so a duel
always resolves long before; no visible effect.

### C4. Item values below 1 count as 1 — low (unreachable with shipped data)

`hero_battle_items` / `hero_command_items` (`6a89:0d1a`, `6a89:0d71`) and
`side_gold_per_city` (`66d4:07cf`) add each item's value as stored. The web
(`combat.itemTotal`, `game.itemBonus`) adds `max(1, value)`. Every battle,
command and gold item in the shipped `.ITM` pools has value ≥ 1.

### C5. A dead attacking hero drops its items on its own tile — low

`attack_tile` (`67cc:0000`) calls `after_battle(won, x, y)` with the
**attacked** tile, and `after_battle` passes that tile to `hero_drop_items`
(`67cc:16cd`) for dead heroes **of both sides**. So a fallen attacker's items
land on the tile it attacked, and are lost if that tile is water (attacking a
fleet). The web (`game.applyAttack`) drops a dead attacker's items at
`a.x, a.y`, the attacker's own tile.

### C9. Boats attacking onto water — known, deliberate

`67cc:0522` puts every non-flying army of any non-flying stack at sea with 0
moves when it attacks a water or shore tile from land, boats included. The
web (`chargeAttack`) does this only for land stacks, as its comment says.

### C11. Navies in Triumphs — low

The Triumphs tally (`67cc:1aff`) counts a navy lost only for an army that
fought as a boat (at sea **and** on a water or shore tile, `DS:029c` /
`DS:02a4`). The web (`history.tally`) counts any army with `atSea`.

### C12. View › Stack sums the heroes' bonuses — confirmed, no change

`PLAN.md` asks whether the stack view's sum of every hero's bonus matches the
battle. The original's view (`89e0:1b9c`) **sums** them too, while the battle
(`combat_setup`) takes the strongest hero. The web does the same in both
places.

---

## Production and setup

### P2. Production spills onto tiles around the city — medium

`city_production_turn` (`6f8c:0000`) calls the free-tile routine
(`6f8c:0bff`) with **spill = 0**, both for a newly built army (`6f8c:056c`)
and for a vectored army arriving (`6f8c:0123`). If all four city tiles hold 8
armies, nothing is built (see P3) and an arrival is sent home. Only
`hero_recruit` (`7563:031b`) passes spill = 1.

The web (`game.runProduction` and `deliver`) calls `freeTileIn(g, city, true)`
for both, so it places armies on random tiles around the city.

### P3. A city that fails to build goes idle — medium

In the original, a city builds only while its countdown (city `+0x2d`) is above
0 (`6f8c:04f3`). When it reaches 0 the countdown is left at 0. If the side has
no gold, or there is no room (P2), nothing is built — and from then on the
city is **skipped every turn**: its countdown stays 0 until production is set
again. The city dialog then shows no production (`7204:06de` treats a
countdown below 1 as "none"). For a human, the countdown is only restarted for
cities that did build (`6f8c:06bd`, see P5).

The web (`runProduction`) sets the countdown to 0 and decrements it again next
turn (to −1, still ≤ 0), so the city **retries every turn** until it succeeds.

### P1. City production nudges are 1 percentage point too likely — low

`setup_city_production` (`79fa:0a75`) rolls `dice(1,100,0) < 10` (9%) for the
strength, cost and time nudges, `< 20` (19%) for move, and `< 60` (59%) for
the direction of the strength, cost and time nudges. The web
(`rules.citySlots`) uses `rng.chance(p)`, which is `dice ≤ p`: 10%, 20%, 60%.
`rng.js`'s own comment says `chance` means `dice < p`, so the code
contradicts its comment. `docs/rules.md` › Production's table also says
10% / 20% / 60%. (`citySlots` is the only caller of `chance`.)

### P4. A computer's new armies get 2 extra moves on their first turn — low

`city_produce_army` (`6f8c:0dd7`) gives a new army its full move in both
`+6` (max) and `+7` (left). For a computer (`start_of_turn`, `8cc6:0000`)
production runs **before** `reset_movement`, which then adds
`min(moves left, 2)`: a new or arriving army starts with max + 2. For a human
(`8cc6:0259` → `8cc6:04bd`) production runs **after** the reset, so the army
keeps exactly its max.

The web (`runProduction`) sets new and arriving armies to 0 moves, then resets
them for every side: max + 0.

Besides the extra reach, this feeds the AI: `ai_phase_rescue` (`5ad0:0888`)
and `ai_phase_last_rescue` (`5ad0:0b4d`) pick out armies whose moves equal
max + 2 (an army that sat out a turn), which in the original includes every
army a computer built or received that turn. In the web those armies are
skipped until they have idled a turn.

### P5. Production keeps going — confirmed, with a decompiler warning

Not a divergence, recorded because the decompile suggests otherwise:
`city_produce_army` sets the countdown to 0, and `decompiled/6f8c_06bd.c`
shows no restart. The raw code of `6f8c:06bd` (Ghidra cut the function off at
51 bytes and mis-resolved its near calls) loops over the production report and
sets each building city's countdown back to its slot's time
(`6f8c:0744`). So a human's cities keep building, as the web does. For a
computer, `6f8c:06bd` is not called; `ai_production` re-sets production.

### P6. Enhanced uses the current player's flag — low

`best_production_for` (`623c:103d`) and `city_produce_army` (`6f8c:0dd7`)
apply the Enhanced bonus (+2 strength) from **the current player's** flag
(`.SCN 0xf0 + 2·current_player`), not the city owner's. During game setup
this means every garrison, neutral ones included, uses whichever side is
current then. The web (`setupGarrisons`) uses the city owner's flag, and none
for neutral cities. Shipped scenarios have every flag off, but the web's
settings screen lets a player turn it on.

### P7. City tile order is x first — low

The city tile table (`DS:0e54` dx = `0 1 0 1`, `DS:0e5c` dy = `0 0 1 1`) used
by `6f8c:0bff` fills **(x,y), (x+1,y), (x,y+1), (x+1,y+1)**. The web
(`game.freeTileIn`) fills (x,y), (x,y+1), (x+1,y), (x+1,y+1), and so does the
description in `docs/rules.md` › Production. Which tile a new army appears on
differs once the first one is full.

### P8. The spill test is "empty tile", not "fewer than 8" — low

When `6f8c:0bff` does spill (`hero_recruit` only, in the original), each of
its 20 random tries accepts a tile only if its map flag `0x10` ("an army
stands here") is clear, i.e. the tile is **empty**. The web accepts any tile
with fewer than 8 armies, **including one held by another side**.

---

## Movement

The cost grid, grid preparation, wavefront, trace, step costs, the walk,
going to sea / ashore and the stack's mode and bonuses all match
`1555:*` / `1a8b:*` / `1c8c:0529` closely. One structural difference:

### M1. The original reuses cached routes — medium

`pathfind` (`1555:000a`) first looks in a **route cache** (`1555:1508` →
`1555:1595`): 20 slots of 200 steps (slot 0 for routes under 15 tiles, slots
1–19 for longer ones), filled after every search (`1555:16f0`; when the long
slots are full, the slot whose stored route is shortest is replaced if shorter
than the new one). A request is answered from the cache when the
**destination, the stack mode and the woods/hills/at-sea flags match** and the
stack's tile **lies on the cached route**: the rest of that route is used, with
no search. The single-step shortcut is only tried after a cache miss.

The key has **no side**, and the cache is cleared only at start-up and when a
game is loaded (`path_cache_reset_all` from `1555:02f2` and `623c:0dff`), not
per turn or when cities change hands. So a stack can follow a route found
earlier, for another stack or another side, over a cost grid that has since
changed. The web (`move.findPath`) always searches afresh, so its routes are
always the current shortest ones: ties and stale routes come out differently.

---

## Heroes

### H1. No hero offers once surrender has been offered — low

`hero_offer_check` (`7563:0000`) returns "no offer" for every side, turn 1
included, while `surrender_offered` is set. The web (`hero.offer`) has no such
check.

### H2. Allies are placed on the hero's tile — low

`create_ally_army` (`6536:12e6`) puts each ally on **the hero's tile** if it
has fewer than 8 armies and is the side's own or empty. Otherwise it takes up
to 10 steps of a **random walk** from there (each step ±1 in x and y, applying
the same test), and if all fail **no ally is created**, although the message
already gave the count. Allies get **home city 0**. The web (`hero.recruit`)
gives each ally its own `freeTileIn(city, true)` search, falls back to the
hero's tile when that fails, and sets the hero's city as home.

### H3. Hero placed on the last spill try when the city is full — low

`hero_recruit` (`7563:031b`) uses whatever tile `6f8c:0bff` last wrote, even
when it returned "no room" (the last random try around the city). The web
falls back to the city's top-left tile.

### H4. A human's hero offer is checked against gold after income — low

For a human, `hero_offer_check` runs in `8cc6:04bd`, after `apply_income`
(so the price is tested against the gold after this turn's income and
upkeep), and the hire, with its allies, comes before the turn's production
claims the city's tiles. For a computer it runs before income
(`start_of_turn`). The web (`game.startTurn`) rolls every side's offer before
income and runs production before a human sees the offer.

---

## Ruins, temples and sages

The content tables (`DS:0c9a`–`0cae`), item hiding, gold amounts, ally
counts, temple numbering and blessing all match. Differences:

### S1. Allies ruins are guarded in the web — high

`setup_random_sites` (`66d4:0000`) stores an allies site's **ally type in the
guardian byte** (`+0x1a`). `ruin_search` (`6536:01ab`) skips the guardian
fight entirely when the content is 5 (allies). So in the original **an allies
ruin has no guardian and can't kill the hero**. The web (`site.setup`) gives
allies sites both `allyType` and a random `guardian` (1d9), and `site.search`
fights it, so the hero can die there. (The web's `ui/search.js` already omits
the guardian lines for allies, but a "killed" result still comes through.)

### S2. "Near a capital" is a Euclidean distance — medium

`setup_random_sites` treats a site as near a capital when `map_distance`
(`1a8b:0acc`, `floor(sqrt(dx² + dy²))`) to an in-play side's capital is below
15. The web (`site.band`) uses `max(|dx|, |dy|) < 15`, so sites up to 15
tiles away diagonally (about 21 in a straight line) count as near. This
changes which content and ally tables those sites use.

### S3. Gold found in a ruin isn't recorded — low (History)

`ruin_search` records deed 6 (FINDS) with `v1 = 102` (gold) after a gold
find. The web's gold branch records no deed.

### S4. A hero killed in a ruin isn't counted in Triumphs — low

`ruin_search` adds 1 to the side's own "heroes lost" count, and 1 per standard
the hero carried (`DS:1166 + side·0x5a`, `DS:116b`), before dropping its
items. The web removes the hero without `history.tally`.

### S5. The guardian fight counts every army on the tile — low

`guardian_fight_hero_dies` (`6536:07ad`) adds 3 per army on the hero's
**tile**, whoever's stack it belongs to. The web (`survivesGuardian`) adds 3
per army in the **moving stack**.

### S6. Searching needs a move left — low

`site_search` (`6536:0000`) does nothing unless the selected army has at
least 1 move left. The web's `canSearch` / `site.search` have no such check.

### S7. A temple without a hero blesses at once — low (UI)

With no hero in the stack, `site_search` calls `temple_bless` directly: a
blessing and its message, no temple dialog. The web opens the temple dialog
(Bless / Quest) either way.

### S8. Allies from a ruin are placed like a hero's allies — low

`ruin_search` places allies with `create_ally_army` (see H2: the tile, then a
10-step random walk, no terrain check). The web tries the tile, then the last
of its 8 neighbours with room and passable terrain.

---

## Quests

The success and failure checks (`quest_check`, `4976:1ded`), slaughter and
gold counts, item hand-in, and the +10 experience match. Differences:

### Q1. Computer players get the human's quest types — medium

The computer takes a quest through `5ad0:15c3` → `quest_assign(1)`
(`4976:0d7a`). With `param_1 ≠ 0` the type is **not** rolled from the 1d10
table: the first try is raze (1 in 5) or occupy (4 in 5), the second
slaughter, every later one pillage gold (which always succeeds). Ranges are
shorter too: retrieve-item 30 instead of 50, occupy 40 instead of 60. So a
computer never gets slay-hero, retrieve-item or slay-type quests. The web
(`site.search` → `quest.assign`) uses the human table for computers.

### Q2. Quest rewards: which items — medium

`quest_choose_reward` (`4976:1909`) offers an unused item only if it is
**reserved** (`item_reserved`: flight, double movement, a standard, or command
2+) and, for items 0–7, only **the side's own standard**. The web
(`quest.reward`) picks from every unused item, including other sides'
standards and ordinary items.

### Q3. Quest rewards: which site is revealed — medium

When no item is left, the original reveals the rich site that is **not yet
known to the side** (`+0x82e` bit clear), is not empty, and lies **nearest
the quest hero** (Chebyshev distance), and sets the side's bit in `+0x82e`
(`quest_give_reward`, `4976:1c02`). The web picks a **random** rich, unsearched
site, whether or not the side knows it, and sets `revealed = 0`, which
clears every side's bit instead of setting this side's. `quest.pickTarget`
does the same for a retrieve-item quest, where the original also only sets
the side's bit. (This settles the open question in `PLAN.md`.)

### Q4. Quest allies are Archons, at the hero — medium

`quest_give_reward` creates the allies with `create_ally_army(19, hero.x,
hero.y)` (`4976:1d5b`): always **type 19 (Archons)**, on the quest hero's
tile (H2 placement). The web (`quest.reward` → `allies`) uses a random magical
type and places them in the hero's home city with `freeTileIn`.

### Q5. City and item quest ranges — low

`quest_assign` measures occupy and raze targets with `map_distance`
(Euclidean, `≤ 60`, `< 61`) and items with `|dx| < 50 && |dy| < 50`. The web
uses `max(|dx|, |dy|) ≤ 60` for cities and `≤ 50` for items.

### Q6. Quest assignment never gives up — low

`quest_assign` loops until a type finds a target (pillage gold always does).
The web stops after 20 tries and gives no quest, about 1% of the time.

### Q7. Slay-type targets include neutral armies — low

The slay-type check accepts any army of the type not owned by the side (owner
15 included) and standing on the map (`x, y ≥ 0`). The web excludes neutral
armies and doesn't skip armies in transit.

---

## Computer players

The AI port is close: phase order (`ai_turn`, `5db9:0000`), diplomacy
(`558d:*`), enemy choice (`5f19:0e04`, including its slips), evaluate,
garrisons (`5ca7:023f`), neutrals, quick attack, odds and retargeting,
production purposes, vectoring, rebuilding, the sage, rescue and the move
phase were checked against the decompile and match. Differences found:

### A1. The AI's distance flood is a bounded sweep, not Dijkstra — medium

`1555:1ab9` floods by calling `pathfind` with the start as its destination
and flag `0x10`. With that flag `path_wavefront` (`1555:0373`) runs **at most
`range` sweeps** (stops when `DS:0008 == range`), each over a square around
the start that grows by one per sweep, scanning x then y and spreading each
tile once per sweep. Costs travel quickly with the scan but only one tile per
sweep against it, so on a winding route (round a lake, through a pass) the
distance comes out too high, or the tile stays unreached, when the sweeps run
out. It also uses the search's water penalty: 20 when the start tile is water
or shore, else 10.

The web's `core.floodFrom` is an exact Dijkstra over the same box with the
penalty always 10. Every AI decision built on it — the city neighbour table
(radius 45/60), `cityDistance`, `bestCity`, retargeting, group staging —
can see shorter distances, and reach cities the original considers out of
range.

### A2. A city's "taken from" side is never updated — medium

City byte `+0x2f` (the web's `city.claim`) is rewritten by `after_battle`
(`67cc:0a6b`) on every capture of a non-neutral city: it becomes the side the
city was taken from, or 15 when that side takes it back. The AI reads it as
"cities taken from me" (`ai_phase_diplomacy`: the grudge counts;
`ai_pick_enemy`: `fromThem × 4`). The web sets `claim` only at game start
(`ai.shareOutCities`), in the diplomacy phase's own claiming, and on razing,
so the computers never hold a grudge over lost cities.

### A3. A computer's new hero has 2 extra moves, and is always hired — low

The computer hires during `start_of_turn` (`7563:0301`), **before** income,
production and the movement reset. The hero (14 moves) and its allies get
`+ min(moves, 2)` = 2 more on their first turn, as in P4. The price was
checked against the gold before income (`hero_offer_check`), and the hire
always goes through. The web hires at the start of `ai.playTurn`, after the
reset (no bonus), and checks the gold again after income and upkeep.

### A4. Staging stacks stop after a pass that changed nothing — known

`groups.gather` stops repeating a staged stack's strike once a pass changes
nothing, where `563e:0996` repeats for as long as it finds a stack to strike
(and can loop for ever). Documented in the code.

---

## Round end, victory, diplomacy

### R1. Surrender threshold counts razed cities — low

`end_game_check` (`8065:1aed`) offers surrender when the last human holds more
than **`city_count / 2`** cities and more than the biggest computer's count
plus **`city_count / 8`**, where `city_count` is every city in the scenario,
**razed ones included**. The "rules the world" test uses standing cities. The
web (`checkEnd`) uses standing cities for both, so it offers surrender sooner
once cities have been razed.

### R2. A side's diplomacy with itself — low

`eliminate_sides` (`8065:18ab`) sets state and proposal to 1 for every pair
with the fallen side, **including itself**; `diplomacy_init` (`484e:11bd`)
with Diplomacy off sets every pair to war, **the diagonal included**; and
`ai_turn` (`5db9:0000`) resets the side's own diagonal to peace each turn.
The web keeps the diagonal at peace. Only matters where something reads a
side's relation to itself (the AI's solidarity loop does propose to itself).

### R3. The capital follows the side — low (view only)

Before each side's turn, `67cc:10d7` moves the side's current capital (side
`+0x189/+0x18b`) to its city nearest the old one when the side no longer
holds its original capital, and back once it does. The human's turn opens
centred there (`8cc6:0259`, `8c07:0113`). The web keeps `side.capital` fixed,
so a turn opens centred on a city the player may have lost. The AI and the
city dialog use the original capital (`+0x193`) in both.

---

## Interface

Only behaviour was compared here, not drawing.

### U1. Capture dialog: enabling and the default button — low

`city_capture_dialog` (`63fa:0000`) enables Pillage and Sack by the city's
pillage / sack **value** (`63fa:0834`, `63fa:07d0`), and when the stack's hero
is the side's quest hero, makes **Raze** the default for a raze quest on this
city and **Sack** (or Pillage) for a pillage-gold quest; otherwise Occupy.
The web (`presentVictory`) enables them by **slot count** and sets no default.

---

## Data model

### G1. The army table is ordered differently — low, broad

The original keeps armies in a fixed table of 1,000 slots: a new army takes
the **lowest free slot** (`1b62:0309`), a dead one frees its slot
(`1b62:03be`), and most scans walk it **from the top down**
(`armies_at_tile`, `1b62:007d`; `combat_setup`). The web appends new armies
to the end of `g.armies`, removes dead ones with `splice`, and scans from the
start. Wherever the original breaks ties by table order (C2, which army of a
stack is listed first, AI scans that take the first match), the web can pick
a different army.

---

## Checked and matching

For the record, these were compared against the decompile (and the raw code
where the decompile was unclear) and match:

- **Combat**: terrain class and its jump table, both lines' modifiers and
  caps, hero table (`DS:0ce4`), command and battle items, fortification and
  its halving for neutrals, at-sea strength 4, the dice loop and tutorial
  protection, the advisor's 19 battles.
- **Setup**: production lists and Navy removal (no shipped scenario hits the
  original's single-pass compaction), purpose weights (`DS:0a10/0a1e/0a2c`),
  garrison purposes (`DS:0d60`), placeholder Scouts, the 1d8 (+400) scores.
- **Turn**: income, upkeep (at sea at least 4), movement reset and carry,
  encampment terrain, transit and vectoring rules, elimination, the end of
  the game, accepted surrender.
- **Heroes**: offer odds and prices, the 5/6 limit, promotions (15/30/60, +1
  strength, +2 moves), experience cap, drop / take / plant flag.
- **Sites**: content and ally tables, rich sites, item placement bands, gold
  and ally counts, temple blessing.
- **Quests**: every success and failure branch, rewards' amounts.
- **Diplomacy**: apply, score update, ratings and their rank tables, attack
  anyway, the "city 0" quirk of `attack_tile`.
- **Movement**: everything in `move.js` apart from M1.
- **Reports**: Army, City, Gold, Winning score and rank, the even-scale rule;
  history's deed slots and the 201-turn limit.
- **Options**: presets (`DS:2378`), side setup on Begin, *View Production*
  and *View Enemies* (both read as the original reads them).
- **AI**: see the note at the top of its section.

## Not covered, or only sampled

- Drawing, layout and fonts of the dialogs (positions, colours, pictures).
- Sound and music cues beyond the ones named above.
- The random map generator: sampled (`513d:1171`, `513d:161d`); `web/PLAN.md`
  already lists where it departs from the original.
- The computer's hero phase (`6087:*`) and explorer walk (`57ea:0e4b`–`1bab`)
  were read only in part.
- The save format, which is the web's own.
