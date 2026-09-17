# `<scenario>.ITM` — magic item pool — **SOLVED**

A plain-text list of the magic items a scenario can hand out. Every shipped
scenario ships a byte-identical copy (1018 bytes, 39 items), including
`RANDOM/RANDOM.ITM`.

```
39\r\n                                 count
Firesword            1 1\r\n           39 lines, each exactly 24 bytes
Icesword             1 1\r\n
...
```

Each item line is fixed width (`load_item_pool`, Ghidra `66d4:04ef`):

| bytes | meaning |
|---|---|
| 0–19 | name, space-padded. **`_` becomes a space**, and the first space ends the name |
| 20 | space |
| 21 | **type** digit (see `docs/rules.md` › Item types) |
| 22 | space |
| 23 | **value** digit |

The shipped pool has 39 items: swords and helms (type 1, battle), crowns and
banners (2, command), wings and brooms (5, flight), boots and steeds (6,
double movement) and purses (7, gold per city).

## How it's used

At game start `setup_random_sites` loads the pool and **fills item records
8–21** of the live scenario image with 14 random, non-repeating entries from
it (`docs/rules.md` › Ruins). The `.SCN` file's own names for those slots are
overwritten; only records 0–7, the sides' standards, survive.

The first `sites × 2 / 10` of those slots are filled with **reserved** items
(flight, double movement, standards, and command items worth 2+; see
`item_reserved`) and the rest with ordinary ones. That split decides where
they can be hidden: reserved items only go into "rich" ruins, ordinary items
only into ordinary ones.
