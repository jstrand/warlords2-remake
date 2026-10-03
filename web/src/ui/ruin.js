// View > Ruins (`.`): the city dialog's fifth mode, on a ruin or temple.
//
// 7204:0000 with mode 4 at the cursor takes the nearest site shown to the
// side (828e:06fd). Dialog 6 over popup 2 with only Done (192); the map with
// every site shown marked and this one boxed; and 7204:0cdf's case 4: the
// name, SPECBITS.PCK's picture, "Type: ..." and "Explored: ...", the map
// markers' legend, and the site's three lines of .SPC description.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as infobox from "./infobox.js";
import * as game from "../warlords/game.js";
import * as site from "../warlords/site.js";
import * as uidata from "../warlords/uidata.js";

const R = { x: 80, y: 60, w: 480, h: 312 };      // popup 2
const MAP = { x: 80, y: 60 };
const DIALOG = 6, DONE = 192;
const PICTURES = [[0, 0], [96, 0], [192, 0], [0, 63], [96, 63], [192, 63]];
const LEGEND = [[112, 0], [112, 10], [112, 20], [128, 0]];     // 4125:0f28
const LEGEND_AT = [[320, 188], [432, 188], [320, 208], [432, 208]];

/** The site nearest (x, y) that the side may look at, or null. */
export function nearest(x, y) {
  const G = kit.G;
  let best = null, bestD = null;
  for (const s of G.g.map.sites) {
    if (site.shownTo(s, G.player) && game.seen(G.g, G.player, s.x, s.y)) {
      const dd = Math.floor(Math.sqrt((s.x - x) ** 2 + (s.y - y) ** 2));
      if (bestD === null || dd < bestD) { best = s; bestD = dd; }
    }
  }
  return best;
}

export function open(s) {
  const G = kit.G;
  if (!s) return null;
  const d = { view: kit.view(DIALOG) };
  d.view.state[DONE] = uidata.NORMAL;
  d.hidden = {};
  for (const c of d.view.dialog.controls) if (c.id !== DONE) d.hidden[c.id] = true;

  d.draw = () => {
    kit.popup(R);
    G.drawStrategicMap(MAP.x, MAP.y);
    G.drawSiteMarkers(MAP.x, MAP.y, s);
    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), s.name || "", 432, 62);
    kit.bevel(326, 102, 100, 67, 4, 2);
    kit.setPal(0);
    kit.outline(327, 103, 98, 65);
    const pic = PICTURES[s.content === site.TEMPLE ? 0 : (s.index % 5) + 1];
    const art = G.screen.art_for(6);
    if (art) {
      gfx.setColor(1, 1, 1);
      gfx.draw(art.image, gfx.newQuad(pic[0], pic[1], 96, 63), 328, 104);
    }
    const f = kit.font(2);
    gfx.setColor(1, 1, 1);
    f.draw(kit.text(0x71, s.content || 0), 432, 112);
    f.draw(kit.text(0x72, s.searched ? 1 : 0), 432, 136);
    kit.setPal(2);
    gfx.rectangle("fill", 312, 180, 240, 48);
    kit.setPal(0);
    kit.outline(312, 180, 240, 48);
    LEGEND.forEach((src, i) => {
      const at = LEGEND_AT[i];
      gfx.setColor(1, 1, 1);
      gfx.draw(G.atransShields, gfx.newQuad(src[0], src[1], 16, 10), at[0], at[1]);
      f.draw(kit.text(0x73, i), at[0] + 16, at[1] - 3);
    });
    const lines = G.g.map.siteText[s.index] || [];
    for (let i = 0; i < 3; i++) f.draw(lines[i] || "", 310, 259 + 20 * i);
    kit.drawControls(d.view, d.hidden);
  };

  const onMap = (x, y) => x >= MAP.x && x < MAP.x + 224 && y >= MAP.y && y < MAP.y + 312;

  // a click on the map moves to the site nearest it (7204:1afa)
  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (c && c.id === DONE) { kit.pop(d); return; }
    if (!c && onMap(x, y)) {
      const other = nearest(Math.floor((x - MAP.x) / 2), Math.floor((y - MAP.y) / 2));
      if (other && other !== s) {
        kit.pop(d);
        open(other);
      }
    }
  };

  d.rightpressed = (x, y, sx, sy) => {
    if (onMap(x, y)) infobox.lines(sx, sy, kit.text(0x7a, 2), kit.text(0x7a, 3));
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") kit.pop(d);
  };
  return kit.push(d);
}
