// Hero > Inspect: the hero info dialog.
//
// 6c1b:0000 lists the side's heroes -- by army, last first -- and opens
// dialog 8 over popup 2 on the one nearest the cursor. 6c1b:00f9 draws the
// strategic map with every hero's figure, the hero's name, its stack along
// y = 110, where it is, its battle and command bonuses, level and
// experience; 6c1b:0724 the items it carries and those on the ground.
// Controls: 242 Done, 243/244 next and previous hero, 245/246 the item
// above and below, 247 Drop It, 248 Take It, 250/249 Carried and Ground.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as armytype from "../warlords/armytype.js";
import * as combat from "../warlords/combat.js";
import * as game from "../warlords/game.js";
import * as hero from "../warlords/hero.js";
import * as rules from "../warlords/rules.js";
import * as quest from "../warlords/quest.js";
import * as uidata from "../warlords/uidata.js";
import * as sound from "../sound.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 312 };      // popup 2
const MAP = { x: 80, y: 60 };
const DIALOG = 8;
const DONE = 242, NEXT = 243, PREV = 244, UP = 245, DOWN = 246, DROP = 247, TAKE = 248, GROUND = 249, CARRIED = 250;
const S = 0x83;                                  // STRING.DAT group 131
const CHECKED = [320, 0], CLEAR = [320, 20];     // 4125:0ad4

function itemColour(it) {
  if (it.status === 1) return 10;
  if (it.status === 0) return 5;
  return 7;
}

/** What an item does, as the dialog abbreviates it (4125:0d7c-0d9c). */
function effect(it) {
  const t = it.type, v = it.value || 0;
  if (t === rules.ITEM_STANDARD) return "com +1";
  if (t === rules.ITEM_COMMAND) return fmt("com +%d", v);
  if (t === rules.ITEM_BATTLE) return fmt("bat +%d", v);
  if (t === rules.ITEM_FLIGHT) return "fly";
  if (t === rules.ITEM_DOUBLE_MOVE) return "move";
  if (t === rules.ITEM_GOLD_PER_CITY) return fmt("gld +%d", v);
  return "";
}

function itemSum(h, t) {
  let n = 0;
  for (const it of h.items || []) {
    if (it.type === t) n += it.value || 0;
    if (t === rules.ITEM_COMMAND && it.type === rules.ITEM_STANDARD) n++;
  }
  return n;
}

/** Open the dialog, or do nothing if the side has no hero. */
export function open() {
  const G = kit.G;
  const heroes = [];
  for (let i = G.g.armies.length - 1; i >= 0; i--) {
    const a = G.g.armies[i];
    if (a.type === armytype.HERO && a.owner === G.player.index && !a.transit && a.x != null) heroes.push(a);
  }
  if (heroes.length === 0) return null;
  const [cx, cy] = G.viewCentre();
  let cur = 0, best = null;
  heroes.forEach((h, i) => {
    const dd = Math.max(Math.abs(h.x - cx), Math.abs(h.y - cy));
    if (best === null || dd < best) { cur = i; best = dd; }
  });

  const d = { view: kit.view(DIALOG), cur, item: null };
  const list = () => hero.itemsHere(G.g, heroes[d.cur]);

  /** choose item i of the list, from 0 (or none) */
  const choose = (i) => {
    const items = list();
    d.item = (i != null && items[i]) ? i : null;
    d.ground = d.item != null && items[d.item].status === 1;
    const st = d.view.state;
    st[DONE] = uidata.NORMAL;
    st[NEXT] = heroes.length > 1 ? uidata.NORMAL : uidata.DISABLED;
    st[PREV] = st[NEXT];
    st[UP] = (d.item != null && d.item > 0) ? uidata.NORMAL : uidata.DISABLED;
    st[DOWN] = (d.item != null && d.item < items.length - 1) ? uidata.NORMAL : uidata.DISABLED;
    st[DROP] = d.item != null ? uidata.NORMAL : uidata.DISABLED;
    st[TAKE] = st[DROP];
    d.hidden = { [d.ground ? DROP : TAKE]: true };
  };

  const showHero = (i) => {
    const n = heroes.length;
    d.cur = ((i % n) + n) % n;
    choose(list().length > 0 ? 0 : null);
  };

  /** the first item of a kind: 3 carried, 1 on the ground (6c1b:0fc2, 1037) */
  const firstOf = (status) => {
    const items = list();
    for (let i = 0; i < items.length; i++) if (items[i].status === status) { choose(i); return; }
  };

  d.close = () => {
    kit.pop(d);
    quest.event(G.g, G.player, "item", {});
  };

  d.draw = () => {
    const h = heroes[d.cur];
    const f = kit.font(2);
    kit.popup(R);
    G.drawStrategicMap(MAP.x, MAP.y, null, true);
    heroes.forEach((o, i) => { if (i !== d.cur) G.drawHeroFigure(MAP.x, MAP.y, o.x, o.y); });
    G.drawHeroFigure(MAP.x, MAP.y, h.x, h.y);

    kit.setPal(0);
    kit.outline(308, 206, 248, 129);
    kit.bevel(307, 205, 250, 131, 4, 2);

    gfx.setColor(1, 1, 1);
    kit.centred(kit.font(1), h.name || "", 432, 62);

    kit.army(armytype.HERO, G.player.index, 304, 110, 1);
    let k = 1;
    for (const a of game.armiesAt(G.g, h.x, h.y)) {
      if (a !== h && k < 8) {
        kit.army(a.type, a.owner, 304 + k * 32, 110, 1);
        k++;
      }
    }
    for (let j = k; j <= 7; j++) kit.army(null, G.player.index, 304 + j * 32, 110, 1);

    const inCity = game.cityAt(G.g, h.x, h.y);
    let near = null, nearD = null;
    for (const c of G.g.map.cities) {
      if (game.seen(G.g, G.player, c.x, c.y)) {
        const dd = Math.max(Math.abs(c.x - h.x), Math.abs(c.y - h.y));
        if (nearD === null || dd < nearD) { near = c; nearD = dd; }
      }
    }
    const battle = itemSum(h, rules.ITEM_BATTLE);
    const command = (combat.HERO_TABLE[Math.min(9, (h.strength || 0) + battle)] || 0)
                  + itemSum(h, rules.ITEM_COMMAND);
    gfx.setColor(1, 1, 1);
    kit.right(f, kit.text(S, inCity ? 1 : 0), 384, 145);
    kit.right(f, kit.text(S, 2), 384, 165);
    kit.right(f, kit.text(S, 3), 384, 185);
    kit.right(f, kit.text(S, 4), 528, 165);
    kit.right(f, kit.text(S, 5), 528, 185);
    f.draw(near ? near.name : "", 392, 145);
    f.draw(fmt("+%d", battle), 392, 165);
    f.draw(fmt("+%d", command), 392, 185);
    f.draw(fmt("%d", h.level || 1), 536, 165);
    f.draw(fmt("%d", h.experience || 0), 536, 185);
    f.draw(fmt(kit.text(S, 6), d.cur + 1, heroes.length), 312, 347);

    const items = list();
    kit.setPal(3);
    gfx.rectangle("fill", 367, 236, 184, 66);
    kit.bevel(367, 236, 184, 66, 4, 2);
    kit.bevel(369, 258, 180, 21, 2, 4);
    gfx.setColor(1, 1, 1);
    kit.centred(f, kit.text(S, d.ground ? 7 : 8), 432, 211);
    if (d.item != null) {
      for (let row = 0; row <= 2; row++) {
        const it = items[d.item - 1 + row];
        if (it) f.colours(itemColour(it), 0).draw(it.name || "", 376, 238 + 22 * row);
      }
      const it = items[d.item];
      kit.centred(f.colours(itemColour(it), 0), effect(it), 340, 260);
    }
    const box = (src, x) => {
      gfx.setColor(1, 1, 1);
      gfx.draw(G.abits, gfx.newQuad(src[0], src[1], 24, 20), x, 311);
    };
    box(d.ground ? CLEAR : CHECKED, 312);
    box(d.ground ? CHECKED : CLEAR, 400);
    f.colours(7, 0).draw("Carried", 336, 313);
    f.colours(10, 0).draw("Ground", 424, 313);
    kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c) return;
    const items = list();
    if (c.id === DONE) d.close();
    else if (c.id === NEXT) showHero(d.cur + 1);
    else if (c.id === PREV) showHero(d.cur - 1);
    else if (c.id === UP) choose(d.item - 1);
    else if (c.id === DOWN) choose(d.item + 1);
    else if (c.id === DROP && d.item != null) {
      const it = items[d.item];
      hero.dropItem(G.g, heroes[d.cur], it);
      // 7563:0943: dropped at sea it is gone, with a splash
      if (it.status === 0) sound.effect("splash");
      choose(Math.min(d.item, list().length - 1));
    } else if (c.id === TAKE && d.item != null) {
      hero.takeItem(G.g, heroes[d.cur], items[d.item]);
      choose(Math.min(d.item, list().length - 1));
    } else if (c.id === GROUND) firstOf(1);
    else if (c.id === CARRIED) firstOf(3);
  };

  d.keypressed = (key) => {
    if (key === "escape" || key === "return" || key === "kpenter") d.close();
  };

  showHero(cur);
  return kit.push(d);
}
