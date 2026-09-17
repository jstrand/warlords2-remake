# Function labels

Names in the Ghidra project come from two files in `tools/ghidra/`, both in
flat-EXE `seg:off` form (Ghidra adds `0x1000` to the segment):

| file | written by | trust |
|---|---|---|
| `war2_labels.txt` | hand, after reading the code | confirmed. May carry a `\| C prototype`. |
| `war2_auto_labels.txt` | `tools/autolabel.py`, **generated** | provisional; every name starts with `auto_` |

`SetupWar2.java` applies the generated file first and the hand-made file second.
A generated name only replaces a default `FUN_` name or an older `auto_` name, so
it never overwrites real work. **To confirm an `auto_` name, copy the line into
`war2_labels.txt` with a proper name** instead of editing the generated file.

## Regenerating

With the Ghidra GUI closed (the project must not be locked):

```sh
G=/opt/homebrew/opt/ghidra/libexec/support/analyzeHeadless
$G build/ghidra WAR2 -process WAR2FLAT.EXE -noanalysis -readOnly \
   -scriptPath tools/ghidra -postScript CollectEvidence.java $PWD/build/ghidra/evidence.tsv
python3 tools/autolabel.py build/ghidra/evidence.tsv > tools/ghidra/war2_auto_labels.txt
$G build/ghidra WAR2 -process WAR2FLAT.EXE -noanalysis \
   -scriptPath tools/ghidra -postScript SetupWar2.java
```

## How `auto_` names are chosen

`CollectEvidence.java` records, per function:

- **text lookups:** calls to `get_string` / `get_file_string` /
  `get_error_string` (`string_lookup` tables 0 / 1 / 2 = `STRING.DAT` /
  `FILE.DAT` / `ERROR.DAT`, API at `6ecb:052f`) with a constant
  group. 365 of the 385 calls Ghidra knows qualify (10 sit outside any function, 10 pass a computed group).
- **DGROUP strings:** `push ds; push offset` and `push dword 3125:offset`
  pairs that point at a printable string.

`autolabel.py` picks one name per function, first match wins:

1. `auto_file_*`: a data file it names (`FILE.DAT` entry or literal path)
2. `auto_ui_*`: its most-used `STRING.DAT` group, named by the hand-written
   topic table `UI_GROUPS` in `autolabel.py` (one entry per group)
3. `auto_sound_*`: a sound or advisor-voice file
4. `auto_err_*`: an `ERROR.DAT` message
5. `auto_str_*`: the first other literal string with real words (often debug
   or error text, e.g. `auto_str_no_room_ai_memory`)

The comment on each generated line shows the evidence.

## Coverage (September 2026)

| | functions |
|---|---|
| hand-named (`war2_labels.txt`) | 350 |
| generated (`auto_`) | 145 |
| still `FUN_` | 1072 |
| thunks / fragments | 3 |
| **total** | **1570** |

The rest mostly shows no text and names no files: drawing, map and path
logic, AI arithmetic. Those need reading (step 3), and the neighbours of
named functions are the natural place to start. Known gaps in the evidence:

- a byte scan finds 377 `get_string` calls but Ghidra only has 294 as
  references. The rest sit in code the auto-analysis didn't disassemble.
- 27 `push bp; mov bp,sp` prologues after a return aren't functions in
  Ghidra yet.
