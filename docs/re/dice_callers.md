# Triage of `dice()` callers

`dice(n, sides, bonus)` is the game's only source of randomness (see
`docs/formats/exe.md`). Ghidra finds **97 calling functions**. This page sorts
them so we know where combat, map generation, AI and rewards live.

**Addresses are Ghidra addresses** (flat-EXE segment + `0x1000`), because that's
where you'll be looking. `tools/ghidra/war2_labels.txt` uses flat addresses.

Regenerate the raw material (works while the GUI has the project open if you
point it at a copy):

```sh
analyzeHeadless <projdir> WAR2 -process WAR2FLAT.EXE -noanalysis -readOnly \
    -scriptPath tools/ghidra -postScript SetupWar2.java \
    -postScript DumpCallers.java dice build/ghidra/callers
```

`index.txt` lists each caller's call sites, the constant pushes and who calls
it. `<addr>.c` holds the decompiled function.

## Reading the arguments

The arguments alone classify many callers:

| call | range | usually means |
|---|---|---|
| `dice(1,100,0)` | 1–100 | percentage check (`< p`) |
| `dice(1,1000,0)` | 1–1000 | per-mille (rare event) check |
| `dice(1,N,-1)` | 0–N-1 | pick a random **index** from N |
| `dice(1,0x70,-1)` / `dice(1,0x9c,-1)` | 0–111 / 0–155 | random **map x / y** (map is 112×156) |
| `dice(1,0x66,5)` / `dice(1,0x92,5)` | 6–107 / 6–151 | map x / y with a 5-tile border |
| `dice(3,500,500)`, `dice(3,1000,1000)` | 503–2000 / 1003–4000 | gold amounts |
| `dice(1,20,0)` / `dice(1,24,0)` | | combat die |

## Confirmed (read and understood)

| Ghidra addr | name | what it does |
|---|---|---|
| `67cc:08a6` | `combat_resolve` | **The combat loop.** See `docs/rules.md` › Combat. d20, or d24 with Intense Combat. Returns 1 if the attacker wins. |
| `67cc:0000` | `attack_tile(x, y)` | Checks diplomacy, sets up both sides, calls `combat_resolve`, shows the result (`STRING.DAT` groups 141–146), takes the city, then calls `67cc:2274` |
| `6087:0efc` | `ai_choose_target` | 4 candidates scored `100 − distance + 1d20` |
| `66d4:0000` | `setup_random_sites` (provisional) | Walks the **site** table (`2c04:0811`, 31-byte records, count `2c04:080f`; see `docs/formats/scenario.md`) and assigns random contents. Called by `79fa:0000`. |

Globals behind `combat_resolve` (DS-relative, flat DGROUP `3125`):

| DS off | Ghidra | meaning |
|---|---|---|
| `429c` / `42a4` | `451b:033c` / `:0344` | attacker / defender strength, `char[8]` |
| `4274` / `427c` | `451b:0314` / `:031c` | attacker / defender hits left (start 1 = 2 HP) |
| `4224` / `422c` | `451b:02c4` / `:02cc` | attacker / defender alive flags |
| `424c` | `451b:02ec` | attacker army type ids |
| `42c4` / `42c5` | `451b:0364` / `:0365` | attacker / defender counts |
| `41b5`, `41b4` | `451b:0255`, `:0254` | fight log (who died), log length |
| `42c6` | `451b:0366` | far pointers to the attacking armies (type at +4) |

Segment `2c04` (Ghidra `3c04`) **is the `.SCN` file loaded verbatim**, so its
offsets are file offsets. Option words start at `0x11a`, one `u16` per entry in
`STRING.DAT` group 4: `011a` Neutral Cities, `011c` Diplomacy, … `0126` Intense
Combat, … `012c` Random Turns. `012e` is a hidden eleventh setting that's 1 only
in `TUTORIA.SCN`. `0110` is the current player, `0112` the combat modifier cap
(5).

## Inferred from arguments (not yet read)

| Ghidra addrs | evidence | likely |
|---|---|---|
| `4d71:03af`, `4d71:0495`, `4e47:00b5`, `4eb7:005d`, `4f5f:0044`, `4fef:0a4b`, `4fef:113d` | random map x/y over the whole map | **random map generator** / random placement. The `4d71`–`4fef` segments form one cluster. |
| `4fc9:010a`, `513d:0331`, `513d:0a77` | map x/y with a 5-tile border | placing cities or sites away from the edge |
| `513d:003a`, `513d:0689`, `513d:1104`, `513d:1171`, `513d:1468`, `513d:161d`, `513d:1b3f`, `513d:1c1d` | many `1d10-1`, `1d100 > 79`, all called from `513d:0ce0` | one big generator driven by `513d:0ce0`; probably the same map/scenario generator |
| `6536:01ab`, `6536:0b1a`, `5e97:0080`, `7563:0000` | `3d500+500`, `3d1000+1000`, `1d400+300`, `1d600+1000` | **gold rewards**: ruin search / treasure, `7563` maybe quests |
| `5f19:06a0`, `5311:0e22` | `1d1000` checks | rare random events |
| `67cc:2274` | runs after every attack; shows STRING.DAT groups 158–162 | **medal awards** (`auto_ui_medal_effect`): `1d100 < p` chance, then +strength or +movement |
| `67cc:20a8` | `1d100`, `1d15`; shows STRING.DAT group 111 (war declared); called from `67cc:124a` | AI-side attack handling; `67cc:124a` is the "you are being attacked" path (`auto_ui_being_attacked`), which also calls `combat_resolve`. The Military Advisor is `67cc:1f19` (`auto_ui_advisor`). |
| `79fa:0a75` | eight `1d100` checks; `79fa:0000` → `66d4:0000` | game/scenario start setup |
| `5311:01f6`, `5311:03c3`, `5311:08f6` | `1d15+1` (2–16) | pick a random army type or player |
| `834b:2785`, `834b:232b` | `1d3+1` ×4 | hero or item stat generation |
| `59bf:0d7b` | `1d4`, `1d8`, `1d10`, `1d6`, variable-sided | random name / sign / flavour text? |

Everything else is still untriaged. Each has only one or two calls, so reading
`build/ghidra/callers/<addr>.c` is quick.

## Next useful reads

1. `67cc:1f19` (`auto_ui_advisor`): confirm the Military Advisor simulation
   and how its odds map to `STRING.DAT` group 126.
2. The code that fills attacker/defender strength (`451b:033c`/`:0344`) before
   `combat_resolve`. That's where the hero, terrain, command and fortification
   bonuses from `rules.md` get applied, so it's the real prize.
3. `67cc:2274` for the medal rules.
