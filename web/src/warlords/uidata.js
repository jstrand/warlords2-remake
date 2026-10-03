// The original's screen layout, read from its own data files.
//
// docs/formats/screens.md. DATA/JOIN.DAT names, for each dialog, one group
// of controls in BUTTON.DAT and one set of clickable regions in AREA.DAT.
// DATA/FILE.DAT group 3 turns a control's bitmap id into a .pck file name.

import * as vfs from "../vfs.js";
import { u16 } from "./bytes.js";
import { cstr } from "./bytes.js";

function read(path) {
  const s = vfs.read(path);
  if (!s) throw new Error("cannot open: " + path);
  return s;
}

/** A file in STRING.DAT's layout: an array of groups, each an array of
 *  strings, both from 0. docs/formats/string.md. */
export function strings(path) {
  const s = read(path);
  const nGroups = u16(s, 0) / 4;              // the first offset IS the index size
  const groups = [];
  for (let g = 0; g < nGroups; g++) {
    const off = u16(s, g * 4), count = u16(s, g * 4 + 2);
    const out = [];
    for (let i = 0; i < count; i++) {
      const at = u16(s, off + i * 2);
      out.push(cstr(s, at, s.length - at));
    }
    groups.push(out);
  }
  return groups;
}

const MAGIC = 0x7d00;

/** DATA/JOIN.DAT: {dialog id: { button, area }}. */
export function joins(path) {
  const s = read(path);
  const out = {};
  for (let i = 0; i < Math.floor(s.length / 6); i++) {
    const at = i * 6;
    out[u16(s, at)] = { button: u16(s, at + 2), area: u16(s, at + 4) };
  }
  return out;
}

/** DATA/AREA.DAT: screens of clickable regions, 14 bytes each. */
export function areas(path) {
  const s = read(path);
  const out = {};
  let at = 0;
  while (at + 13 < s.length) {
    if (u16(s, at) !== MAGIC) throw new Error("AREA.DAT: lost the record boundary");
    const id = u16(s, at + 2), enabled = u16(s, at + 4), count = u16(s, at + 6);
    const regions = [];
    for (let i = 0; i < count; i++) {
      const r = at + 14 + i * 14;
      regions.push({ id: u16(s, r), x: u16(s, r + 4), y: u16(s, r + 6), w: u16(s, r + 8), h: u16(s, r + 10) });
    }
    out[id] = { id, enabled, regions };
    at += 14 + count * 14;
  }
  return out;
}

// A control's three source rects are indexed by its state: 1 is the resting
// look, 0 the lit one and 2 the disabled one.
export const ACTIVE = 0, NORMAL = 1, DISABLED = 2;

/** DATA/BUTTON.DAT: groups of controls, 33 bytes each. */
export function buttons(path) {
  const s = read(path);
  const out = {};
  let at = 0;
  while (at + 32 < s.length) {
    if (u16(s, at) !== MAGIC) throw new Error("BUTTON.DAT: lost the record boundary");
    const id = u16(s, at + 2), count = u16(s, at + 4);
    const controls = [];
    for (let i = 0; i < count; i++) {
      const c = at + 33 + i * 33;
      const src = [];
      for (let st = 0; st <= 2; st++) src[st] = { x: u16(s, c + 19 + st * 4), y: u16(s, c + 21 + st * 4) };
      controls.push({
        id: u16(s, c), x: u16(s, c + 6), y: u16(s, c + 8),
        w: s[c + 10], h: s[c + 11],
        src, bitmap: u16(s, c + 31),
      });
    }
    out[id] = { id, controls };
    at += 33 + count * 33;
  }
  return out;
}

/** UDB/UDB.DAT: the menu items that may be put on the four configurable
 *  buttons, {id: { name, src: [lit, resting, greyed], w, h }}. */
export function shortcutItems(path) {
  const s = read(path);
  const out = {};
  for (let i = 0; (i + 1) * 68 <= s.length; i++) {
    const at = i * 68;
    const id = u16(s, at + 2);
    const name = cstr(s, at + 4, 50);
    const x = u16(s, at + 54), y = u16(s, at + 56);
    if (name.length > 0) {
      out[id] = {
        name,
        w: u16(s, at + 66), h: u16(s, at + 60),
        src: [
          { x: x + u16(s, at + 66), y },             // ACTIVE
          { x, y },                                 // NORMAL
          { x: u16(s, at + 62), y: u16(s, at + 64) },   // DISABLED
        ],
      };
    }
  }
  return out;
}

/** UDB/UDB.CUR: which menu item sits on each of the four buttons. */
export function shortcuts(path) {
  const s = read(path);
  const out = {};
  for (let i = 0; i < Math.floor(s.length / 2); i++) out[i] = u16(s, i * 2);
  return out;
}

// The four configurable buttons, in order (545c:0072).
export const SHORTCUT_FIRST = 179, SHORTCUT_COUNT = 4;
// Their art is bitmap 43, MENUBUTT.PCK, at the rect the item carries.
export const SHORTCUT_BITMAP = 43;

/** Load every layout file under `dataDir`. */
export function load(dataDir) {
  const d = dataDir + "/DATA/";
  const files = strings(d + "FILE.DAT");
  const bitmaps = {};
  files[3].forEach((name, i) => { bitmaps[i] = name.toLowerCase(); });
  let sc = {}, items = {};
  try {
    items = shortcutItems(dataDir + "/UDB/UDB.DAT");
    sc = shortcuts(dataDir + "/UDB/UDB.CUR");
  } catch (e) {
    sc = {}; items = {};
  }
  // the player's own assignment, kept in the browser (ui/shortcuts.js)
  try {
    const saved = localStorage.getItem("w2:shortcuts");
    if (saved) sc = JSON.parse(saved);
  } catch (e) { /* none */ }
  const names = {};
  for (const id in items) names[id] = items[id].name;
  return {
    shortcuts: sc,
    shortcutItems: items,
    shortcutNames: names,
    joins: joins(d + "JOIN.DAT"),
    areas: areas(d + "AREA.DAT"),
    buttons: buttons(d + "BUTTON.DAT"),
    bitmaps,
    files,
    // DATA/STRING.DAT: the game's whole text corpus; get_string(group, i) in
    // the executable is uidata.text(ui, group, i) here, both from 0.
    text: strings(d + "STRING.DAT"),
  };
}

/** One string, numbered as the executable numbers them (both from 0). */
export function text(ui, group, index) {
  const g = ui.text[group];
  return (g && g[index]) || "";
}

/** The controls and regions of one dialog, following JOIN.DAT. */
export function dialog(ui, id) {
  const join = ui.joins[id];
  if (!join) return null;
  const b = ui.buttons[join.button], a = ui.areas[join.area];
  return {
    id,
    controls: b ? b.controls : [],
    regions: a ? a.regions : [],
    enabled: a ? a.enabled : 0,
  };
}

export const MAIN_SCREEN = 0;
