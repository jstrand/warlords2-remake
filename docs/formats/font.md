# `.FNT` and `.FIN` — the proportional fonts

Three fonts ship in the game's root and are loaded together at startup by
`78a8:0000`: `TEXT`, `CHANCE17` and `CHANCE36`. Each is a pair.

**The `.FNT` is a `.PCK` image** — an ordinary glyph sheet, no new codec. It
decodes with `tools/pck.py` and `love2d/warlords/pck.lua` unchanged.

| font | sheet | line height | baseline | rows | copies |
|---|---|---|---|---|---|
| `TEXT` | 320 × 135 | 15 | 12 | 9 | 3 |
| `CHANCE17` | 416 × 102 | 17 | 12 | 6 | 2 |
| `CHANCE36` | 560 × 148 | 37 | 25 | 4 | 1 |

Colour 3 is the background, 15 the glyph and 0 its outline. One full set of
glyphs takes three rows (four for `CHANCE36`), so a taller sheet holds
**several stacked copies** of the whole set. In `TEXT` the copies differ in
colour — one is drawn in 15 and one in 4, 3385 pixels each.

## `.FIN` — the metrics

| off | type | meaning |
|---|---|---|
| 0 | u8 | glyph count (`0x60` = 96) |
| 1 | u8 | first character (`0x20` = space) |
| 2 | u16 **big-endian** | sheet width, matching the `.FNT` header |
| 4 | u8 | line height |
| 5 | u8 | baseline / ascent |
| 6 | 3 × u16 | small spacing values, **not decoded** |
| 12 | 96 × u8 | **glyph widths** |
| 108 | 96 × u8 | a second, byte-identical width table |
| 204 | 17 bytes | trailer, not decoded |

**The width table starts at character `first + 1`, not `first`.** The space has
no glyph in the sheet at all, and `width[0]` is the width of `!`. Getting this
wrong shifts every character by one — text renders as clean, correct-looking
glyphs spelling the wrong word, which is a quiet enough failure to be worth
naming.

So: `width(c) = table[c - 33]` for `c >= 33`.

## Glyph layout

Glyphs are packed left to right into rows `lineHeight` tall. Each takes a slot
**rounded up to a multiple of 8** — the art is 1-bit planar, so glyphs start on
byte boundaries. A glyph that does not fit in what is left of a row starts the
next one.

That reproduces the shipped sheets almost exactly: rows come out filled
560/560, 416/416 and 320/320, with 24 pixels wasted in one row of `CHANCE36`
and 8 in one of `CHANCE17`. Verified by overlaying the computed boxes on the
decoded sheets, and by rendering text.

The **last entry has no glyph**: 96 counted, 95 drawn. Characters 32–126 is
exactly 95, so entry 96 would be character 127. Including it pushes every
sheet one row past its actual height, which is the check that it is not there.

## Still open

- The three u16 values at `+6`.
- **The space's advance width**, which is in none of the above.
  `love2d/warlords/font.lua` uses a quarter of the line height, which matches
  the shipped screens closely but is a guess.
- What distinguishes the two identical width tables, and the 17-byte trailer.
