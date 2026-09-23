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

Read by `78a8:03ea`, a byte or a big-endian word at a time:

| off | type | meaning |
|---|---|---|
| 0 | u8 | glyph count, *n* (`0x60` = 96) |
| 1 | u8 | first character (`0x20` = space) |
| 2 | u16 **big-endian** | sheet width, matching the `.FNT` header |
| 4 | u8 | line height |
| 5 | u8 | baseline / ascent |
| 6 | u8 | 0, 1 or 2 — not read by anything found yet |
| 7 | u16 big-endian | **rows per copy** of the glyph set: 3, 3, 4 — times the line height, the height of one copy |
| 9 | u16 big-endian | **number of copies**, *k*: 3, 2, 2 |
| 11 | *n* × u8 | **ink widths**, from the first character |
| 11 + *n* | *n* × u8 | **advances**, from the first character |
| 11 + 2*n* | *k* × 3 × u16 BE | each copy's colours: glyph, outline, ground |

**The ink widths** are how wide each glyph's art is: the sheet is packed by
them, and the space, which has no glyph, is 0. **The advances** are how far the
pen moves on, and are what text is measured by. They are usually the same; a
letter that overhangs its neighbour has a smaller advance — CHANCE17's `T`
is 18 wide and moves on 13, which puts its bar over the "u" of "Turn" at the
top of the screen — and the space has its width here and only here: 8, 7 and
15. (An earlier reading took both tables one byte late, which made the two
look identical and left the space unknown.)

The colour triples say what each stacked copy is drawn in: `TEXT` has (15, 0,
3), (0, 3, 3) and (4, 3, 3); both `CHANCE` fonts (15, 0, 3) and (0, 3, 3).
`78a8:06ae` keeps these as a cache: asking for a font in colours no copy
already has recolours one through `78a8:0839`.

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

- The byte at `+6`.
