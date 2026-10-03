// Warlords II .PAL reader: plain ASCII, 16 lines of "RR GG BB", the
// components PERCENTAGES (0-99), not VGA 0-63. See docs/formats/pck.md.

import * as vfs from "../vfs.js";

/** The palette as 16 [r, g, b] in 0..1, indexed from 0. */
export function load(path) {
  const text = vfs.readText(path);
  if (text === null) throw new Error("cannot open palette: " + path);
  const colours = [];
  const re = /(\d+)\s+(\d+)\s+(\d+)/g;
  let m;
  while ((m = re.exec(text)) !== null) {
    colours.push([Number(m[1]) / 99, Number(m[2]) / 99, Number(m[3]) / 99]);
  }
  if (colours.length !== 16) throw new Error(`${path}: expected 16 entries, got ${colours.length}`);
  return colours;
}
