# Sound files, and the music driver

What the game plays is described in [`../re/sound.md`](../re/sound.md). This
is the files it plays them from, and the AdLib driver whose behaviour decides
what the music sounds like. The engine's side is
`love2d/warlords/{xmi,ailfm,opl}.lua`.

## `SOUND.DAT` — which drivers

Written by `INSTALL.EXE`, read by `sound_init` (`155e:0005`) with
`sscanf("%s %s %d")`:

```
<digital driver> <music driver> <n>          e.g.  SBLASTER.COM SBFM.ADV 0
```

| field | means |
|---|---|
| digital | a DIGPAK driver (`SBLASTER.COM`, `SBPRO.COM`, `SB16.COM`, `ADLIBG.COM`, `PAUDIO.COM`, …), or `NO_SOUND`. `IBMBAK.COM` is the PC speaker: every sample becomes a `printf("\a")` beep |
| music | a MIDPAK/AIL driver (`ADLIB.ADV`, `SBFM.ADV`, `SBP2FM.ADV`, `SB16.ADV`, `PASFM.ADV`, `MT32MPU.ADV`, `SC32MPU.ADV`, …), or `NO_SOUND` |

`MIDPAK.COM` loads the `.ADV` and the timbre library `MIDPAK.AD`. The music
driver also picks which of three sets of songs is played, by a letter put in
front of each file name (`music_load_xmi`, `155e:032f`, format `"%s%s%s"`):

| driver | prefix | the files |
|---|---|---|
| `MT32MPU.ADV` | `m` | `MINT*.XMI`, `MSTARTUP.XMI` — Roland MT-32 |
| `SC32MPU.ADV` | `r` | `RINT*.XMI`, `RSTARTUP.XMI` — Roland Sound Canvas (General MIDI) |
| any other | `s` | `SINT*.XMI`, `SSTARTUP.XMI` — FM synthesis |

The three sets are the same music arranged for each synthesizer. The engine
plays the `s` set through an emulated AdLib, below.

## `DATA/OPTIONS.SND` — the three switches

Three characters, `'0'` or `'1'`: Music, Effects, Speech (`111` as shipped).
Read at start-up by `sound_options_load` (`6dda:0bc5`) — Music and Effects
count only if their driver loaded — and written back by `64d2:0576` every
time a box in Game › Settings is turned over.

## `.8SN` — samples

Headerless **unsigned 8-bit mono PCM at 11 000 Hz**, silence at 0x80. The
rate is the constant `sample_load` (`155e:04e9`) and `sample_play`
(`155e:065e`) put in DIGPAK's sound structure (`451b:3106 = 11000`). The file
is read whole; its length is the sample's length.

| file | length | what |
|---|---|---|
| `ARMY.8SN`, `ARMY2.8SN` | 0.31 s, 0.28 s | a blow in the battle window: an attacker falls, a defender falls |
| `CHORD.8SN` | 0.22 s | "cannot" |
| `DING.8SN` | 0.23 s | the next army found |
| `TURN.8SN` | 1.39 s | the fanfare under the turn's banner |
| `WAR.8SN` | 2.08 s | the fire cloud before a battle |
| `SPLASH.8SN` | 1.10 s | something lost in the sea |
| `DRAMATIC.8SN`, `ORCH.8SN` | 1.73 s, 1.22 s | a ruin or a sage; a ruin's guardian |
| `V*.8SN` | 2–4 s | the advisor's 27 lines |

## `SOUND/V*.TXT` — subtitles

One per advisor line: text with `|` between lines and `~` at the end, at
most five lines. Shown under the helmet when there is no digital driver
(`advisor_subtitles`, `6dda:0d01`): each line centred on x = 312 at y = 408 in
a box 16 pixels wider than the text rounded down to 16, for 40 ticks.
`VHERO1.TXT` belongs to `VHERO01.8SN`.

## `.XMI` — XMIDI

Miles Design's Extended MIDI, an IFF file:

```
FORM XDIR  { INFO: u16 sequence count }
CAT  XMID  { FORM XMID { TIMB, EVNT } ... }
```

All of the game's files hold one sequence. `TIMB` lists the (patch, bank)
pairs the sequence uses, for a driver to preload. `EVNT` is MIDI with two
differences:

- **Time** is counted in intervals of AIL's 120 Hz timer. A delay is a run
  of bytes below 0x80, simply added up (not a variable-length number).
  Tempo meta events are left in by the converter but mean nothing: the
  delays already have the tempo in them.
- **A note-on carries its duration**, a MIDI variable-length number after
  the velocity. There are no note-offs; the driver's note queue ends each
  note when its time is up.

What the `S` files use is small: notes, program changes, controllers 0, 1
(modulation) and 7 (volume), on MIDI channels 2–6 and 10. No pitch bend, no
sustain pedal, no loops (controllers 116/117). They run from 18 s (`INT22`,
the war beginning) to 4 min 37 s (`INT9`).

## `MIDPAK.AD` — the Global Timbre Library

A directory of 6-byte entries, ended by a bank of 0xFF:

```
+0 u8  patch
+1 u8  bank
+2 u32 offset of the timbre in the file
```

and at each offset a timbre, `u16 length` then its bytes. All 256 here are
14 bytes long — OPL2 two-operator voices:

| +  | byte |
|---|---|
| 2 | transpose, signed |
| 3–7 | modulator: registers 0x20 (AM/VIB/EG/KSR/MULT), 0x40 (KSL/TL), 0x60 (AR/DR), 0x80 (SL/RR), 0xE0 (waveform) |
| 8 | 0xC0: feedback and connection |
| 9–13 | carrier: 0x20, 0x40, 0x60, 0x80, 0xE0 |

Bank 0 holds the 128 melodic programs (General MIDI order). **Bank 127 is
the drums, keyed by note**: MIDI channel 10's key picks the timbre, and the
timbre's transpose byte is the pitch it is played at (key 38, the snare,
plays at note 38; keys below 35 all play at 51).

## `ADLIB.ADV` — the driver

Miles Design's AIL 2.0 AdLib driver, "Copyright (C) 1991,1992". It contains
the XMIDI sequencer as well as the FM code. It loads at offset 0 of its
segment, so the addresses here are file offsets. `ailfm.lua` reads the tables
straight out of the file.

| offset | table |
|---|---|
| `0x101` | 192 words: F-numbers, 16 fine steps for each of the 12 half-tones from C. Those from G up are stored an octave down with the high byte 0xFE |
| `0x281` | 96 bytes: the octave of each note of the driver's range |
| `0x2e1` | 96 bytes: its half-tone |
| `0x340` | register values written to OPL registers 1–0xF5 at start-up (`0x129c`): 0x01 = 0x20 (waveforms on), 0xBD = 0xC0 (deep tremolo and vibrato, no rhythm mode), 0x20–0x35 = 1, 0x40–0x55 = 0x3F, 0x60–0x75 = 0xFF, 0x80–0x95 = 0x0F |
| `0x52b` | 16 bytes: velocity / 8 → sensitivity: 82, 85, 88 … 127 |
| `0x21f2` | the controllers set on channels 2–10 at start-up (`0x3037`), values at `0x21fb`: volume 127, modulation 0, pan 64, expression 127, sustain 0, bank 0 |
| `0x2204` | the program each of channels 2–9 starts on: 68, 48, 95, 78, 41, 3, 110, 122 |

**Channels.** Note-ons count only on MIDI channels 2–10 (`0x203f`); 10 is
the drums. Controller 0 is ignored; the timbre bank is controller 114.
Controller 112 locks a channel's voices against being taken (below).

**A note** (`0x1ecc`) takes the first free one of 16 slots — no free slot,
no note — copies its timbre into it (`0x1d6c`), and looks for a voice.

**Voices** (`0x1740`): nine, searched round-robin from the one after the
last given out. With none free, `0x1c69` weighs the claims: each sounding
note is worth 0x7FFF less the number of voices its channel holds (0xFFFF if
the channel is locked). While the best claim among notes without a voice is
at least the worst among notes with one, the weaker note is cut off and its
voice handed over. So a channel with many voices gives them up to one with
few — the tenth note of a chord takes a voice from its own chord.

**Pitch** (`0x1b61`): the note plus the timbre's transpose, less 24, folded
into 0–95 by octaves; in sixteenths of a half-tone with the pitch bend
(±12 half-tones over its range); then the F-number from `0x101` and the
block from `0x281` less one, one up for the entries flagged 0xFE and one up,
with the F-number halved, below block 0. **MIDI note 60 sounds at 131 Hz**,
F-number 690 in block 2 — an octave below General MIDI's middle C. The music
was written on this driver, so this is how it is meant to sound.

**Volume** (`0x1807`, flag 0x40): with `f(a, b)` = the high byte of
`a × b × 2`, plus one unless it is 0,

```
v = f(f(channel volume, expression), sensitivity[velocity / 8])
level = (63 - TL) × v / 127          -- the carrier always; the modulator
TL' = 63 - level                     -- only when connection is additive
```

**Modulation** of 64 or more sets the vibrato bit of both operators
(register 0x20).

## The OPL2

The Yamaha YM3812, at 3.579545 MHz / 72 = 49 716 samples a second. `opl.lua`
follows Nuked-OPL3's model of the chip — the log-sine and exponent ROMs, the
envelope generator's rate counter, the tremolo and vibrato counters — cut
down to what the driver uses: nine two-operator channels, four waveforms, no
rhythm mode, no timers.
