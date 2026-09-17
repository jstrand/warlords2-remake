# Computer players — structure

What's known about the AI in `WARLORD2.EXE`. The individual phases aren't
decoded. Several of them (production in particular) sit in code Ghidra
mis-disassembles, so reading them means working from raw disassembly as was
done for quests. A remake can reasonably write its own AI; this page is for
matching the original's behaviour where that matters.

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
| 18 | `production` | `5db9:06d4` | |
| 19 | `vectoring` | `5db9:085f` | |
| – | | `diplomacy_score_update` | see `docs/rules.md` › Diplomacy |

## AI data

Each side has a `0x42c`-byte block, held in a memory handle at
`DS:40ca + 4·side` and saved as block 10 of a save file
(`docs/formats/save.md`). Fields seen so far:

- `+0x56 + city`: a per-city **role** byte (values 2, 3, 7 seen)
- `+0x02`, `+0x0a`, `+0x0e`, `+0x10`, `+0x12`: used by *rebuilding*, below

A shared `0x4b0`-byte block follows the eight side blocks in a save.

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

**Production choice.** Probably uses `best_production_for` with purposes
1–6 (`docs/rules.md` › Starting garrisons); not confirmed, because
`5db9:06d4` doesn't disassemble cleanly in Ghidra.

**Debug output.** `5db9:0dc9` receives every phase string. It's a 5-byte stub
in the shipped game, so the debug log is compiled out. The `auto_str_*` AI
functions (`Scan dist …`, `Quest city … GOING !!`, `Legal site …`,
`Enemy city … combat …`) are leftover debug-format strings in the same style.
