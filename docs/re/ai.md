# Computer players — structure

What's known about the AI in `WARLORD2.EXE`. Most individual phases aren't
decoded; the economic ones are. A remake can reasonably write its own AI; this
page is for matching the original's behaviour where that matters.

Addresses are Ghidra addresses.

## Who is a computer

`is_computer_turn` (`5db9:09ab`) is true when the side's controller
(`.SCN` `0x00d0 + 2·side`) is 1, or while the AI turn itself is running (a
human side is briefly flagged as computer during one phase so shared code
takes the automatic path). Levels live at `.SCN` `0x00c0 + 2·side` (0–2).

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

- `+0x56 + city`: a per-city **role** byte, 2–13; drives production purpose
  and vectoring (below)
- `+0x02`, `+0x0a`, `+0x0e`, `+0x10`, `+0x12`: used by *rebuilding*, below

A shared `0x4b0`-byte block follows the eight side blocks in a save.

### Assault groups

The side's attack plans live at `+0x24a` (count) and `+0x24c` (entries of
`0x5c` bytes). Both the `assault` and `vectoring` phases walk them:

| offset in entry | meaning |
|---|---|
| `+0x00` | active; also counted up each turn the assault continues |
| `+0x02` | the target's owning side |
| `+0x04` | **target city** |
| `+0x06` … | up to four **member cities** |

### City roles (`+0x56 + city`)

| role | meaning |
|---|---|
| 1 | just captured (set when the AI takes a city) |
| 6 | member of an assault group; vectors production to the group target |
| 7 | an assault group's target |
| 8 | stop producing here (set on own role-7 cities at the start of `assault`) |
| 2–5, 9–13 | set by the `evaluate` phase; meaning not decoded, but each maps to a production purpose (below) |

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

**Assault** (`ai_phase_assault`, `563e:0000`). Re-evaluates first
(`59bf:01b3`), turns its own role-7 cities into role 8, then runs each active
assault group (`563e:00ca`): give up if the side's capital has fallen and no
group targets its captor, otherwise gather the group's armies, move them at the
target and attack (`5ca7:023f`). A group that acts has its turn counter
incremented.

**Vectoring** (`ai_vectoring`, `5db9:085f`). The AI keeps a list of groups at
AI data `+0x24a` (count) and `+0x24c` (entries of `0x5c` bytes): a target city
at `+0x250` and up to four member cities at `+0x252`. For an active group whose
target the side still owns, the target is marked **role 7** and every member
city with **role 6** has its production vectored to it (`623c:0f10`).

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
| `threat[j] ≥ grudge + 5`, where `threat = ai[0x3ec+2j] + 4·ai[0x40c+2j] + 2·ai[0x3bc+2j]` | keep, at least intermediate |
| `j` is the enemy chosen by `ai_pick_enemy` | **war** |
| `j` owns an active assault group's target | **war** |
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
| AI data `+0x3bc+2j` | ×4 |
| AI data `+0x3cc+2j`, `+0x3dc+2j` | ×1 |
| AI data `+0x3ec+2j`, `+0x3fc+2j`, `+0x40c+2j` | ×2 |
| a constant for our own controller: human `+0x18`, else level 0 `+0x1c`, 1 `+0x1a`, 2 `+0x1e` | +1 |
| **&#124;our diplomatic score − `j`'s&#124; / 8** | +1 |
| &#124;our city count − `j`'s&#124; / 4 | +1 |

A term meant to weight enemy cities next to ours is **dead**: it accumulates
into `score[owner of our own city]`, which is always this side's own slot,
and that slot is zeroed before the pick.

A side's score is then zeroed if `558d:0a6e` vetoes it, if we already propose
war on somebody and have no proposal toward it, if there are no assault
groups and it has no unflagged cities, before **turn 8** (turn 4 with *Quick
Start*) unless `ai[0x3dc] + ai[0x3fc]` is non-zero, or if more than one other
side is in play and it is already the target of more than a quarter of its
own cities' worth of assault groups. The highest remaining score wins;
whoever holds our capital overrides that unless an assault group already
aims at them. A target with no unflagged cities is dropped.

**Debug output.** `5db9:0dc9` receives every phase string. It's a 5-byte stub
in the shipped game, so the debug log is compiled out. The `auto_str_*` AI
functions (`Scan dist …`, `Quest city … GOING !!`, `Legal site …`,
`Enemy city … combat …`) are leftover debug-format strings in the same style.
