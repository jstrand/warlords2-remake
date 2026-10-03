// Node's way into the game's files: web/assets/data, read straight off disk
// into the vfs, so the rules core runs here as it does in the browser. The
// images and sounds are left out; nothing headless draws or plays.
import * as fs from "node:fs";
import * as path from "node:path";
import { fileURLToPath } from "node:url";
import * as vfs from "../src/vfs.js";

const here = path.dirname(fileURLToPath(import.meta.url));
export const ASSETS = path.join(here, "..", "assets");

export function loadData() {
  const root = path.join(ASSETS, "data");
  const walk = (dir) => {
    for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
      const p = path.join(dir, e.name);
      if (e.isDirectory()) walk(p);
      else if (!/\.(png|wav)$/i.test(e.name)) {
        vfs.install(path.relative(root, p), new Uint8Array(fs.readFileSync(p)));
      }
    }
  };
  walk(root);
}
