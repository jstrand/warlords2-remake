// A help screen from a .GFX file (8065:168d), e.g. HELP\HITEM.GFX.
//
// The file's #D number picks the popup -- 14 + n -- and 7ecb:06de lays the
// page out inside it, every position counted from the popup's corner:
//
//   #H          font 1, colour 15        #T      font 2, colour 15
//   #Fnnn       font 2, colour nnn       #E      the end
//   #C(x,y)|t|  centred on x             #L(x,y)|t|  from x
//   #R(x,y)|t|  ending at x
//   #G(x,y,w,h)nnn(dx,dy)  the (x, y) w x h of bitmap nnn, at (dx, dy)
//
// A key or a click puts it away. The control panel's "?" shows HMOUSE and
// then HKEYS in popup 4, (120, 50) 400x360.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as vfs from "../vfs.js";

const POPUPS = {
  0: { x: 256, y: 40, w: 352, h: 340 },       // popup 14
  1: { x: 32, y: 40, w: 352, h: 340 },        // popup 15
};

/** The page's drawing steps: [page, steps]. */
export function parse(text) {
  let page = 0;
  const steps = [];
  for (const line of text.split(/[\r\n]+/)) {
    const m = line.match(/^#([A-Z])(.*)$/);
    if (!m) continue;
    const [, op, rest] = m;
    if (op === "D") {
      const n = rest.match(/^(\d+)/);
      page = n ? Number(n[1]) : 0;
    } else if (op === "H") steps.push({ font: 1, colour: 15 });
    else if (op === "T") steps.push({ font: 2, colour: 15 });
    else if (op === "F") {
      const n = rest.match(/^(\d+)/);
      steps.push({ font: 2, colour: n ? Number(n[1]) : 15 });
    } else if (op === "C" || op === "L" || op === "R") {
      const t = rest.match(/^\((\d+),(\d+)\)\|(.*?)\|/);
      if (t) steps.push({ text: t[3], align: op, x: Number(t[1]), y: Number(t[2]) });
    } else if (op === "G") {
      const t = rest.match(/^\((\d+),(\d+),(\d+),(\d+)\)(\d+)\((\d+),(\d+)\)/);
      if (t) {
        steps.push({ bitmap: Number(t[5]), sx: Number(t[1]), sy: Number(t[2]),
                     w: Number(t[3]), h: Number(t[4]), x: Number(t[6]), y: Number(t[7]) });
      }
    } else if (op === "E") break;
  }
  return [page, steps];
}

export const POPUP4 = { x: 120, y: 50, w: 400, h: 360 };

/** Show the help file `name` -- "HELP\HITEM.GFX" -- in popup `R` or the one
 *  its #D picks, then run `after` when it is put away. */
export function open(name, after, R) {
  const G = kit.G;
  const text = vfs.readText(G.dataDir + "/" + name.replace(/\\/g, "/"));
  if (text === null) return null;
  const [page, steps] = parse(text);
  R = R || POPUPS[page] || POPUPS[0];

  const d = {};
  const close = () => { kit.pop(d); if (after) after(); };

  d.draw = () => {
    kit.popup(R);
    let font = kit.font(2), colour = 15;
    for (const s of steps) {
      if (s.font) {
        font = kit.font(s.font); colour = s.colour;
      } else if (s.text !== undefined) {
        const fc = font.colours(colour, 0);
        gfx.setColor(1, 1, 1);
        if (s.align === "C") kit.centred(fc, s.text, R.x + s.x, R.y + s.y);
        else if (s.align === "R") kit.right(fc, s.text, R.x + s.x, R.y + s.y);
        else fc.draw(s.text, R.x + s.x, R.y + s.y);
      } else if (s.bitmap !== undefined) {
        const art = G.screen.art_for(s.bitmap);
        if (art) {
          gfx.setColor(1, 1, 1);
          gfx.draw(art.image, gfx.newQuad(s.sx, s.sy, s.w, s.h), R.x + s.x, R.y + s.y);
        }
      }
    }
  };
  d.mousepressed = () => close();
  d.keypressed = () => close();
  return kit.push(d);
}
