// Gather what a web server needs into one folder: the page, its modules, the
// game's files and _headers. Nothing is built; the files are copied as they
// are. Run from the repository root:
//
//     node web/dist.js [folder]       -- into build/web/ by default
//
// It first checks that every file the page fetches is spelled as on disk: a
// Mac's disk ignores case, the Linux servers that host sites do not.

import * as fs from "node:fs";
import * as path from "node:path";
import { fileURLToPath } from "node:url";

const web = path.dirname(fileURLToPath(import.meta.url));
const out = path.resolve(process.argv[2] || path.join(web, "..", "build", "web"));
const SHIP = ["index.html", "_headers", "src", "assets"];

// ------------------------------------------------------------------ spelling

const listings = new Map();     // folder -> its names, as spelled on disk

function list(dir) {
  if (!listings.has(dir)) {
    let names = [];
    try { names = fs.readdirSync(dir); } catch (e) { /* not a folder */ }
    listings.set(dir, names);
  }
  return listings.get(dir);
}

/** Is `rel` (with slashes) under web/, spelled exactly so? */
function onDisk(rel) {
  let dir = web;
  for (const part of rel.split("/")) {
    if (!list(dir).includes(part)) return false;
    dir = path.join(dir, part);
  }
  return true;
}

const problems = [];
function need(rel, from) {
  if (!onDisk(rel)) problems.push(`${from}: ${rel} is not on disk with that spelling`);
}

const html = fs.readFileSync(path.join(web, "index.html"), "utf8");
for (const m of html.matchAll(/<script[^>]*\ssrc="([^"]+)"/g)) need(m[1], "index.html");

// Every relative import, static or dynamic.
const IMPORT = /\b(?:from|import)\s*\(?\s*["'](\.{1,2}\/[^"']+)["']/g;
for (const name of fs.readdirSync(path.join(web, "src"), { recursive: true })) {
  if (!name.endsWith(".js")) continue;
  const file = path.join(web, "src", name);
  for (const m of fs.readFileSync(file, "utf8").matchAll(IMPORT)) {
    const rel = path.relative(web, path.resolve(path.dirname(file), m[1])).split(path.sep).join("/");
    need(rel, "src/" + name.split(path.sep).join("/"));
  }
}

// The game's files, under the names src/vfs.js and src/sound.js fetch them by.
const manifest = JSON.parse(fs.readFileSync(path.join(web, "assets", "manifest.json"), "utf8"));
for (const p of manifest.files) {
  const name = manifest.images[p] ? p + ".png" : /\.8SN$/i.test(p) ? p.slice(0, -4) + ".wav" : p;
  need("assets/data/" + name, "manifest.json");
}
for (const song of manifest.music) need("assets/music/" + song + ".ogg", "manifest.json");

if (problems.length) {
  for (const p of problems) console.error(p);
  process.exit(1);
}

// ------------------------------------------------------------------ copying

// Clear a previous copy, and nothing else: a folder holding anything that is
// not shipped is left alone.
if (fs.existsSync(out)) {
  const strays = fs.readdirSync(out).filter((n) => !SHIP.includes(n) && n !== ".DS_Store");
  if (strays.length) {
    console.error(`${out} holds other files (${strays.slice(0, 3).join(", ")}); not touching it`);
    process.exit(1);
  }
  fs.rmSync(out, { recursive: true });
}
fs.mkdirSync(out, { recursive: true });
for (const name of SHIP) {
  fs.cpSync(path.join(web, name), path.join(out, name), {
    recursive: true, filter: (p) => path.basename(p) !== ".DS_Store",
  });
}

function size(p) {
  const s = fs.statSync(p);
  return s.isDirectory() ? fs.readdirSync(p).reduce((t, n) => t + size(path.join(p, n)), 0) : s.size;
}
const mb = (b) => (b / 1048576).toFixed(1) + " MB";
const music = size(path.join(out, "assets", "music"));
console.log(`${path.relative(process.cwd(), out)}: ${mb(size(out) - music)} for the page and the game's files,`
  + ` ${mb(music)} of music fetched a song at a time`);
