# `.GFX` — screen layout markup (not an image format)

Despite the extension, `.GFX` files are **plain ASCII markup scripts** that lay
out the credits, help and tutorial screens. They are not `.PCK` images and do
not use that codec.

Directives are `#` followed by a letter, one per line, with text arguments
delimited by `|` and numeric arguments in parentheses (fixed-width, zero-padded):

| Directive | Meaning |
|---|---|
| `#D001` | page number: the help box uses popup 14 + n (`7ecb:062a`) |
| `#H` | heading style: font 1 (CHANCE36), colour 15 |
| `#T` | body style: font 2 (CHANCE17), colour 15 |
| `#F015` | font 2 in colour 15 (any palette index) |
| `#C(x,y)\|text\|` | draw text centred on x, y the top |
| `#L(x,y)\|text\|` | draw text from x |
| `#R(x,y)\|text\|` | draw text ending at x |
| `#G(x,y,w,h)NNN(dx,dy)` | blit the w × h at (x, y) of bitmap NNN (the `BUTTON.DAT` bitmap table) to (dx, dy) |
| `#E` | end of screen |

Every position is relative to the corner of the box the page is drawn in
(`7ecb:06de` adds it). Other letters are skipped. The in-game help box
(`8065:168d`) puts a page in popup 14, (256, 40) 352 × 340, or popup 15,
(32, 40), for `#D001`; the control panel's help button (`8065:16f5`) always
uses popup 4, (120, 50) 400 × 360.

Example (`START/ART.GFX`):

```
#H
#C(240,002)|Art Director|
#T
#C(240,042)|Nick Stathopoulos|
#E
```

Example with a graphic (`TUTORIA/THERO.GFX`):

```
#D001
#H
#C(168,005)|Heroes|
#G(384,030,032,030)016(016,053)
#F015
#L(048,050)|The Hero is one of the most important|
```

Found in `START/` (credits), `HELP/` (help pages) and `TUTORIA/` (tutorial).
Reimplementing this is a small parser plus a text renderer — no reverse
engineering required. Coordinates are in the game's 640×480 screen space.
