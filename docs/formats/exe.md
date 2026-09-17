# `WARLORD2.EXE` — executable layout — **overlays SOLVED**

Borland C++ 3.x (1991), 16-bit real-mode MZ, with VROOMM overlays appended as
an `FBOV` block. `tools/exe.py` parses it and rebuilds a **flat, overlay-free
MZ** that rizin/Ghidra/IDA load as an ordinary DOS executable:

```sh
python3 tools/exe.py info    original/WARLORD2.EXE
python3 tools/exe.py flatten original/WARLORD2.EXE build/WAR2FLAT.EXE build/war2segs.json
rizin build/WAR2FLAT.EXE        # addresses are linear: seg*16 + off
```

`build/` is git-ignored: the flat EXE is derived from copyrighted code.

## File layout

```
0x000000  MZ header, 0x677 relocations, header size 0x2000
0x002000  load image (0x384D0 bytes)  entry 0000:0000, stack 383C:0080
0x03A4D0  FBOV header (16 bytes)
0x03A4E0  overlay code + fixups (0x4A3C0 bytes)
```

### FBOV header

| off | type | value | meaning |
|---|---|---|---|
| 0 | char[4] | `FBOV` | |
| 4 | u32 | 0x4A3C0 | size of overlay area |
| 8 | u32 | 0x310E0 | **file** offset of the segment table (not image-relative) |
| 12 | u32 | 137 | segment table entries |

### Segment table — 8 bytes per entry

`u16 seg, u16 size, u16 flags, u16 minoff`, ascending by `seg`.

| flags | count | meaning |
|---|---|---|
| 1 | 42 (+1 empty) | resident code — entries 0–41, segs `0000`–`197C`; entry 0 is the C runtime |
| 3 | 69 | overlay stub — entries 56–124, segs `2F54`–`3120` |
| 0 | — | data. Entry 125 `3125` is **DGROUP** (DS, size 0x7166). `2C04` and `1E1D` are large far-data segments holding game state. |
| 4 | — | BSS / stack pieces; `383C` is the stack |

### Overlay stub segment

```
+00  CD 3F           INT 3Fh (overlay manager trap)
+04  u32             code offset, relative to the byte after the FBOV header
+08  u16             code size
+0A  u16             fixup size (bytes)
+0C  u16             entry count
+20  entries, 5 bytes each:  CD 3F <u16 target offset> 00
```

Resident code calls an overlaid function as `CALL FAR stub:0020+5k`. At run
time the manager loads the code and patches the entry into
`EA <off> <seg>` (`JMP FAR`), which is exactly 5 bytes.

### Overlay fixups

Directly after each overlay's code: `fixup size / 2` little-endian `u16`
offsets into that code. Each names a **segment word**, and the word holds a
**segment-table index × 8**, not a segment value. Example: `B8 68 01 8E C0`
→ `mov ax, 0x168; mov es, ax` → entry 45 → segment `2C04`.

## What `flatten` does

1. Copies the load image unchanged, then appends each overlay's code at a new
   segment above the stack (the first is `384E`).
2. Replaces each fixup's `index*8` with the real segment and adds an MZ
   relocation for it.
3. Patches every stub entry into `JMP FAR` to the new location (+ relocation).
4. **Redirects far pointers**: any relocated `seg:off` that points at a stub
   entry (3220 of them — `CALL FAR` operands and far function pointers in data)
   is rewritten to point at the real function, so xrefs work.
5. **Decodes Borland's x87 emulation** in code segments (282 sites):
   `INT 34h..3Bh` → `FWAIT; D8h..DFh`, `INT 3Ch xx` → `ES:` + `D8h|xx&7`,
   `INT 3Dh` → `FWAIT`. This is a byte scan, so an immediate containing
   `CD 34..3D` would be corrupted. None has been seen yet.
6. Writes a new MZ header sized for all the relocations.
   `build/war2segs.json` maps each stub to its new code segment.

The flat EXE has not been run. It is meant for static analysis. It should
still execute, because every rewrite preserves behaviour, but nobody has
tested that.

## Landmarks found so far

Addresses are in the flat EXE, as `seg:off`.

| address | what |
|---|---|
| `0000:0000` | C0 startup, `mov dx, 3125h` → DS |
| `0000:200A` | `rand()` — Borland LCG `seed = seed*0x015A4E35 + 1`, returns `(seed>>16) & 0x7FFF` |
| `0000:4944` | `sprintf` |
| `070B:0006` | `open` (`070B:003A` close, `:006D` read, `:00A9` filelength) |
| `0D28:011F` / `:069A` / `:07A9` | memory: allocate handle / lock / unlock |
| **`6ECB:02BF`** | **`dice(n, sides, bonus)`** — the game's only RNG entry point, see below |
| `6ECB:0356` | `srand(time())`, called lazily by `dice` |
| `6ECB:03A0` | loads `DATA\STRING.DAT`, `ERROR.DAT`, `FILE.DAT` into memory handles `DS:5C34`, `5C2C`, `5C30` |
| `6BAB:0323` | scenario map loader (`sprintf "%s\%s.map"`) |
| `5087:0EFC` | AI target choice: 4 candidates, score `100 − distance + 1d20` each |

### `dice(n, sides, bonus)`

```c
int dice(int n, int sides, int bonus) {
    if (!seeded) { seeded = 1; srand(time(0)); }
    if (sides == 0) return bonus;
    int sum = 0;
    for (i = 0; i < n; i++)
        sum += (int)(rand() / 32768.0 * sides + 1.0);   /* 1..sides */
    return clamp(sum + bonus, n + bonus, n * sides + bonus);
}
```

There are **254 call sites in 96 functions**, and `dice` is the *only* caller
of `rand()`, so every random outcome in the game goes through it. That makes the call-site list the index to combat, AI, the random map
generator and quests. A call is easy to spot in the bytes:
`66 68 <sides:u16> <bonus:u16>  6A <n>  9A BF 02 CB 6E`.

## Ghidra project

```sh
/opt/homebrew/opt/ghidra/libexec/support/analyzeHeadless build/ghidra WAR2 \
    -import build/WAR2FLAT.EXE -scriptPath tools/ghidra -preScript SetupWar2.java
ghidraRun    # then File > Open Project > build/ghidra/WAR2.gpr
```

About 3 minutes; it finds ~1580 functions. `SetupWar2.java` sets `DS = DGROUP`
across the program, so `[xxxx]` operands resolve to named data, and then
applies `tools/ghidra/war2_labels.txt`. **Record new discoveries in that file**
and re-run the script (Script Manager, category *War2*) so they survive a
re-import. `CheckWar2.java` is a smoke test that prints the function count and
`dice` callers and decompiles a couple of functions.

**Ghidra loads the image at segment `1000`**, so add `0x1000` to every segment
in this document: `dice` `6ECB:02BF` is `7ECB:02BF` in Ghidra, and DGROUP is
`4125`. The labels file uses the flat-EXE form; the script converts it.

## Tooling notes

- rizin's linear `pd` sometimes **loses sync** after `66`-prefixed 32-bit
  pushes and prints `invalid`. Re-disassemble from the known instruction
  address (`pd N @ addr`) and it is correct. Ghidra's flow-following
  disassembly doesn't have this problem.
- rizin shows linear addresses as `6000:ef6f`-style normalised pairs. Treat
  them as linear.
