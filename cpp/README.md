# Warlords II — in C++ on SDL2

The Lua remake (`love2d/`) ported to C++20, drawing and playing sound through
SDL2. The Lua remake defines how the game behaves. The rules core, the
computer player and the FM music follow it closely enough that an
all-computer game plays out identically in both, and a song renders to the
same samples (see *Tests*).

Nothing is converted and no assets are checked in. The game reads the
original files from `original/` as they are, and streams the MT-32 and Sound
Canvas recordings from `pre-rendered-sound/` where they already lie.

## Building

You need CMake, a C++20 compiler and SDL2. libvorbisfile is optional: without
it the game still builds, and plays the AdLib music only.

```sh
brew install cmake sdl2 libvorbis                       # macOS
sudo apt install cmake libsdl2-dev libvorbis-dev        # Debian, Ubuntu

cmake -S cpp -B cpp/build
cmake --build cpp/build -j
```

## Running

Run from the repository root, so that `original/` and `pre-rendered-sound/`
are found:

```sh
cpp/build/warlords2                          # the start screens
cpp/build/warlords2 --scenario ISLADIA       # straight into a scenario
cpp/build/warlords2 --scenario ERYTHEA --seed 7 --window
cpp/build/warlords2 --data /path/to/WARLORD2 # the game's files elsewhere
```

The controls are the Lua remake's (`love2d/README.md`). The differences:

- Saves, the quick save (F5 and F9), preferences and the button shortcuts are
  `w2-*.json` files in SDL's per-user folder for the game. On macOS that is
  `~/Library/Application Support/Warlords II Remake/`. The format is this
  port's own, so saves from the Lua or the browser version do not load here.
- The sound switches live in `original/DATA/OPTIONS.SND`, as they do for the
  original and the Lua remake.
- The AdLib music is synthesized live by the port of the remake's OPL2
  emulator and AIL driver. The MT-32 and SC-55 music items in the Game menu need
  libvorbisfile and the recordings.

## Layout

```
src/util/         printf-style formatting, Lua 5.1's sort, JSON, files found ignoring case
src/warlords/     the rules core, plus XMI, the OPL2 and the AIL driver (no SDL)
src/warlords/ai/  the computer player
src/platform/     the slim layer over SDL2: gfx (a love.graphics-alike over
                  SDL_Renderer), display, keys, the coroutine, the sound mixer
src/front/        fonts, images, the screen layout, the menus, prefs, shared state
src/ui/           one file per dialog (love2d/ui/)
src/main.cpp      the front end (love2d/main.lua): map, bars, input, menus, the computer's turn
test/             the rules suite, and the Lua comparisons
```

Notes for anyone porting more of the Lua:

- `w2::NONE` (-1) stands for Lua's nil in ids and indices. Functions that
  return several values in Lua return a struct or a pair here.
- A game owns every army it ever made, so pointers held by quests, groups and
  selections stay valid. An army is alive while it is in `Game::armies`.
- The computer's turn runs on a `Coroutine`, a thread with a strict handoff,
  so only one side runs at a time. The AI's hooks yield to the screen after a
  walk or a battle worth showing.
- Dialogs are `kit::Modal`s on `G.modals`. A handler copies what it needs
  before it calls `kit::pop(this)`.
- `PLAN.md` lists where the port deliberately differs from the Lua.

## Tests

From the repository root, after building:

```sh
cpp/build/w2test                    # the rules suite (love2d/test/run.lua, ported)
sh cpp/test/compare.sh              # all-computer games, Lua vs C++, turn by turn (needs luajit)
cpp/build/w2fm original/SOUND/SINT12.XMI 10 > c.txt
luajit cpp/test/fmrender.lua original/SOUND/SINT12.XMI 10 | tr '\t' ' ' > l.txt
diff c.txt l.txt                    # the FM music, checksummed second by second
```

The game can also be driven by a script, with no window, for checking the
screens:

```sh
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy \
  cpp/build/warlords2 --window --scenario ERYTHEA --seed 1 --script steps.txt
```

A script has one command per line: `wait N` (frames), `key NAME`,
`hold NAME`, `release NAME`, `click X Y [BUTTON]`, `move X Y`, `text STRING`,
`select TX TY`, `attack TX TY`, `near`, `shot FILE.bmp`, `eval` and `quit`.
Keys go by LÖVE's names for them. Points are in the 640x480 interface's
pixels, and tiles in map coordinates. With `SDL_AUDIODRIVER=disk` the mix is
written to a file.
