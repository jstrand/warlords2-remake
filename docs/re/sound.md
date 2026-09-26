# Music, effects and the advisor

What `WARLORD2.EXE` plays and when. The files are described in
[`../formats/sound.md`](../formats/sound.md); the engine's side is
`love2d/warlords/cues.lua` (the choices), `love2d/sound.lua` (playback) and
`love2d/ui/advisor.lua`. Addresses are Ghidra's (segment + 0x1000) unless
they are labels in `tools/ghidra/war2_labels.txt`, which use the file's.

## Layers

| where | what |
|---|---|
| segment 0 | the DIGPAK (`INT 66h`, `AX = 068xh`) and MIDPAK (`07xxh`) calls ([`runtime.md`](runtime.md)) |
| `255e` | the sound manager: `sound_init` reads `SOUND.DAT` and loads the drivers; samples are loaded into 11-byte slots at `451b:3108`; one song is loaded at a time into a 10 000-byte buffer |
| `7dda` | what the game calls: `music_cue` (`7dda:0000`), a function per effect, `advisor_speak` (`7dda:026f`) |

The switches are words: `4125:0bee` Music, `4125:0bec` Effects, `4125:26c8`
Speech **off**. Music and Effects are forced off when their driver did not
load; `4125:37a4` says whether there is a digital driver for the voice.

## The music

`music_cue(n)` (`7dda:0000`). With Music off it does nothing. Asked for the
cue already playing, it lets it play on — unless the song has ended, when it
starts it again (that is the computer's cue, the only one that does not
loop). Otherwise it stops what is playing, looks the song up in
`FILE.DAT`, loads the file with the driver's prefix (`SINT12.XMI`) and plays
it; a looping song is restarted by `sound_poll` (`255e:02ab`) from the main
loop when it ends. A cue plays until another replaces it.

| cue | `FILE.DAT` | songs | played by |
|---|---|---|---|
| 0 | 8 | `STARTUP` | the start screens (`7f77:0000`), setting a game up (`7bab:0000`) |
| 1 | 10, random | `INT0 4 6 9 10 16 17 23` | a human's turn once it opens and no hero offers (`8cc6:04bd`); a game loaded (`7721:02d3`); Music switched on; after a medal |
| 2 | see below | | each computer turn (`8065:2123`); **once**, no loop |
| 3 | 9, random | `INT12 21 12` | a quest done, on a human's turn (`quest_check`, `4976:1ded`); Congratulations (`8065:1fbd`) |
| 4 | 12 | `INT11` | a hero offers to join (`8cc6:04bd`) |
| 5 | 13, random | `INT1 8` | a temple (`4976:0000`) |
| 6 | 14 | `INT14` | a sage (`6536:0aa0`) |
| 7 | 15 | `INT15` | a hero's promotion, on a human's turn (`7563:0672`) |
| 8 | 16 | `INT18` | an army wins a medal after a battle (`67cc:2274`) |
| 9 | 17 | `INT19` | the computers offer to surrender (`8065:1f68`) |
| 10 | 18 | `INT20` | the offer refused (`8065:1ecd`) |
| 11 | 19 | `INT22` | the war begins (`start_game_from_setup`, `7bab:0cfe`) |

**Cue 2.** If any side still in play is human (`.SCN` 0x137 set and 0xd0
clear) the song is one of group 11, `INT2 3 5 7 13`. In a game of computers
alone, `dice(1,100)`: below 6 `INT11` (group 12), below 53 group 11, else
group 10.

"Random" is `string_lookup`'s rule (`7ecb:052f`): a negative index is
`dice(1, count, -1)`. **The picks draw on the game's one random stream** — so
in the original, turning the music on changes what happens later in the
game. The engine gives the music a stream of its own.

The medal cue and cue 1 after it bracket the medal dialog; promotion is
followed by the turn's own cue a moment later, as `8cc6:04bd` goes on.

## The effects

One sample plays at a time. `sample_play` (`255e:065e`) first **waits for
the one sounding to end** (`255e:0736`), and some callers then wait for their
own to end too, so the game stops while they play.

| sample | function | played by |
|---|---|---|
| `TURN` | `sound_turn` | the turn's banner (`8cc6:0259`), human turns only; the advisor waits for it |
| `WAR` | `sound_war` | the fire cloud over a battle (`67cc:1836`), when a human attacks; **holds the cloud until it ends** |
| `ARMY` / `ARMY2` | `sound_casualty(0/1)` | the battle window (`6a35:0094`), each fallen attacker / defender, until Space hurries it |
| `DRAMATIC` | `sound_dramatic(0)` | a ruin searched by a human (`ruin_search`, `6536:01ab`) and a sage (`6536:0aa0`); holds |
| `ORCH` | `sound_dramatic(1)` | the ruin's guardian, after its line is read; holds |
| `SPLASH` | `sound_splash` | an item dropped at sea on a human's turn (`7563:0943`), a dead hero's items lost at sea (`67cc:16cd`), armies sunk (`1b62:0ea0`) |
| `DING` | `sound_ding` | the next or previous army found (`8c07:019b`, `8c07:0341`) |
| `CHORD` | `sound_chord` | "cannot": no army left to offer, no route (`1a8b:0c4f`, `1c8c:0007`), attacking where no army can stand (`attack_tile`), a key with no command (`17be:0064`, `17be:03ef`), a greyed button (`17be:0e26`), no sides to play (`7bab:0cfe`), a fatal error |

`ARMY`, `DING`, `CHORD` and `ARMY2` are loaded once at start-up
(`sound_options_load`); the rest are loaded, played and freed.

## The advisor

`advisor_speak(what)` (`7dda:026f`). With Speech off it returns at once.

| what | says | when |
|---|---|---|
| 0 | `VMOMENT` "One moment..." | before a random map is made (`7f77:060f`) |
| 1 | `VBEGIN` "Very well then... Let the war... begin!" | the game begins (`7bab:0cfe`), after cue 11 |
| 2 | `VGREET0` "Greetings, Warlord!" | the start screens, the first time (`7f77:0000`) |
| 4 | `VQUIT` "Farewell, Warlord! We -shall- meet again!" | quitting (`7721:0072`) |
| 5 | see below | a human's turn, after the banner (`8cc6:0259`) |

**What 5 says.** Never with 40 cities or more. Each side keeps two bytes in
the `.SCN`: 0x100 + side, what was said last (1 winning, 2 losing), and
0x108 + side, the city count it was said at, rounded down to 5. Both start
at 0.

1. **Fewer cities than the mark:** the mark becomes the count rounded down
   to 5, and the line is by the mark + 5 — 5 `VLOSE05`, 10 `VLOSE10`, 15
   `VLOSE15`, 20 `VLOSE20`, 25 and 30 `VLOSE25`, else (35) `VLOSE35`.
2. **At least mark + 5:** the mark is updated the same way, and the line is
   by the new mark — 10 `VWIN10` … 35 `VWIN35`, else (5) `VWIN05` or
   `VWIN05A`.
3. **Otherwise**, only when the turn is a multiple of 7: gold under 100
   `VGOLD00`; over 2800 `VGOLD01` or `VGOLD01A`; no hero `VHERO00`; five
   or more `VHERO01`; else one time in five (`dice(1,5) = 1`) one of
   `VMESS00`–`03`.

On turn 1 he says nothing, but the marks have been updated.

**The helmet.** He waits for any sample still sounding, saves the screen
under (152, 15) 352 × 442 and draws `VOICE.PCK` there through colour 10
(bitmap 50 of `FILE.DAT` group 3). The clip plays; a count starts at
`dice(1,30,10)` and goes up a tick at a time, and past 40 it goes back to 0
and he **blinks**: `VOICEBIT.PCK` (bitmap 51), three 160 × 47 frames down
the sheet — open, half, shut — put at (232, 261) half, shut, half, open, two
ticks each. When the clip has ended the screen is put back. Nothing cuts him
short. With no digital driver the clip's `.TXT` is written under him
instead, a line at a time.

## In the engine

Played as the Sound Blaster FM version: the `S` songs through a port of
`ADLIB.ADV` into an emulated OPL2, on a thread; the samples at 11 000 Hz,
queued one behind another. Not yet heard, because the engine does not have
the moment they belong to: cue 3 for a quest (no quest-done dialog), cue 7
(promotions happen without their screen), cue 8 (no medals), the random
map's "One moment...", `SPLASH` for armies sunk and a dead hero's items, the
subtitles, and most of the `CHORD`s — it plays for no army left and no
route.
