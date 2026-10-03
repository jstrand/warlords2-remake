// Where the main screen's pieces go on a screen of any size.
//
// Not the original's: it only ever had 640x480. This keeps every piece of
// that screen at its own size and moves it as a whole, tied to an edge of the
// bigger screen, and gives all the room left over to the map. The popups are
// drawn in a 640x480 frame of their own, centred, so every dialog keeps the
// coordinates the original gives it. At 640x480 every offset is zero.

export const W = 640, H = 480;

const MAP = { x: 16, y: 30, w: 360, h: 360 };      // region 2
const PANEL = { x: 0, y: 17, w: 392, h: 386 };     // region 13, the map's frame
const STRAT = { x: 400, y: 30, w: 224, h: 312 };   // region 1
const BAR = { x: 16, y: 408, w: 360, h: 56 };      // region 9

/** The layout of a screen `w` x `h` UI pixels. */
export function compute(w, h) {
  w = Math.max(W, Math.floor(w));
  h = Math.max(H, Math.floor(h));
  const ex = w - W, ey = h - H;
  const L = {
    w, h, ex, ey,
    classic: ex === 0 && ey === 0,
    offset: {
      strat: { x: ex, y: 0 },
      panel: { x: ex, y: ey },
      bar: { x: Math.floor(ex / 2), y: ey },
      map: { x: 0, y: 0 },
    },
    dialog: { x: Math.floor(ex / 2), y: Math.floor(ey / 2) },
  };
  L.map = { x: MAP.x, y: MAP.y, w: MAP.w + ex, h: MAP.h + ey };
  L.panel = { x: PANEL.x, y: PANEL.y, w: PANEL.w + ex, h: PANEL.h + ey };
  L.strat = move(L, "strat", STRAT);
  L.bar = move(L, "bar", BAR);
  return L;
}

/** Which group a rect of the original screen belongs to. */
export function group(r) {
  if (r.x >= 392) return r.y < 348 ? "strat" : "panel";
  if (r.y >= 396) return "bar";
  return "map";
}

/** A copy of the original rect `r`, moved with group `g`. */
export function move(L, g, r) {
  const o = L.offset[g];
  return Object.assign({}, r, { x: r.x + o.x, y: r.y + o.y });
}

/** One of the main screen's regions, placed: the map, its frame and the menu
 *  bar stretch, the rest move with their group. */
export function region(L, r) {
  const out = move(L, group(r), r);
  if (r.id === 2) Object.assign(out, L.map);
  else if (r.id === 13) Object.assign(out, L.panel);
  else if (r.id === 3) { out.x = r.x; out.y = r.y; out.w = L.w; }
  return out;
}
