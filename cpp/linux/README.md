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

The touchscreen is the mouse (a tap is a left click), and the game reads the
controller itself, through Steam Input's gamepad: leave the controller layout
on Steam's **Gamepad** default.

| Deck | does |
|---|---|
| right trackpad or right stick | move the pointer: the further over, the faster |
| left stick | slide the map |
| R2 or A | left click: pick up, move, press buttons; hold to drag the map |
| L2 or X | right click, held: what is on a tile, an army's numbers, a button's help |
| B / Y | Esc / Enter: quit army / next army, Cancel / OK |
| L1 / R1 | zoom the map out / in, about the pointer |
| D-pad | step the stack one tile, as 8 2 4 6 do |
| hold View (⧉) | Shift, for a Shift-click (the Military Advisor's verdict) |
| hold Menu (☰) | Alt, for an Alt-click (plan a route) with R2 |
| L3 / R3 | group the whole stack / centre on it |

The menus, End Turn and everything else are on the screen, under the
pointer. Steam's Gamepad layout hands the game the right trackpad as the
right stick, so the thumb on the trackpad steers the pointer as the stick
does, and R2 clicks. For the trackpad as a true mouse instead, pick the
**Gamepad with Mouse Trackpad** layout in the game's Controller Settings.

On the Deck's screen the interface is drawn 1.67 times its size, smoothed.
View › Interface 2x draws it twice its size in whole pixels instead, for
bigger menus and buttons under a finger: the map and the strategic map give
up the rows for it, and the strategic map scrolls with the view. The start
screens, which need all their rows, stay at 1.67.

Clicking into a name field (a city, a save, a hero) brings up Steam's
keyboard; Enter on it keeps the name. Steam + X brings it up anywhere.

With Steam running, Steam has the Deck's controls and the game sees only the
gamepad Steam makes of them. Run from Konsole in Desktop Mode, the trackpads
stay the mouse they are on the desktop.
