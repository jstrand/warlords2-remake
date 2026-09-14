# `HELP/WARLORD2.HLP` — context help — **SOLVED**

A flat table of the tooltip the game shows for every button, list control and
menu item. Parser: `tools/helpfile.py`.

```
0     u16   record count (388)
2     388 x 104-byte records
```

Record:

| Offset | Type | Meaning |
|---|---|---|
| `+0` | `u16` | help id |
| `+2` | `u16` | sub-id for repeated controls; 0 normally, 65535 on placeholders |
| `+4` | `char[50]` | title, e.g. `"- Raze City -"` |
| `+54` | `char[50]` | description, e.g. `"Permanently ruin city"` |

`2 + 388 × 104 = 40354` — the exact file size.

The **sub-id** keys controls that repeat: ids 336–343 are eight
`- Select Army -` buttons with sub-ids 37–44; ids 430–456 are twenty-seven
`- Swap Armies -` buttons with sub-ids 45–71. Lookup is therefore by
`(id, sub_id)`, not id alone — records are **not** in id order, and one id
appears twice. Ids 224–241 are empty placeholders carrying sub-id 65535.

## Why it is useful

It is effectively a **UI specification**: 388 named actions covering every
screen, which doubles as a build checklist for the engine's interface. It also
states rules in passing that are otherwise only inferable:

| Entry | Rule it states |
|---|---|
| `- Pillage -` | "Reduce defence for gold" — city **defence is mutable at runtime** |
| `- Sack -` | "Destroy defences for gold" |
| `- Raze City -` | "Permanently ruin city" |
| `- Build Production -` | "Buy new army types for a city" — confirms the 4th empty production slot |
| `- Intense Combat -` | "Bias combat towards large stacks" — a combat-model option |
| `- Neutral Cities -` | "Set difficulty of neutral cities" |
| `- Produce Allies -` | "Let cities produce ally armies" |
| `- Military Advisor -` | "Speculates on combat results" |
| `- Vector Production -` | armies produced can be given a standing destination |

## Usage

```sh
python3 tools/helpfile.py original/HELP/WARLORD2.HLP          # all entries
python3 tools/helpfile.py original/HELP/WARLORD2.HLP pillage  # filter
```

The three `HELP/*.GFX` files are screen-layout markup, not images — see
`docs/formats/gfx.md`.
