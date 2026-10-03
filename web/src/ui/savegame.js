// Game > Save game and Load game (7721:093b, 7721:026b).
//
// Ten slots, each with a name, as SAVEINFO.DAT has them -- "Not_Used" for an
// empty one. Save lists all ten in the list chooser titled "Save Game"
// (group 46), on the slot last used; the one chosen asks for its name -- 15
// characters, 160 pixels -- and is written (7721:0995). Load lists the slots
// in use (group 47) and tells the side whose turn it is that "thy turn
// continues!". The web port keeps the slots in the browser's localStorage.

import * as kit from "./kit.js";
import * as chooseUi from "./choose.js";
import * as input from "./input.js";
import * as searchUi from "./search.js";
import * as saveMod from "../warlords/save.js";
import { fmt } from "../util.js";

const SLOTS = 10;
const UNUSED = "Not_Used";
const INFO = "w2:saveinfo";

const slotKey = (i) => "w2:save" + i;

function readInfo() {
  let names = [];
  try { names = JSON.parse(localStorage.getItem(INFO) || "[]"); } catch (e) { names = []; }
  for (let i = names.length; i < SLOTS; i++) names[i] = UNUSED;
  return names.slice(0, SLOTS);
}

function writeInfo(names) {
  try { localStorage.setItem(INFO, JSON.stringify(names)); } catch (e) { /* no storage */ }
}

/** Write a game into a slot of the browser's storage; false if it would not
 *  fit. */
export function writeSlot(key, g) {
  try {
    localStorage.setItem(key, saveMod.encode(g));
    return true;
  } catch (e) {
    return false;
  }
}

/** Read a game back, or null. */
export function readSlot(key, dataDir) {
  try {
    const text = localStorage.getItem(key);
    return text ? saveMod.decode(text, dataDir) : null;
  } catch (e) {
    return null;
  }
}

export let last = 0;

/** How many slots hold a game (7721:0e25). */
export function used() {
  return readInfo().filter((n) => n !== UNUSED).length;
}

export function save() {
  const G = kit.G;
  const names = readInfo();
  const list = names.map((name, i) => ({ name, slot: i }));
  chooseUi.open(kit.text(0x2e, 0), list, last, (e) => {
    if (!e) return;
    last = e.slot;
    input.open({
      title: kit.text(0x2e, 0), lines: [kit.text(0x2e, 1), kit.text(0x2e, 2)],
      text: e.name !== UNUSED ? e.name : "", maxChars: 15, maxWidth: 160,
      ok: (name) => {
        if (name === "") name = UNUSED;
        if (!writeSlot(slotKey(e.slot), G.g)) {
          searchUi.message("The game could not be saved:", "the browser's storage is full.");
          return;
        }
        names[e.slot] = name;
        writeInfo(names);
      },
    });
  });
}

/** `loaded(g)` takes the game read back. */
export function load(loaded) {
  const names = readInfo();
  const list = [];
  names.forEach((name, i) => { if (name !== UNUSED) list.push({ name, slot: i }); });
  if (list.length === 0) return null;
  let start = 0;
  list.forEach((e, k) => { if (e.slot === last) start = k; });
  chooseUi.open(kit.text(0x2f, 0), list, start, (e) => {
    if (!e) return;
    const g = readSlot(slotKey(e.slot), kit.G.dataDir);
    if (!g) return;
    last = e.slot;
    loaded(g);
    const side = g.sides[g.current];
    searchUi.say(fmt(kit.text(0x2f, 1), side ? side.name : ""));
  });
}
