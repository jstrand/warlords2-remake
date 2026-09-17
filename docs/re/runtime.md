# Segment 0 — C runtime and low-level helpers

Flat segment `0000` (Ghidra `1000`), 0x52A9 bytes, 204 functions. **194 are
named** in `tools/ghidra/war2_labels.txt`. The other 10 are empty stubs and
jump-table fragments.

It holds three unrelated things that the linker put side by side:

| range (offsets) | what | how identified |
|---|---|---|
| `0000`–`027a`, `0e4d`, `10c3`–`52a8` | **Borland C++ 3.1 runtime** (large model) | code shape, DOS/BIOS calls, known Borland helper conventions (`F_LDIV@` with CX = mode, struct push/copy with CX = size) |
| `0543`–`0b94` | **SSG's hand-written assembly helpers** | raw DOS calls, NOP-padded jumps, return-code style (`1`/`0`), not Borland code |
| `02b5`–`04a5`, `0c4e`–`0e15` | **DIGPAK / MIDPAK** sound-driver wrappers | `INT 66h` with `AX = 068xh` (DIGPAK) or `07xxh` (MIDPAK); driver load checks the `"DIGPAK"` / `"MIDPAK"` file signature |

`main` is `0a4c:000b` (Ghidra `1a4c:000b`); `_start` calls it.

## Things worth knowing when reading game code

- **The game barely uses stdio.** File access goes through SSG's
  `dos_load_file` / `dos_read_huge` / `dos_seek` / `dos_close`, plus the
  `open` / `read` / `close` wrappers at `070b`. `sprintf` is the most-called
  runtime function (207 calls), then `strcpy` (80).
- **Most heavy use:** `__F_SPUSH` (99 calls) pushes a struct by value, and
  `__F_SCOPY` (55) copies one. Treat them as struct assignment.
- **Long arithmetic** goes through `__F_LDIV`, `__F_LMOD` and `__N_LXMUL`,
  with operands in registers, so decompiled long division often looks like
  a plain call with odd arguments.
- **Floating point** exists but is rare: `dice`, the printf/scanf float paths
  and `sqrt`. It came through the x87 emulator interrupts decoded by
  `tools/exe.py`.
- `set_timer_isr` (`0894`) hooks INT 08h and reprograms the PIT. That's the
  game's timer for animation and music pacing.

## Prototypes in Ghidra

Functions with a `| prototype` in the labels file get a real signature. Two
16-bit quirks are handled in `SetupWar2.java`:

- **Data pointers are 4-byte far pointers**, but the program's default
  pointer size is 2, so every pointer in a prototype is widened.
- **4-byte return values** (`long`, far pointers) come back in **DX:AX**.
  Ghidra's 16-bit conventions have no such slot and would invent a hidden
  return-pointer argument, so those functions get custom storage instead:
  DX:AX return, stack arguments from `[SP+4]`.

Only the functions whose arguments I checked in the disassembly have
prototypes. Adding more is safe as long as far pointers are written as
pointers and `long` arguments as `long`.

## Uncertain names

- DIGPAK `0694` and MIDPAK `070c`, `070d`, `0710`: named only by function
  number. `070d` is probably *SetRelativeVolume*.
- `digpak_sound_status` / `digpak_massage_audio` / `digpak_play2` follow the
  DIGPAK numbering (`689h`/`68Ah`/`68Bh`) as I remember it; check them against
  the DIGPAK documentation before relying on them.
- Floating-point formatting internals (`__realcvt`, `__xcvt`, `__scantod`,
  …) are named by role; nothing in the game depends on their details.
