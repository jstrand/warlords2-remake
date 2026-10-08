# Warlords II — remade

A remake of SSG's *Warlords II* (1993) that reads the original game's
own data files and plays it: the maps, armies, art, music and computer
players are the original's, decoded rather than redrawn.

Play it in the browser: <https://jstrand.github.io/warlords2-remake/>

## What's here

| folder | what it is |
|---|---|
| [`love2d/`](love2d/README.md) | the engine in Lua on LÖVE; it defines how the game behaves |
| [`cpp/`](cpp/README.md) | the Lua remake ported to C++20 on SDL2 |
| [`web/`](web/README.md) | the Lua remake ported to plain JavaScript on a `<canvas>` |
| [`docs/`](docs/) | the file formats, the rules, and the reverse engineering of `WARLORD2.EXE` |
| `tools/` | decoders and exporters for the data files, the Ghidra and DOSBox helpers |
| `original/` | the original game's files, read as they are at runtime |
| `pre-rendered-sound/` | the music, recorded in advance |

The three remakes play the same game: an all-computer game plays out
identically in each.

## Quick start

```sh
love love2d                                    # Lua, needs LÖVE 11+
cmake -S cpp -B cpp/build && cmake --build cpp/build -j && cpp/build/warlords2
cd web && python3 -m http.server 8765          # then open http://localhost:8765/
```

Run the first two from the repository root, so that `original/` is found. Each remake's
README has its controls, options and tests.
