# Reverse engineering `WARLORD2.EXE`

What's here, and where to look first.

| document | contents |
|---|---|
| [`../formats/exe.md`](../formats/exe.md) | the executable itself: Borland overlays, how `tools/exe.py` flattens it, the Ghidra import command, landmark addresses |
| [`labels.md`](labels.md) | how functions get names: the hand-written and generated label files, and coverage |
| [`dice_callers.md`](dice_callers.md) | every caller of `dice()`, the index to the game's random rules |
| [`runtime.md`](runtime.md) | segment 0: Borland runtime, SSG's assembly helpers, sound drivers |
| [`ai.md`](ai.md) | computer players: turn pipeline, AI data, decoded decisions |
| [`random_map.md`](random_map.md) | random map generator: pipeline and `RANDOM.DAT` parameters |

The **rules** that came out of all this live in [`../rules.md`](../rules.md),
and the file formats in [`../formats/`](../formats). `docs/rules.md` is the
one to read for a remake; this folder is about how it was established and
where to look to extend it.

## Decoded from the executable so far

Combat (bonuses, terrain classes, resolution), movement (terrain costs, stack
modes, bonuses), production and the start-of-turn sequence, city capture,
starting garrisons, heroes (offers, levels, experience, death), ruins,
temples and sages, quests (types, targets, completion, rewards), diplomacy,
game setup and difficulty, end-of-game conditions, the save layout, the
random map generator's terrain, and the AI's diplomacy, city roles,
production and garrisons.

## Still open

- The AI's remaining arithmetic: the per-side statistics at AI data
  `+0x3bc`…`+0x40c` that drive its diplomacy and target scores, and how
  `623c:13b2` chooses what a stack walks to.
- Graphics, sound and UI plumbing — deliberately skipped; the data formats
  are decoded in `docs/formats/`.
- A handful of small unknowns, listed at the end of `docs/rules.md`.

## Method notes

- **Work from `dice()` outwards.** Every random rule goes through it, so its
  call sites (`dice_callers.md`) locate combat, quests, ruins and the AI.
- **Names come cheap from text.** `CollectEvidence.java` + `tools/autolabel.py`
  name functions from the `STRING.DAT` groups and file names they reference.
- **The `.SCN` file *is* the live game state**, loaded verbatim into one
  segment, so scenario offsets and memory addresses are the same thing. Many
  "where is this stored" questions are answered by reading the shipped files.
- **Check Ghidra's output against the bytes.** Two bugs found this way: a
  byte-scan FPU rewrite that corrupted operands (fixed in `tools/exe.py`), and
  CS-relative jump tables that Ghidra resolved to garbage
  (`FixJumpTables.java`). Both silently produced plausible-looking nonsense.
- **When the decompiler fails, read the disassembly.** The quest rules were
  recovered that way before the jump-table fix, and later confirmed.
