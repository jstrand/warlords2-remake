# Warlords II Remake for Linux

The C++ port, built for x86_64 Linux in Valve's Steam Runtime 3 ("sniper")
SDK. It runs on any distribution with glibc 2.29 or newer: Ubuntu 20.04
and Debian 11 on, any current Fedora, Arch or openSUSE, and SteamOS on the
Steam Deck (Debian 10 is too old). It carries
its own SDL2 and Vorbis in `lib/` and uses whatever the system has for the
screen (X11 or Wayland) and the sound (PipeWire, PulseAudio or ALSA).

## Building (on any computer with Docker)

```sh
sh cpp/linux/build.sh            # or DOCKER=podman sh cpp/linux/build.sh
```

Out comes `cpp/build/linux/warlords2-linux-x86_64.tar.gz` (about 130 MB),
holding the game, the original's files and the music recordings. The first
run downloads the SDK image (a few GB) and SDL2's source; on a Mac with
Apple silicon everything runs under x86 emulation and takes a few minutes.

## Installing and running

Unpack it somewhere writable, e.g. your home folder, and run the launcher:

```sh
tar -xzf warlords2-linux-x86_64.tar.gz -C ~
~/Warlords2/warlords2.sh
```

The folder must stay writable: the game keeps its options in
`original/DATA/`, as the original does. It opens full screen; View › Window
switches, and `--window` starts in a window. Game › Quit ends it.

Saves, the quick save, preferences and button shortcuts are kept in
`~/.local/share/Warlords II Remake/`.

The launcher passes its arguments on, as on other systems
(`cpp/README.md`): `warlords2.sh --scenario ISLADIA`, `--seed 7`, `--window`.

## On the Steam Deck

1. In Desktop Mode, copy the archive over (a USB stick, `scp`, KDE
   Connect…) and unpack it as above.
2. Try it once from Konsole: `~/Warlords2/warlords2.sh`.
3. In Steam: Games › Add a Non-Steam Game to My Library › Browse, choose
   `~/Warlords2/warlords2.sh` (set the file type filter to "All Files"), and
   Add Selected Programs. Leave Compatibility alone: this is a native Linux
   game, not one for Proton.

Back in Gaming Mode it is in the library under Non-Steam. Launch options
(`--scenario ISLADIA` and the like) go in the shortcut's Properties.

### Controls

The game is played with the mouse and a few keys. The touchscreen works
as the mouse straight away (a tap is a left click). For the controller, open
the game's Controller Settings in Gaming Mode and pick the **Keyboard
(WASD) and Mouse** template, then adjust:

| Deck | as | does |
|---|---|---|
| right trackpad | mouse | point |
| R2 / right trackpad click | left click | pick up, move, press buttons |
| L2 | right click (hold) | what is on a tile, an army's numbers |
| left stick or D-pad | arrow keys | move the view |
| right stick, scroll | mouse wheel | zoom the map |
| A / B | Enter / Esc | next army / quit army, OK / Cancel |
| X | Space | group the whole stack |
| Y | Alt+E (a key chord) | end the turn |
| L1 / R1 | F5 / F9 | quick save / quick load |

Steam + X brings up the on-screen keyboard for naming cities and saves.
