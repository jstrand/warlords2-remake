// The right button on the map: what is on a tile (740d:0037, button 2).
//
// Only on a tile the side has seen, and gone when the button comes up
// (740d:11cf). The box is 256x75, centred on the tile (740d:131a), with
// POPUP.PCK behind it through its mask. What it shows, first that applies:
// a stack the side may see, side by side (740d:0bad) -- an enemy in a tower
// only "Tower / Very hard to conquer!"; a city (740d:0c73) with its shields,
// income and defence; a site (740d:0fd7); a signpost over POPUP2.PCK
// (740d:0a1c); else the terrain (740d:10e1).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as infobox from "./infobox.js";
import * as game from "../warlords/game.js";
import * as move from "../warlords/move.js";
import * as scn from "../warlords/scn.js";
import * as siteMod from "../warlords/site.js";
import { fmt } from "../util.js";

const W = 256, H = 75;

function box(tx, ty) {
  const G = kit.G;
  const L = G.layout;
  let [cx, cy] = G.mapToUI(tx + 0.5, ty + 0.5);
  cx = Math.floor(cx); cy = Math.floor(cy);
  cx = Math.floor((cx + 4) / 8) * 8;
  cx = Math.max(W / 2, Math.min(L.w - W / 2, cx));
  cy = Math.max(Math.floor(H / 2), Math.min(L.h - 2 - Math.floor(H / 2), cy));
  return [cx - W / 2 - L.dialog.x, cy - Math.floor(H / 2) - L.dialog.y];
}

/** Show what is on (tx, ty); null when the side has not seen it. */
export function open(tx, ty) {
  const G = kit.G;
  const g = G.g;
  if (!game.seen(g, G.player, tx, ty)) return null;
  const [bx, by] = box(tx, ty);
  const me = G.player;
  const d = {};
  let back = 29, draw;
  const f = kit.font(2);

  const armies = game.armiesAt(g, tx, ty);
  const owner = armies[0] ? armies[0].owner : undefined;
  const tower = g.towerAt && g.towerAt[ty * g.map.width + tx];
  const city = game.cityAt(g, tx, ty);
  const site = g.map.siteAt ? g.map.siteAt[ty * g.map.width + tx] : null;
  const terrain = scn.terrainAt(g.map, tx, ty);

  const side = (i) => g.map.sides[i ?? 8];
  const colours = (s) => {
    s = s || me;
    return f.colours(s.colour ?? 15, s.edge ?? 0);
  };
  const twoLines = (a, b, fa, fb) => {
    kit.centred(fa || f.colours(7, 0), a || "", bx + W / 2 - 8, by + 11);
    kit.centred(fb || f, b || "", bx + W / 2 - 8, by + 35);
  };

  if (armies.length > 0 && (g.map.options.viewEnemies !== 0 || owner === me.index || tower)) {
    if (owner !== me.index && tower && g.map.options.viewEnemies === 0) {
      draw = () => twoLines(kit.text(0x82, 0), kit.text(0x82, 1));
    } else {
      draw = () => {
        const n = armies.length;
        const x = Math.floor((bx + W / 2 - n * 12 - 16 + 4) / 8) * 8;
        armies.forEach((a, i) => kit.army(a.type, a.owner, x + 24 * i, by + 16, 0));
      };
    }
  } else if (city) {
    draw = () => {
      const own = city.ownerIndex != null ? side(city.ownerIndex) : me;
      kit.centred(colours(own), city.name || "", bx + W / 2 - 8, by + 11);
      if (city.razed) {
        kit.centred(f, "Razed!", bx + W / 2 - 8, by + 35);
        return;
      }
      if (city.ownerIndex != null && city.ownerIndex < 8) {
        for (const x of [bx + 24, bx + W - 48]) {
          gfx.setColor(1, 1, 1);
          gfx.draw(G.shieldsImg, gfx.newQuad(city.ownerIndex * 40 + 24, 46, 16, 16), x, by + 10);
        }
      }
      gfx.setColor(1, 1, 1);
      gfx.draw(G.abits, gfx.newQuad(424, 0, 24, 11), bx + 48, by + 36);
      f.draw(fmt("%d", city.income || 0), bx + 80, by + 33);
      gfx.setColor(1, 1, 1);
      gfx.draw(G.abits, gfx.newQuad(424, 11, 24, 11), bx + 144, by + 36);
      f.draw(fmt("%d", city.defence || 0), bx + 176, by + 33);
      const bs = G.screen.art_for(38);
      g.map.sides.forEach((s, i) => {
        if (s.capital === city && bs) {
          for (const x of [bx + 16, bx + W - 56]) {
            gfx.setColor(1, 1, 1);
            gfx.draw(bs.image, gfx.newQuad(i * 32, 36, 32, 23), x, by + 10);
          }
        }
      });
    };
  } else if (site) {
    draw = () => {
      kit.centred(colours(me), site.name || "", bx + W / 2 - 8, by + 11);
      if (site.content === siteMod.TEMPLE) kit.centred(f, "Blessings & Quests!", bx + W / 2 - 8, by + 35);
      else if (site.searched) kit.centred(f, "Explored!", bx + W / 2 - 8, by + 33);
      else kit.centred(f, "Unexplored!", bx + W / 2 - 8, by + 35);
    };
  } else if (terrain === move.TOWER && game.signAt(g, tx, ty)) {
    const sign = game.signAt(g, tx, ty);
    back = 36;
    draw = () => twoLines(sign.lines[0], sign.lines[1], f, f);
  } else {
    const road = (scn.roadAt(g.map, tx, ty) || 0) % 32 !== 0;
    const port = g.map.crossing && g.map.crossing[ty * g.map.width + tx];
    if (port) {
      draw = () => twoLines("Port", "A way for armies to put to sea");
    } else {
      const t = road ? 0 : terrain;
      draw = () => twoLines(kit.text(0x80, t), kit.text(0x81, t));
    }
  }

  d.draw = () => {
    const b = infobox.board(back === 36 ? infobox.POPUP2 : infobox.LINES);
    gfx.setColor(1, 1, 1);
    if (b) gfx.draw(b.image, gfx.newQuad(0, 0, W, H), bx, by);
    gfx.setColor(1, 1, 1);
    draw();
  };
  d.mousereleased = () => kit.pop(d);
  d.mousepressed = () => kit.pop(d);
  d.keypressed = () => kit.pop(d);
  return kit.push(d);
}
