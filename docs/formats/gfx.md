# `.GFX` — screen layout markup (not an image format)

Despite the extension, `.GFX` files are **plain ASCII markup scripts** that lay
out the credits, help and tutorial screens. They are not `.PCK` images and do
not use that codec.

Directives are `#` followed by a letter, one per line, with text arguments
delimited by `|` and numeric arguments in parentheses (fixed-width, zero-padded):

| Directive | Meaning |
|---|---|
| `#D001` | page / dialog id |
| `#H` | select heading style |
| `#T` | select body-text style |
| `#F015` | select font or colour index (here 15) |
| `#C(x,y)\|text\|` | draw text centred on x at y |
| `#L(x,y)\|text\|` | draw text left-aligned at x,y |
| `#G(x,y,w,h)NNN(sx,sy)` | blit graphic NNN into the rect at x,y,w,h from source sx,sy |
| `#E` | end of screen |

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
