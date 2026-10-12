// Drive the web build in headless Chromium: load a game, run a script
// against it, take screenshots.
//
//   node .claude/skills/run-web/driver.mjs <script.mjs> [url]
//
// <script.mjs> exports `default async (t) => {}`, where t holds
//   page                 the Playwright page (real mouse and keyboard)
//   shot(name)           screenshot to $SHOTS/<name>.png (default: $TMPDIR/w2-shots)
//   settle()             press Enter until no banner, offer, dialog or computer turn is up
//   speechOff()          turn the advisor's speech off (he holds the screen while he talks)
//   tileCentre(x, y)     page coordinates of a map tile's middle
//   click(px, py)        a left click at page coordinates
//   altE()               Turn > End Turn, as the key
//   waitTurn(n)          wait for the computers to finish and the turn to pass n
//   pointer(px, py)      the pointer the game shows there, by name ("ATTACK", "WALK", ...)
//
// The game's state is `window.W2` in the page (src/main.js's G). Modules can be
// reached from page.evaluate with `await import("/src/...")`: they are the same
// instances the game uses.
//
// Needs playwright-core installed in $PW_DIR (default: $TMPDIR/w2-pw) and a
// Chromium: $CHROME, or the newest one Playwright has cached.

import { createRequire } from "node:module";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { pathToFileURL } from "node:url";

const PW_DIR = process.env.PW_DIR || path.join(os.tmpdir(), "w2-pw");
const SHOTS = process.env.SHOTS || path.join(os.tmpdir(), "w2-shots");
const { chromium } = createRequire(path.join(PW_DIR, "x.js"))("playwright-core");

function findChrome() {
  if (process.env.CHROME) return process.env.CHROME;
  const caches = [path.join(os.homedir(), "Library/Caches/ms-playwright"), path.join(os.homedir(), ".cache/ms-playwright")];
  const tails = [
    "chrome-mac-arm64/Google Chrome for Testing.app/Contents/MacOS/Google Chrome for Testing",
    "chrome-mac/Chromium.app/Contents/MacOS/Chromium",
    "chrome-linux/chrome",
  ];
  for (const c of caches) {
    if (!fs.existsSync(c)) continue;
    const dirs = fs.readdirSync(c).filter((d) => /^chromium-\d+$/.test(d)).sort().reverse();
    for (const d of dirs) for (const t of tails) {
      const p = path.join(c, d, t);
      if (fs.existsSync(p)) return p;
    }
  }
  throw new Error("no Chromium found: set CHROME to its executable");
}

const PTR = ["ARROW", "VIEW", "BOAT", "CITY", "HAND", "SELECT", "WALK", "SITE", "ATTACK", "ADVISE", "PEACE", "ALT"];

const [scriptArg, url = "http://localhost:8765/?scenario=ERYTHEA&seed=3"] = process.argv.slice(2);
if (!scriptArg) { console.error("usage: node driver.mjs <script.mjs> [url]"); process.exit(2); }
fs.mkdirSync(SHOTS, { recursive: true });

const browser = await chromium.launch({ executablePath: findChrome() });
// 640x480 is the original's screen: one CSS pixel per UI pixel
const page = await browser.newPage({ viewport: { width: 640, height: 480 } });
const errors = [];
page.on("pageerror", (e) => errors.push(String(e)));
page.on("console", (m) => { if (m.type() === "error") errors.push(m.text()); });
await page.goto(url);
await page.waitForFunction(() => window.W2 && window.W2.g && window.W2.player, null, { timeout: 60000 });

const t = {
  page,
  async shot(name) {
    await page.waitForTimeout(150);
    const file = path.join(SHOTS, name + ".png");
    await page.screenshot({ path: file });
    console.log("shot", file);
    return file;
  },
  async settle() {
    for (let i = 0; i < 20; i++) {
      const busy = await page.evaluate(async () => {
        const kit = await import("/src/ui/kit.js");
        return !!(W2.banner || W2.offer || W2.aiRun || W2.assault || W2.victory || kit.top());
      });
      if (!busy) return true;
      await page.keyboard.press("Enter");
      await page.waitForTimeout(400);
    }
    return false;
  },
  speechOff: () => page.evaluate(async () => {
    const snd = await import("/src/sound.js");
    if (snd.speechOn()) snd.toggle("speech");
  }),
  tileCentre: (x, y) => page.evaluate(([x, y]) => {
    const [ux, uy] = W2.mapToUI(x + 0.5, y + 0.5);
    const c = document.querySelector("canvas").getBoundingClientRect();
    return [c.left + ux * c.width / 640, c.top + uy * c.height / 480];
  }, [x, y]),
  async click(px, py) {
    await page.mouse.move(px, py); await page.mouse.down(); await page.mouse.up();
    await page.waitForTimeout(300);
  },
  async altE() {
    await page.keyboard.down("Alt"); await page.keyboard.press("KeyE"); await page.keyboard.up("Alt");
    await page.waitForTimeout(300);
  },
  async waitTurn(n, ms = 120000) {
    const t0 = Date.now();
    while (Date.now() - t0 < ms) {
      if (await page.evaluate((n) => !W2.aiRun && W2.g.turn > n, n)) return true;
      await page.waitForTimeout(200);
    }
    return false;
  },
  async pointer(px, py) {
    await page.mouse.move(px, py);
    await page.waitForTimeout(150);
    return PTR[await page.evaluate(([px, py]) => {
      const c = document.querySelector("canvas").getBoundingClientRect();
      return W2.pointerKind((px - c.left) * 640 / c.width, (py - c.top) * 480 / c.height);
    }, [px, py])];
  },
};

let failed = false;
try {
  const mod = await import(pathToFileURL(path.resolve(scriptArg)).href);
  await mod.default(t);
} catch (e) {
  failed = true;
  console.error(e);
} finally {
  if (errors.length) { failed = true; console.log("PAGE ERRORS:\n" + errors.join("\n")); }
  await browser.close();
}
process.exit(failed ? 1 : 0);
