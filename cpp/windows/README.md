# Warlords II Remake for Windows

The C++ port, built for 64-bit Windows 10 and 11 with MinGW-w64's gcc. It
carries the DLLs it needs (SDL2, Vorbis and gcc's runtime) beside
`warlords2.exe`.

## Installing and running

Unzip it somewhere writable, e.g. your Documents folder (not Program Files),
and run `Warlords2\warlords2.exe`. Windows may warn that it comes from an
unknown publisher, because it is not signed: More info › Run anyway.

The folder must stay writable: the game keeps its options in
`original\DATA\`, as the original does. It opens full screen; View › Window
switches, and `--window` starts in a window. Game › Quit ends it.

Saves, the quick save, preferences and button shortcuts are kept in
`%APPDATA%\Warlords II Remake\`.

Arguments go to the game as on other systems (`cpp/README.md`), from a
shortcut's Target or a command prompt in the folder:
`warlords2.exe --scenario ISLADIA`, `--seed 7`, `--window`.

## Building

In MSYS2's UCRT64 shell (<https://www.msys2.org/>):

```sh
pacman -S --needed git zip mingw-w64-ucrt-x86_64-{gcc,cmake,ninja,pkgconf,SDL2,libvorbis}
sh cpp/windows/build.sh
```

Out comes `cpp/build/windows/warlords2-windows-x86_64.zip`, holding the
game, the original's files and the music recordings.
