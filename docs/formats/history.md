# `CURRENT.HST` — the game's history — **layout from the executable**

The History menu plays back what this file records. It is FILE.DAT group 6
(`CURRENT.HST`), rewritten as the game goes and carried inside a saved game
(block 9 of `docs/formats/save.md`). No sample ships with the game; the
layout comes from the writer, `6d51:0d60`, which runs once a turn.

## Layout

```
u32   total size of the file, this field included (starts at 4)
then one record per turn, turn 1 first:
  u16   size of the record, this field included
  8 × u16   each side's gold (side record +0x185)
  8 × u16   each side's score, as the Winning report reckons it (6ef3:0947)
  8 × u16   each side's city count (4125:5dea, counted by 828e:03f9)
  n × u8    each city's owner, 0xff for one that is no longer a city
            (its tile is not terrain 10); padded to an even count
  u16   size of the events that follow, in bytes (22 each)
  events, 22 bytes each:
    u8   side
    u8   type (below)
    u16  first value
    u16  second value
    16 × char  a name
```

The writer runs when the round wraps (`8065:17f6`), just after the turn
counter goes up: on turn 2 it first creates the file with the header alone,
so record *n* is the state at the end of turn *n*. Nothing is recorded from
turn 202 on.

## Events

Nothing is logged as it happens. Each side keeps its **two most important
deeds of the turn** in its 43-byte record at `2c04:13e3` (`6d51:1244`): a
count, and for each of two slots the type, two values and a name. A new deed
takes a free slot; with both full it replaces the one of higher type, if the
new one's type is lower — **a lower type counts for more**, so a side keeps
its two lowest. When the round ends `6d51:132f` takes every side's first
deed, then, side by side, second deeds while fewer than ten are taken, and
`6d51:1225` clears the slots. The events go out side by side, each side's
first before its second.

The type picks the wording (`6d51:1516`, its jump table at `6d51:16a4`;
STRING.DAT group 94):

| type | first value | second value | wording |
|---|---|---|---|
| 0 | city | | *%s emerges in %s* — the hero, the city |
| 1 | −1 / −2 / city | | *%s killed in battle* / *killed searching* / *killed in %s* |
| 2 | | | *%s completes quest* |
| 3 | | | *%s receives a quest* |
| 4 | side | | *%s vanquished!* |
| 5 | −1 or side | city | *%s won %s* — the name, or the side, and the city |
| 6 | item, or 100 / 101 / 102 | | *%s finds %s* — the item's name, or *some allies* / *a sage* / *some gold* |
| 7 | side | | *%s victorious!* |
| 8 | side | side | *Treachery by %s on* — attacking a side at peace (`67cc:2257`) |
| 9 | side | side | *%s at war with* |
| 10 | side | side | *%s at peace with* |

The callers of `6d51:1244`: `484e:0f15`/`1027` (war, peace, as proposals
land), `4976:0dae`/
`1da8` (quests), `6536:013c`/`02ef`/`0571`/`07a1` (searching), `67cc:0af5`/
`0ba1`/`0c5a`/`2257` (battles and cities), `7563:04e7` (a hero emerges),
`8065:19f1`/`1cff`/`1d91`/`1eaf` (the turn's end: sides out and the winner).
