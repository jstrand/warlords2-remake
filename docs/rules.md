# Game rules

Sourced from the *Warlords II Deluxe* manual (Appendix C, "Combat Mechanics",
and chapter 8, "Movement"), cross-checked against the shipped data files and the
original game running in DOSBox. Deluxe was a later release with balance tweaks,
so treat exact constants as "very likely" rather than certain for Warlords II
proper — but every rule below that could be checked against the original's data
or UI **did** check out.

## Two values that are derived, not stored

This is why neither could be found in the scenario file.

**City ownership at scenario start.** Each side owns exactly its capital; every
other city is neutral. See `docs/formats/scenario.md`.

**City defence.**

```
tower            -> 1
special location -> 2
city             -> 1 if it produces fewer than 3 army types
                    2 if it produces 3 or 4
halved when the city being attacked is neutral
```

Verified against the running game: Mirea (4 types) → 2, Axbridge (4) → 2,
Largash (3) → 2, all displaying `Defence: 2`. Across Erythea's 80 cities this
gives 26 at defence 1 and 54 at defence 2. It also explains the help file's
`- Pillage -` → "Reduce defence for gold": pillaging removes production types.

## Combat

Armies each get **2 hit points**; survivors are healed at the end of a combat
(not of each individual fight). Attackers and defenders form two lines ordered
by the *fight order* (stored per side in `.SCN` at 1547).

Bonuses are gathered for each side:

- **MAX HERO STRENGTH** → **ATTACK HERO BONUS**: +3 if hero strength is 9,
  +2 if > 6, +1 if > 3
- **ATTACK MAX TERRAIN** — highest group terrain bonus (only the highest counts,
  so two Elephants still give −1)
- **ATTACK COMMAND** — sum of command items carried by heroes
- **ATTACK MAX SUBTRACT** — highest negative bonus
- **SIEGE** negates the defender's FORTIFIED bonus
- **NEGATE HERO** negates the defender's hero bonus
- **NEGATE NON HERO** negates the defender's MAX TERRAIN

```
ATTACK MODIFIER = ATTACK HERO BONUS   (unless defender negates hero)
                + ATTACK MAX TERRAIN  (unless defender negates non-hero)
                capped at 5, then reduced by the defender's MAX SUBTRACT
DEFEND MODIFIER = same, plus DEFEND FORTIFIED (unless attacker has siege)
```

Individual terrain bonuses (`ARMYTYPE.DAT +32..+38`) are added to army strength
and **cannot be negated**. Boats attacking boats or fliers fight at strength 4
or their natural strength, whichever is lower. **Strength is capped at 15.**

Resolution, army against army:

- both roll a **d20** (**d24** if the *Intense Combat* option is on)
- attacker hits if it rolls ≤ its modified strength **and** the defender rolls
  above his
- defender hits on the mirror condition; if neither, re-roll
- after 10000 throws with no hit, the defender wins by default

Because the die is d20 and strength caps at 15, a group of weak armies can
always beat one strong army.

The Military Advisor runs **20 simulated combats** with the real routine and
reports the success count.

## Movement

Each army type has **movement points**; each terrain type has a fixed MP cost
that is the same for all armies. An army with a **move bonus** for some terrain
pays the *plains* cost there instead, and **transmits that bonus to its whole
group** — one elf makes the entire stack cheap in forest.

- a group stops as soon as **one** member lacks the MPs for the next square
- up to **2 unused MPs** carry over to the next turn
- a **Hero travels at any army's movement cost**, so a flier can carry a hero
  over mountains
- max **8 armies** in one location
- cities and enemy armies must be attacked, never moved into

Terrain classes for combat are **CITY, WOODS, HILLS, OPEN** — where OPEN
includes water, shore and mountains.

## Still unknown

The manual states army move values live "in the Appendix", but the Deluxe
appendices are a scenario list, scenario notes and combat mechanics — no stats
table. Consistent with the manual's own note that army types and bonuses vary by
scenario and should be read "within the game itself". So the per-type movement
allowance is still unlocated (see `docs/formats/armytype.md`), as are the
per-terrain MP costs.
