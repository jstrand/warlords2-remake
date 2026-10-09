// TERRAIN0/ARMYTYPE.DAT -- the 29 army types.
// See docs/formats/armytype.md. Records are keyed by their type id (+0), not
// by position in the file: the file is in display order.

import * as vfs from "../vfs.js";
import { u16, i16, cstr } from "./bytes.js";

const COUNT = 29, STRIDE = 62;
const N_BONUS = 15;

// bonus field offsets worth naming (docs/formats/armytype.md, docs/rules.md)
export const SIEGE = 52;   // value 1 = siege ability (+2 strength vs cities)
export const FLIES = 54;
export const WOODS_MOVE = 56;
export const HILLS_MOVE = 58;
export const BOAT = 60;

export const NAVY = 5;     // removed from every city's production at game start
export const HERO = 28;
export const SCOUTS = 11;  // placeholder garrison when Neutral Cities is off

/** Load ARMYTYPE.DAT: { byId: {[id]: rec}, list: [rec...] }, the list in file
 *  (display) order. */
export function load(path) {
  const s = vfs.read(path);
  if (!s) throw new Error("cannot open: " + path);
  if (s.length !== COUNT * STRIDE) throw new Error(`unexpected ${path} size: ${s.length}`);
  const byId = {}, list = [];
  for (let i = 0; i < COUNT; i++) {
    const o = i * STRIDE;
    const bonus = {};
    // signed: the Elephants' -1 to the enemy stack (+50) is 0xffff
    for (let b = 0; b < N_BONUS; b++) bonus[32 + b * 2] = i16(s, o + 32 + b * 2);
    const a = {
      id: u16(s, o),               // also the sprite index
      name: cstr(s, o + 2, 16),
      strength: u16(s, o + 22),
      time: u16(s, o + 24),
      cost: u16(s, o + 26),        // per-turn upkeep basis
      move: u16(s, o + 28),
      price: i16(s, o + 30),       // negative: can never be bought
      bonus,
    };
    a.flies = bonus[FLIES] !== 0;
    a.siege = bonus[SIEGE] === 1;
    a.boat = bonus[BOAT] !== 0;
    a.woodsMove = bonus[WOODS_MOVE] !== 0;
    a.hillsMove = bonus[HILLS_MOVE] !== 0;
    byId[a.id] = a;
    list.push(a);
  }
  return { byId, list };
}
