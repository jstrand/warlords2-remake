---
name: run-web
description: Run, start, serve, drive and screenshot the Warlords II web build (web/) in a headless browser. Use when asked to run the web version, try a change in the browser, check a UI fix in the real game, take a screenshot, play turns, or test the web build.
---

The web build is static ES modules served from `web/`. Drive it with
`.claude/skills/run-web/driver.mjs`: it opens a game in headless Chromium
through Playwright, runs a script of yours against it (real mouse and keys),
and saves screenshots. The game's state is `window.W2` in the page, so a
script can set up a situation directly and then play it through the UI.

All paths below are relative to `web/`. Verified on macOS (arm64) with
Node 24 and the Chromium Playwright had cached.

## Setup

`playwright-core` goes outside the repo (no `node_modules` in `web/`):

```bash
npm i --prefix "${TMPDIR:-/tmp}/w2-pw" playwright-core@1.49
```

The driver finds Chromium in Playwright's cache (`~/Library/Caches/ms-playwright`
or `~/.cache/ms-playwright`, newest `chromium-*`). If there is none, set
`CHROME` to any Chromium/Chrome executable.

## Run (agent path)

Serve `web/`, poll until it answers, run a script, stop the server:

```bash
(python3 -m http.server 8765 > "${TMPDIR:-/tmp}/w2-http.log" 2>&1 &)
for i in $(seq 1 30); do curl -sf http://localhost:8765/ >/dev/null && { echo served; break; }; sleep 1; done
node .claude/skills/run-web/driver.mjs .claude/skills/run-web/smoke.mjs
lsof -ti:8765 -sTCP:LISTEN | xargs kill
```

`smoke.mjs` gets past the turn-1 dialogs, clicks the hero to select it,
reads the pointer next to it, ends the turn with Alt-E, waits out the
computers and screenshots turn 2. It is the template for a check of your
own: copy it, keep the shape. Screenshots land in `$SHOTS` (default
`$TMPDIR/w2-shots/`), and each path is printed. **Look at them.** The driver
exits 1 if the script throws or the page logged an error.

An optional second argument is the URL: `?scenario=<DIR>&seed=<n>` (default
`ERYTHEA`, seed 3; the same seed gives the same game).

What a script gets (`export default async (t) => {}`):

| helper | does |
|---|---|
| `page` | the Playwright page: `page.mouse`, `page.keyboard`, `page.evaluate` |
| `shot(name)` | screenshot to `$SHOTS/<name>.png` |
| `settle()` | press Enter until no banner, hero offer, dialog, battle or computer turn is up; `false` if something stays |
| `speechOff()` | turn the advisor's speech off (call it first, see Gotchas) |
| `tileCentre(x, y)` | page coordinates of a map tile's middle (centre the view on it first) |
| `click(px, py)` | left click |
| `altE()` | End Turn, as the key |
| `waitTurn(n)` | wait until the computers are done and the turn is past `n` |
| `pointer(px, py)` | the pointer the game shows there: `ATTACK`, `WALK`, `HAND`, `SELECT`, ... |

Setting things up from a script (all used for real):

- Armies: push `{ x, y, owner, type: 11, name: "Scouts", strength: 3, maxMoves: 20, moves: 20, upkeep: 1 }` onto `W2.g.armies` (type 28 is a hero).
- Cities: `(await import("/src/warlords/game.js")).setCityOwner(W2.g, city, sideIndex)`.
- Diplomacy, which the URL cannot switch on: `W2.g.map.options.diplomacy = 1`, then `diplomacy.init(W2.g)` from `/src/warlords/diplomacy.js`.
- Watching computer turns: `side.observe` per side (Settings' Observe).
- The view: `W2.centreOn(x, y)`; the selection: click its tile.
- Menu greying: `W2.menuEnabled("alt E")`, keyed like the menu's accelerators.
- A computer battle on screen: `W2.assault && W2.assault.computer`.

## Run (human path)

```bash
python3 -m http.server 8765   # then open http://localhost:8765/?scenario=ERYTHEA&seed=3 ; Ctrl-C stops it
```

## Test

The rules suite, from the repository root (it covers `src/warlords/`, not
`src/main.js` or the dialogs, which is why the driver exists):

```bash
(cd .. && node web/test/run.js 2>&1 | tail -1)
```

Expect `9571 passed, 0 failed` (the count grows with the suite).

## Gotchas

- **Turn 1 is two dialogs deep.** The hero offer, then the hero's city's
  production dialog. Until both are gone `W2.pointerKind` says `ARROW`
  everywhere and clicks do nothing. `settle()` handles it.
- **The advisor holds the screen while his speech plays**, and headless
  audio takes its time, so `settle()` gives up with the helmet still up.
  Call `speechOff()` before anything else.
- **Reach the game's modules with `await import("/src/...")` inside
  `page.evaluate`.** They are the same instances the game runs (`kit.top()`
  is the open dialog), not copies.
- **`W2.pointerKind` and `W2.mapToUI` work in UI pixels** (640×480). The
  viewport is 640×480 so they match page pixels, but `tileCentre`/`pointer`
  convert anyway.
- **A win needs more than half of all cities, neutrals included.** Handing
  over only the computers' cities eliminates them without winning.
- **Every run is a fresh browser profile**: saves, quick saves and
  preferences (`localStorage`) start empty, and changing them leaves the
  repo alone.
- **No `timeout` command on macOS**: poll with a `for` loop, as above.

## Troubleshooting

- **`settle()` returns `false` and the screenshot shows the helmet**: the
  advisor is talking. `speechOff()` first.
- **`pointer()` is `ARROW` over the map**: a dialog is still open (the
  production dialog on turn 1). `settle()`, then check `W2.selection`.
- **`timeout: command not found`** (macOS): use the polling loop above.
- **`checkEnd` never sets `W2.g.won`**: neutral cities count. Give every
  unrazed city in `W2.g.map.cities` to the player.
