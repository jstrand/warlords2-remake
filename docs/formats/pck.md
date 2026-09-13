# `.PCK` — Warlords II image format — **SOLVED**

All 92 `.PCK` files in the shipped game decode exactly, consuming each file to
the last byte: 5,952,848 pixels, zero errors. Implementation: `tools/pck.py`.

## Container

```
u16 version    always 1
u16 width      pixels
u16 height     pixels
...            four compressed bitplane streams, back to back
```

The image is **4bpp — 16 colours**, matching the 16-entry `.PAL` files, stored
as **four separate 1-bit planes** of `(width/8) * height` bytes each. Plane *p*
supplies bit *p* of the colour index; within a plane byte the **MSB is the
leftmost pixel**. Each plane is compressed independently — the LZ77 window
resets at every plane boundary, which is why a uniform image produces four
byte-identical streams.

## Compression — LZ77 with a signed big-endian offset

Read a command byte. Its top bit selects the token type — which is really just
the **sign bit of a big-endian 16-bit offset**:

| Command | Meaning |
|---|---|
| `cmd < 0x80` | **Literal run.** The next `cmd + 1` bytes are copied out verbatim. |
| `cmd >= 0x80` | **Match**, 3 bytes total: `[cmd][mid][len]`. |

For a match:

```
offset   = (cmd << 8) | mid      big-endian, always negative (top bit set)
distance = 65536 - offset        1 .. 32768
length   = len + 1               1 .. 256
```

Copy `length` bytes from `distance` back, **one byte at a time**, so overlapping
copies replicate — distance 1 degenerates to RLE.

Reads before the start of the plane yield `0`. A match may therefore reference
the zero-filled window before any output exists, which is how uniform images
reach ~85:1 (the ceiling is 256 output bytes per 3 input bytes).

### Worked example — `START/BLACK.PCK` (640×480, all black)

Body is 1808 bytes = 4 identical 452-byte planes. Each plane:

```
00              literal run of 1 -> emits 0x00                         (1 byte)
ff ff ff        distance 1,   length 256  -> replicates it            (256)
ff 00 ff        distance 256, length 256  -> replicates prev block    (256)
...             x148 more
ff 00 fe        distance 256, length 255                              (255)
                                          total = 1 + 149*256 + 255 = 38400
```

`38400 = (640/8) * 480`. ✔

`PICS/STRAT.PCK` (224×312) is the other clean specimen: its matches walk the
distance backwards in steps of 256 (`ff ff`, `fe ff`, `fd ff`, …), each copying
from output position 0, giving `1 + 34*256 + 31 = 8736 = (224/8) * 312`. ✔

## Transparency and sprite sheets

Sprite sheets use a **colour-key**, not an alpha channel or a skip opcode.
`TERRAIN0/A?.PCK` (512×64 army sheets) key on **index 10** — the bright green
that fills 22620 of their 32768 pixels. Full-screen art such as `PICS/CURS.PCK`
and the `START/` screens uses index 0 (black) as its background instead, so the
key is per-asset convention rather than a fixed index.

Frame subdivision is implicit: `A?.PCK` is a 16×2 grid of 32×32 sprites. The
header carries only overall dimensions, so each sheet's cell size has to come
from convention or from the code that consumes it.

## Usage

```sh
python3 tools/pck.py <palette.PAL> <out_dir> <files...>
```

```sh
# terrain and sprites use the terrain palette
python3 tools/pck.py original/TERRAIN0/WAR2.PAL out original/TERRAIN0/*.PCK
# the intro screens use their own
python3 tools/pck.py original/START/LOGO.PAL out original/START/*.PCK
```

`tools/pck.load(path)` returns `(width, height, indices)` with one byte per
pixel (0–15) for use by the engine.

## Remaining questions

- Which palette pairs with which asset directory. `STANDARD.PAL` and
  `TERRAIN0/WAR2.PAL` are byte-identical; `START/LOGO.PAL` and
  `START/SCREEN0.PAL` differ and are presumably selected by the intro player.
- Whether `.GFX` files (`START/`, `HELP/`) use this same codec — worth trying
  `tools/pck.py` against them directly.
- Cell sizes for the sheets other than `A?.PCK`.
