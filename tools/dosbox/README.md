# Driving the original under DOSBox-X

`shoot.py` runs `WARLORD2.EXE` in DOSBox-X from a script of key presses,
clicks and screenshots, so the original can be captured without anyone at the
keyboard. `w2hook.asm` is the resident helper it loads first: F11 plays the
next scripted click through the game's INT 33h polls, and F12 asks DOSBox-X
for a screenshot through its integration device. Needs `nasm` and `dosbox-x`.

    python3 tools/dosbox/shoot.py myscript.txt out/ --limit 250

`scripts/start.txt` gets from the title screen into the Sirians' first turn.

**Status: experimental.** It works, but runs are slow (the game's setup alone
takes 40-90 s) and the timing varies from run to run, so a click that lands
before its screen is up is lost and the waits have to be generous. The
disassembly is the primary source for the interface; use this for spot checks.
