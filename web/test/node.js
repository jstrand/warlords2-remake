// Node's way into the game's files: web/assets/data, read straight off disk
// into the vfs, so the rules core runs here as it does in the browser. The
// images are decoded too -- they are greyscale PNGs of colour indices -- for
// the tests that look at them; the sounds are left out.
import * as fs from "node:fs";
import * as path from "node:path";
import * as zlib from "node:zlib";
import { fileURLToPath } from "node:url";
import * as vfs from "../src/vfs.js";

const here = path.dirname(fileURLToPath(import.meta.url));
export const ASSETS = path.join(here, "..", "assets");

/** An 8-bit greyscale PNG, as tools/web_assets.py writes them: { w, h, px }
 *  with px the colour indices (grey / 17). */
export function decodePng(buf) {
  let pos = 8, w = 0, h = 0, depth = 0, colour = 0;
  const idat = [];
  while (pos < buf.length) {
    const len = buf.readUInt32BE(pos);
    const type = buf.toString("latin1", pos + 4, pos + 8);
    const data = buf.subarray(pos + 8, pos + 8 + len);
    if (type === "IHDR") {
      w = data.readUInt32BE(0); h = data.readUInt32BE(4);
      depth = data[8]; colour = data[9];
    } else if (type === "IDAT") idat.push(data);
    pos += 12 + len;
  }
  if (depth !== 8 || colour !== 0) throw new Error("not an 8-bit greyscale PNG");
  const raw = zlib.inflateSync(Buffer.concat(idat));
  const px = new Uint8Array(w * h);
  const prev = new Uint8Array(w), cur = new Uint8Array(w);
  for (let y = 0; y < h; y++) {
    const f = raw[y * (w + 1)];
    const row = raw.subarray(y * (w + 1) + 1, (y + 1) * (w + 1));
    for (let x = 0; x < w; x++) {
      const a = x > 0 ? cur[x - 1] : 0, b = prev[x], c = x > 0 ? prev[x - 1] : 0;
      let v = row[x];
      if (f === 1) v += a;
      else if (f === 2) v += b;
      else if (f === 3) v += (a + b) >> 1;
      else if (f === 4) {
        const p = a + b - c, pa = Math.abs(p - a), pb = Math.abs(p - b), pc = Math.abs(p - c);
        v += pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
      }
      cur[x] = v & 0xff;
    }
    for (let x = 0; x < w; x++) px[y * w + x] = Math.round(cur[x] / 17);
    prev.set(cur);
  }
  return { w, h, px };
}

export function loadData({ images = true } = {}) {
  const root = path.join(ASSETS, "data");
  vfs.setManifest(JSON.parse(fs.readFileSync(path.join(ASSETS, "manifest.json"), "utf8")));
  const walk = (dir) => {
    for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
      const p = path.join(dir, e.name);
      const rel = path.relative(root, p);
      if (e.isDirectory()) walk(p);
      else if (/\.png$/i.test(e.name)) {
        if (images) vfs.installImage(rel.slice(0, -4), decodePng(fs.readFileSync(p)));
      } else if (!/\.wav$/i.test(e.name)) {
        vfs.install(rel, new Uint8Array(fs.readFileSync(p)));
      }
    }
  };
  walk(root);
}
