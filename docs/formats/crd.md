# `CARDS/*.CRD`, `*.DSC` — computer characters — **SOLVED**

Each computer side plays with a **character card**: a fixed record of AI
settings, picked on the setup screen's character dialog (dialog 27). The
level picks the deck: `K` Knight, `L` Lord, `W` Warlord, nine cards each,
`000` to `008` (`CARDS\%c%03d.%s` with `"KLW"`, `7bab:21c1`). `000` is the
*Standard* card of its level.

- `.DSC` is text: the character's name on the first line, then a few lines
  of description, `\r\n`-separated.
- `.CRD` is a packed 103-byte (`0x67`) record of little-endian `u16`s, many at
  odd offsets.

## Which card a side plays

The card number is `.SCN` `0x00e0 + 2·side`. It is reset to 0 whenever the
side's level button is pressed (`7bab:0a4e`) and by *I am the Greatest* for a
side that wasn't already a Warlord (`7bab:0ee8`). *Random Characters*
(`7bab:2051`) gives each computer side `1d(n − 1)` with `n` the number of cards
of its level on disk: any card **but** the Standard one.

## Loading (`59bf:0d7b`, from `ai_init_side` `59bf:084d` at game start)

The level first fills the side's AI data (`docs/re/ai.md` › AI data) from
built-in defaults. Then, for a side in play whose controller is computer, the
card is read (`7715:006a`) and **overwrites all of them**. The built-in
defaults are therefore only ever given to human sides, whose AI data nothing
reads. `W000` matches the built-in Warlord values, apart from the rebuild
type above 2000 gold (9, built in 0) and the three level bonuses.

| offset | → AI data | meaning (reader) |
|---|---|---|
| `0x08` | `+0x24a` | assault groups at once (`563e`, `5f19`, `5db9:085f`) |
| `0x0a` | `+0x0c` bit 0 | **bold**: may attack a side it is at peace or uneasy with, when at war with nobody else (`558d:0851`). A card can only set it; the Lord and Warlord defaults already have it |
| `0x0e`–`0x2a` | `.SCN 0x60b + 29·side` | the side's **fight order**, 29 army types |
| `0x2b` | `+0x48` | **cautious**: no *quick attack* phase (`5e97:04ba`), one army more before attacking a neutral (`57ea:03aa`, `0b19`), no follow-up from a newly taken neutral (`57ea:06c7`) |
| `0x2d` | `+0x10` | army type *rebuilding* buys below 2000 gold (`5db9:0af2`) |
| `0x2f` | `+0x12` | army type *rebuilding* buys from 2000 gold |
| `0x31` | `+0x0a` | *rebuilding*'s city limit, `max(5, v × cities / 80)` |
| `0x45` | `+0x26` | ‰ chance an assault group **razes** what it takes (`5f19:06a0`) |
| `0x47` | `+0x28` | ‰ chance it **sacks** |
| `0x49` | `+0x2a` | ‰ chance it **pillages** |
| `0x4b` | `+0x2c` | added to all three per city the side owns |
| `0x4d` | `+0x2e` | added to all three for a human side (never, for a computer) |
| `0x4f` | `+0x34` | added to all three for a Knight |
| `0x51` | `+0x32` | added to all three for a Lord |
| `0x53` | `+0x30` | added to all three for a Warlord |
| `0x55` | `+0x18` = `1dv` | `ai_pick_enemy` constant for a human side |
| `0x57` | `+0x1c` = `1dv` | … for a Knight |
| `0x59` | `+0x1a` = `1dv` | … for a Lord |
| `0x5b` | `+0x1e` = `1dv` | … for a Warlord |
| `0x5f` | `+0x36` | added to the sack and pillage chances while the side has under 100 gold |
| `0x61` | `+0x42` | **early vengeance**: before turn 10, a human's city taken is sacked (worth 200+) or razed (`5e97:0000`) |
| `0x63` | `+0x44` = `10v + 1d10` | % of all cities a **human** must hold to be declared war on (a computer: 50) (`558d:0000`) |
| `0x65` | `+0x38` | **computer solidarity**: with *Diplomacy* on, won't make war on a computer that is itself at war with a human (`558d:0a6e`, `DS:40f2`) |

The other bytes (`0x00`–`0x07`, `0x0c`, `0x33`–`0x43`, `0x5d`) are never read:
`59bf:0d7b` is the only reader of the record, and it skips them.

A group's raze/sack/pillage roll is made once, when the group is formed:
raze first, then sack, then pillage, each `1d1000 < chance + bonus`; the
first that succeeds is the group's (`group +0x5a` bits 0, 1, 2). Raze needs
at least two cities left in the plan and never hits a capital
(`563e:0f1b`).

## The decks

Rebuild types are army-type indices. `dv` columns are dice sizes.

| card | groups | bold | cautious | raze/sack/pillage ‰ | per city | own-level bonus | poor | early | human % | solidarity | name |
|---|---|---|---|---|---|---|---|---|---|---|---|
| K000 | 1 | – | yes | 20/0/0 | 0 | 0 | 0 | – | 8 | – | Standard Knight |
| K001–K008 | 1 | – | yes | 20/0/0 | 0 | 0 | 0 | – | 8 | – | *differ from K000 only in rebuild types and fight order* |
| L000 | 3 | – | – | 1/2/3 | 1 | 0 | 0 | – | 5 | – | Standard Lord |
| L002 | 2 | yes | – | 14/100/100 | 100 | 100 | 100 | yes | 1 | – | Roland the Rabid |
| L004 | 2 | – | – | 0/50/100 | 1 | 0 | 100 | – | 5 | – | Rebecca the Rapacious |
| W000 | 4 | yes | – | 5/10/20 | 5 | 0 | 50 | yes | 3 | yes | Standard Warlord |
| W001–W008 | 4 | yes | – | 4–5/8–12/16–20 | 1–5 | 0 | 20–50 | yes | 3 | yes | Attila the Hun … William the Wily |

The *bold* column is the card's own bit; Lords and Warlords are bold
whatever it says, since their level's defaults set the bit first.

## What a level changes, in sum

Nothing in the rules: combat, production and movement never read the level.
All of its effect is the card: how many assaults run at once, whether it
makes peace, how careful it is with neutrals, whether it razes or sacks what
it takes, how soon it turns on a human who is winning, and whether computers
stand together against a human. The `ai_pick_enemy` constant (`0x55`–`0x5b`)
is added to every candidate alike, so it doesn't change the pick.
