// SSG > About Warlords II, and the "?" key (7721:0084).
//
// Popup 23, (160, 55) 336x347, BSCROLL.PCK through its mask and with no
// frame. 8065:1471 writes up to six lines of group 139 centred on x = 328,
// black edged in colour 7: "", "Version 1.02", "", and three lines of DOS
// memory the remake has none of. Any key or click puts it away.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as pck from "../warlords/pck.js";

const R = { x: 160, y: 55, w: 336, h: 347 };     // popup 23

export function open() {
  const G = kit.G;
  const d = {};
  const lines = [kit.text(0x8b, 1), "Version 1.02", kit.text(0x8b, 3)];   // 4125:1292
  if (!G.bscroll) G.bscroll = pck.toImage(G.dataDir + "/PICS/BSCROLL.PCK", G.palette, 10)[0];
  d.draw = () => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.bscroll, gfx.newQuad(0, 0, R.w, R.h), R.x, R.y);
    const f = kit.font(2).colours(0, 7);
    lines.forEach((line, i) => {
      if (line !== "") kit.centred(f, line, 328, 141 + 20 * (i + 1));
    });
  };
  d.mousepressed = () => kit.pop(d);
  d.keypressed = () => kit.pop(d);
  return kit.push(d);
}
