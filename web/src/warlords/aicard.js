// The computer players' character cards: CARDS/K|L|W nnn.CRD and .DSC.
//
// docs/formats/crd.md. A computer side's level picks the deck (K Knight,
// L Lord, W Warlord) and .SCN 0x00e0 + 2*side the card; at game start the
// card overwrites the level's built-in AI settings (59bf:0d7b).

import * as vfs from "../vfs.js";
import { i16 } from "./bytes.js";
import { fmt } from "../util.js";

export const LETTERS = ["K", "L", "W"];       // "KLW", 7bab:21c1
export const RECORD = 0x67;                   // 103 bytes

/** The file name of a card: CARDS\%c%03d.%s. */
export function path(dataDir, level, number, ext) {
  return fmt("%s/CARDS/%s%03d.%s", dataDir, LETTERS[level] || "W", number || 0, ext || "CRD");
}

/** Read a card, or null when the file is missing or short. */
export function load(dataDir, level, number) {
  const s = vfs.read(path(dataDir, level, number, "CRD"));
  if (!s || s.length < RECORD) return null;
  const fightOrder = [];
  for (let t = 0; t <= 28; t++) fightOrder[t] = s[0x0e + t];
  return {
    groups: i16(s, 0x08),
    bold: i16(s, 0x0a),
    fightOrder,
    cautious: i16(s, 0x2b),
    rebuildType: i16(s, 0x2d),
    rebuildTypeRich: i16(s, 0x2f),
    rebuildLimit: i16(s, 0x31),
    raze: i16(s, 0x45),
    sack: i16(s, 0x47),
    pillage: i16(s, 0x49),
    perCity: i16(s, 0x4b),
    bonusHuman: i16(s, 0x4d),
    bonusKnight: i16(s, 0x4f),
    bonusLord: i16(s, 0x51),
    bonusWarlord: i16(s, 0x53),
    dieHuman: i16(s, 0x55),
    dieKnight: i16(s, 0x57),
    dieLord: i16(s, 0x59),
    dieWarlord: i16(s, 0x5b),
    poor: i16(s, 0x5f),
    early: i16(s, 0x61),
    humanShare: i16(s, 0x63),
    solidarity: i16(s, 0x65),
  };
}

/** A card's name and description lines, from its .DSC: [name, lines], or
 *  null when there is none. */
export function describe(dataDir, level, number) {
  const s = vfs.readText(path(dataDir, level, number, "DSC"));
  if (s === null) return null;
  const lines = [];
  const re = /([^\r\n]*)\r?\n/g;
  let m;
  const text = s + "\n";
  while ((m = re.exec(text)) !== null) lines.push(m[1]);
  const name = lines.shift() || "";
  while (lines.length > 0 && lines[lines.length - 1] === "") lines.pop();
  return [name, lines];
}

/** How many cards a level has on disk, 000 up to the first missing one. */
export function count(dataDir, level) {
  let n = 0;
  while (n < 100 && vfs.exists(path(dataDir, level, n, "CRD"))) n++;
  return n;
}
