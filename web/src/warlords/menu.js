// The original's menu bar.
//
// The menu lives in WARLORD2.EXE's data segment, not in a data file, so it is
// written out here (docs/re/ui.md > The menu). Layout follows 7ae8:0052 and
// 2372:049b; nothing is hard-coded, so the bar measures itself for its font.

export const BAR_X = 8, BAR_Y = 1;

// "-" is a separator. The key is the accelerator, and is what the front end
// dispatches on; a third field is what an item with no accelerator does.
export const MENUS = [
  { title: "SSG", items: [["About Warlords II", null, "?"]] },
  { title: "Game", items: [
    ["Settings", "alt X"], ["Shortcuts", "alt U"], ["-"],
    ["New game", "alt N"], ["Save game", "alt S"], ["Load game", "alt L"],
    ["-"],
    ["Save map", "alt M"], ["Load map", "alt Z"], ["-"],
    ["Quit", "^Q"],
  ] },
  { title: "Order", items: [
    ["Fight Order", "i"], ["Move All", "m"], ["Disband", "q"],
    ["Signpost", "x"], ["-"], ["Resign", "r"],
  ] },
  { title: "Report", items: [
    ["Army", "a"], ["City", "k"], ["Gold", "g"],
    ["Production", "n"], ["Winning", "w"], ["-"],
    ["Diplomacy", "d"], ["-"], ["Quest", "="],
  ] },
  { title: "Hero", items: [
    ["Inspect", ","], ["Plant Flag", "f"], ["Levels", "u"], ["Search", "z"],
  ] },
  { title: "View", items: [
    ["Army Bonus", "o"], ["Items", "t"], ["-"],
    ["Build", "b"], ["Cities", "c"], ["Production", "p"],
    ["Vectoring", "v"], ["Ruins", "."], ["Stack", "s"],
  ] },
  { title: "History", items: [
    ["City", "h"], ["Events", "e"], ["Gold", "j"],
    ["Winners", "y"], ["-"], ["Triumphs", "l"],
  ] },
  { title: "Turn", items: [["End Turn", "alt E"]] },
];

/** The menus with the screen's zooms added to View, and the music's
 *  synthesizer to Game -- not the original's. */
export function withZooms(maxUI, maxMap) {
  return MENUS.map((m) => {
    let items = m.items;
    if (m.title === "Game") {
      items = [];
      m.items.forEach((it, j) => {
        items.push(it);
        if (j === 1) {
          items.push(["-"]);
          items.push(["AdLib music", null, "music fm"]);
          items.push(["MT-32 music", null, "music mt32"]);
          items.push(["SC-55 music", null, "music sc55"]);
        }
      });
    } else if (m.title === "View") {
      items = m.items.slice();
      items.push(["-"]);
      for (let z = 1; z <= maxMap; z++) items.push([`Map ${z}x`, null, "map zoom " + z]);
      items.push(["-"]);
      for (let s = 1; s <= maxUI; s++) items.push([`Interface ${s}x`, null, "ui scale " + s]);
      items.push(["-"]);
      items.push(["Full screen", null, "screen full"]);
      items.push(["Window", null, "screen window"]);
      items.push(["-"]);
      items.push(["4:3 (original)", null, "screen 4:3"]);
    }
    return { title: m.title, items };
  });
}

// The bar is 17 pixels deep and white (7ae8:02d8).
export const BAR_H = 17;

/** Measure the bar and every dropdown for a font (7ae8:0052, 2372:049b). */
export function layout(font, barHeight, screenWidth, menus) {
  const out = [];
  let x = BAR_X;
  const lh = font.lineHeight;
  for (const m of menus || MENUS) {
    const tw = Math.ceil(font.width(m.title) / 8) * 8;
    let w = 0, keyCol = 0, keyW = 0;
    for (const it of m.items) {
      const [label, key] = it;
      if (label !== "-") {
        let lw = 12 + font.width(label);
        if (key) {
          keyCol = Math.max(keyCol, lw + 5);
          keyW = Math.max(keyW, font.width(key));
          lw = keyCol + keyW;
        }
        w = Math.max(w, lw + 5);
      }
    }
    const y = BAR_H + 1;
    const rows = [];
    let at = 2;
    for (const it of m.items) {
      const h = it[0] === "-" ? 2 : lh + 2;
      rows.push({ label: it[0], key: it[1] || null, act: it[2] || null, x, y: y + at, w, h });
      at += h;
    }
    const h = at + 1;
    const dropX = Math.min(x, screenWidth - w);
    for (const r of rows) r.x = dropX;
    out.push({
      title: m.title, items: m.items, x, w: tw + 8,
      drop: { x: dropX, y, w, h, keyCol, rows },
    });
    x += tw + 16;
  }
  return out;
}

/** Which menu title is at this point (an index from 0), or null. */
export function titleAt(lay, x, y, barHeight) {
  if (y < 0 || y >= barHeight) return null;
  for (let i = 0; i < lay.length; i++) {
    const m = lay[i];
    if (x >= m.x && x < m.x + m.w) return i;
  }
  return null;
}

/** Which row of an open menu is at this point, or null. */
export function rowAt(lay, index, x, y) {
  const m = lay[index];
  if (!m) return null;
  for (const r of m.drop.rows) {
    if (r.label !== "-" && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return r;
  }
  return null;
}
