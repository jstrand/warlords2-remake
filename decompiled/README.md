# Decompiled `WARLORD2.EXE`

Ghidra's C for every function it finds in `build/WAR2FLAT.EXE`, with the
names from `tools/ghidra/war2_labels.txt` and `war2_auto_labels.txt` applied.
This is a **generated snapshot** for reading and searching. Don't edit it:
record names in the labels files and regenerate (`docs/re/labels.md`).

- `index.txt`: one line per function: entry, name, size, call count, first
  bytes, callees. `grep auto_ui_ index.txt` lists the dialogs, for example.
- `<seg>_<off>.c`: the decompiled function at that entry.

Addresses are **Ghidra's**, i.e. the flat-EXE segment plus `0x1000`:
`dice` is `6ecb:02bf` in the labels file and `7ecb_02bf.c` here.

Decompiles are a reading aid, not ground truth. Check anything surprising
against the disassembly (`docs/re/README.md`, method notes).
