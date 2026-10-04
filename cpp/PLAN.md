# Warlords II — C++/SDL2 port plan

A port of the Lua/LÖVE remake (`love2d/`) to C++20 on SDL2. The Lua remake is
the spec for behaviour. The JavaScript port (`web/`) has already been checked
against the Lua turn by turn, and is 0-based like C++, so it serves as a
structural guide. Where the two disagree, the Lua wins.

## Shape

```
cpp/
  CMakeLists.txt
  src/
    platform/        the slim layer over SDL2: window and display, a
                     love.graphics-alike over SDL_Renderer, input names,
                     the audio device and mixer, files, timer
    warlords/        the rules core, headless (no SDL): links into the tests
    warlords/ai/     the computer player
    ui/              the dialogs, one file per original dialog
    main.cpp         the front end (love2d/main.lua)
  test/              run.cpp (the rules suite), trace.cpp + compare.sh (Lua vs C++)
```

### Decisions

- **Assets:** none are converted or checked in. The game reads the original
  formats from `original/` at runtime, as the Lua does: `.PCK`/`.FNT` (planar
  LZ77), `.8SN`, `.XMI`, and the text and record files. The MT-32 and SC-55
  recordings are streamed from `pre-rendered-sound/*.ogg` where they lie,
  through libvorbisfile (optional at build time). The AdLib music is
  synthesized live by a C++ port of the remake's OPL2 and AIL driver.
- **SDL layer:** `gfx` mirrors the parts of `love.graphics` the Lua uses:
  setColor, rectangle, draw image or quad, a scissor, push/pop/translate/scale.
  It draws through SDL_Renderer with nearest-neighbour textures. An image
  keeps its colour indices, so palettes and colour remaps still work as they
  do in the Lua.
- **The computer's turn:** Lua's coroutines become one AI thread with a
  strict handoff. The main thread waits while the AI runs. At each hook
  (walk, fight, spoils) the AI hands control back and waits. Only one of the
  two ever runs at a time, so nothing is shared concurrently, and the AI code
  stays ordinary functions.
- **Objects:** armies, cities, sides, sites and items are structs. A game owns
  every army it ever made, so pointers held by quests, groups and selections
  stay valid. Lua's nil becomes `std::optional` or an explicit sentinel.
- **Determinism:** the RNG, Lua 5.1's unstable sort and floating-point
  division all follow the Lua. `test/compare.sh` plays the same all-computer
  games in LuaJIT and in C++ and compares them turn by turn.
- **Files:** saves are JSON in the C++ port's own format (not the Lua's).
  Saves, prefs and shortcuts sit in SDL's per-user folder for the game
  (SDL_GetPrefPath), as `w2-*.json`.

## Steps

- [x] 1. Scaffold: CMake, SDL2 window, platform layer (files, gfx, input, timer)
- [x] 2. Core plumbing: util (fmt, luaSort, json), rng, bytes, pal, pck, armytype, scn, rules
- [x] 3. Rules: move, combat, game, hero, site, quest, diplomacy, slots, history, report, save
- [x] 4. Computer player: aicard, ai, ai/* (with the AI thread handoff)
- [x] 5. Tests: trace + compare.sh against the Lua; the rules suite (run.lua) ported
- [x] 6. Screen plumbing: font, uidata, layout, stonetile, menu, screen, display
- [x] 7. Front end: main — map, strategic map, bars, banner, hero offer, assault, input
- [x] 8. Dialogs: kit, then every ui/*
- [x] 9. Start screens, saves and prefs
- [x] 10. Sound: samples, the advisor, OPL2 music, recordings
- [ ] 11. Check (offscreen screenshots and a scripted run), README, polish

## Notes along the way

- `cpp/test/compare.sh` plays the same all-computer games in LuaJIT and in
  C++ and compares them after every side's turn: gold, armies, the dice's
  state, a hash of every army and every city's owner. Every scenario, the
  hidden map and a save round trip come out identical.
- Not the Lua's: the AI's city neighbour table is kept per map, as in the
  Lua, but holds city indices rather than the first game's city objects. In
  the Lua (and the JS port) a second game of the same map, or a loaded save,
  reads the neighbours' owners from the first game's cities. The comparison's
  Lua trace (`cpp/test/trace.lua`) points the table at the loaded game's
  cities, so the save round trip can be compared at all.
- The save keeps no per-turn marks (done, offered), as the Lua's does not.
- `warlords2 --script FILE` drives the game for checking: keys, clicks,
  frame waits, screenshots (.bmp) and a line of state. With
  `SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software` it runs without a
  window; fifteen skipped rounds and a watched one have been run that way.
- The FM music is checked sample for sample against the Lua: `w2fm` and
  `test/fmrender.lua` render a song through the AIL driver and the OPL2 and
  print a checksum per second; SSTARTUP, SINT0, SINT7 and SINT12 match.
- Sound is mixed in one SDL callback: the DIGPAK queue, the FM driver at the
  chip's rate and the recordings, each resampled to the device's rate.
  `SDL_AUDIODRIVER=disk` writes the mix to a file for checking.
