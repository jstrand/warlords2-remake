// The game's files, fetched once at boot so that everything after can read
// them synchronously, as the Lua remake reads from disk.
//
// tools/web_assets.py writes web/assets/: data/ mirrors the original's
// folders, with every .PCK as a greyscale PNG (grey = colour index x 17) and
// every .8SN as a WAV, and manifest.json lists what is there. Lookups ignore
// case and take either slash, as DOS did.

import { latin1 } from "./util.js";

const files = new Map();    // KEY -> Uint8Array
const images = new Map();   // KEY -> { w, h, px }
const written = new Map();  // files the game writes (OPTIONS.SND), kept in localStorage
let manifest = { files: [], images: {}, songs: {}, music: [] };
let base = "assets/";

export function key(path) {
  return String(path).replace(/\\+/g, "/").replace(/^(\.?\/)+/, "").replace(/\/+/g, "/").toUpperCase();
}

/** Install files directly -- node's tests read them from disk. */
export function install(path, bytes) { files.set(key(path), bytes); }
export function installImage(path, img) { images.set(key(path), img); }
export function setManifest(m) { manifest = m; }
export function getManifest() { return manifest; }
export function assetBase() { return base; }

/** The bytes of a file, or null. */
export function read(path) {
  const k = key(path);
  if (written.has(k)) return written.get(k);
  return files.get(k) || null;
}

/** A file as a string of byte-characters, or null. */
export function readText(path) {
  const b = read(path);
  return b ? latin1(b) : null;
}

export function exists(path) {
  const k = key(path);
  return files.has(k) || images.has(k) || written.has(k);
}

/** A file the game writes: kept in localStorage where there is one. */
export function write(path, bytes) {
  const k = key(path);
  written.set(k, bytes);
  try {
    localStorage.setItem("w2file:" + k, latin1(bytes));
  } catch (e) { /* no storage: it lasts the session */ }
}

/** A decoded .PCK: { w, h, px } with px a Uint8Array of colour indices. */
export function image(path) {
  const img = images.get(key(path));
  if (!img) throw new Error("cannot open image: " + path);
  return img;
}

export function hasImage(path) { return images.has(key(path)); }

/** The URL of a converted asset, by its original path. */
export function url(path) { return base + "data/" + path; }

// Turn a greyscale PNG back into colour indices, at full precision: the
// bitmap is decoded without colour management.
async function decodePng(blob) {
  const bmp = await createImageBitmap(blob, { colorSpaceConversion: "none", premultiplyAlpha: "none" });
  const c = new OffscreenCanvas(bmp.width, bmp.height);
  const ctx = c.getContext("2d", { willReadFrequently: true });
  ctx.drawImage(bmp, 0, 0);
  const rgba = ctx.getImageData(0, 0, bmp.width, bmp.height).data;
  const px = new Uint8Array(bmp.width * bmp.height);
  for (let i = 0; i < px.length; i++) px[i] = Math.round(rgba[i * 4] / 17);
  return { w: bmp.width, h: bmp.height, px };
}

/** Fetch everything. `progress(done, total)` is told as it goes. */
export async function boot(assetBase = "assets/", progress = () => {}) {
  base = assetBase;
  manifest = await (await fetch(base + "manifest.json")).json();
  const jobs = [];
  const total = manifest.files.length;
  let done = 0;
  const tick = () => progress(++done, total);
  for (const path of manifest.files) {
    const ext = path.slice(path.lastIndexOf(".")).toUpperCase();
    if (ext === ".8SN") { tick(); continue; }     // played straight from its WAV
    if (ext === ".PCK") {
      jobs.push(fetch(base + "data/" + path.slice(0, -4) + ".png")
        .then((r) => r.blob()).then(decodePng)
        .then((img) => { images.set(key(path), img); tick(); }));
    } else {
      jobs.push(fetch(base + "data/" + path).then((r) => r.arrayBuffer())
        .then((b) => { files.set(key(path), new Uint8Array(b)); tick(); }));
    }
  }
  await Promise.all(jobs);
  try {
    for (let i = 0; i < localStorage.length; i++) {
      const k = localStorage.key(i);
      if (k.startsWith("w2file:")) {
        const s = localStorage.getItem(k);
        const b = new Uint8Array(s.length);
        for (let j = 0; j < s.length; j++) b[j] = s.charCodeAt(j);
        written.set(k.slice(7), b);
      }
    }
  } catch (e) { /* no storage */ }
}
