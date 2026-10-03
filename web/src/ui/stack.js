// View > Stack (89e0:0c9c): the selected stack laid out at length, to be
// grouped the way the bar under the map groups it.
//
// It works on a copy of the bar's slots and its clicks are the bar's: an army
// (336-343) joins the group that moves or leaves it, a mark (344-351) makes
// its group the one, Group (334) puts the lot together and Ungroup (335)
// breaks it up. OK (332) keeps it, Cancel (333) throws it away.
//
// Popup 0, (80, 60) 480x320 (89e0:0e85): STACK.PCK's column heads with the
// combat cap, and eight rows 30 apart from (112, 90): the mark, the army on a
// ring of its group's colour (its shadow when not moving), a non-hero's
// medals, name, strength -- and for the group that moves its strength in a
// fight here (89e0:1b9c) -- moves, how it moves and its bonus.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as armybonus from "./armybonus.js";
import * as infobox from "./infobox.js";
import * as armytype from "../warlords/armytype.js";
import * as combat from "../warlords/combat.js";
import * as rules from "../warlords/rules.js";
import * as slotsMod from "../warlords/slots.js";
import * as uidata from "../warlords/uidata.js";
import { fmt } from "../util.js";

const R = { x: 80, y: 60, w: 480, h: 320 };      // popup 0
const DIALOG = 18, OK = 332, CANCEL = 333, GROUP = 334, UNGROUP = 335, ARMY = 336, MARK = 344;
const ROWS = 8;
const MEDALS = [[424, 22], [432, 22], [440, 22], [456, 32]];   // 4125:2fa6

function copy(s) {
  return { n: s.n, side: s.side, army: s.army.slice(), group: s.group.slice(),
           inGroup: s.inGroup.slice(), mark: s.mark.slice(), active: s.active };
}

function flies(a, t) {
  if (t.flies) return true;
  if (a.type !== armytype.HERO) return false;
  return (a.items || []).some((it) => it.type === rules.ITEM_FLIGHT);
}

export function open() {
  const G = kit.G;
  if (!G.selection) return null;
  const side = G.player;
  const d = { view: kit.view(DIALOG), s: copy(G.selection.slots) };

  const refresh = () => {
    const st = d.view.state, n = d.s.n;
    st[OK] = uidata.NORMAL; st[CANCEL] = uidata.NORMAL;
    st[GROUP] = n > 1 ? uidata.NORMAL : uidata.DISABLED;
    st[UNGROUP] = st[GROUP];
    for (let i = 0; i < ROWS; i++) {
      st[ARMY + i] = i < n ? uidata.NORMAL : uidata.DISABLED;
      st[MARK + i] = st[ARMY + i];
    }
  };

  d.draw = () => {
    const s = d.s;
    kit.popup(R);
    const head = G.screen.art_for(46);
    gfx.setColor(1, 1, 1);
    if (head) gfx.draw(head.image, gfx.newQuad(0, 0, 400, 23), 80, 62);
    const f = kit.font(2);
    f.draw(fmt("(max +%d)", G.g.map.combatCap ?? 5), 480, 66);      // 4125:320d
    const abits = (sx, sy, w, h, x, y) => {
      gfx.setColor(1, 1, 1);
      gfx.draw(G.abits, gfx.newQuad(sx, sy, w, h), x, y);
    };
    const moving = [];
    for (let i = 0; i < s.n; i++) if (s.inGroup[i]) moving.push(s.army[i]);
    const a1 = s.army[0];
    const fight = a1 ? combat.stackStrengths(G.g, moving, a1.x, a1.y) : new Map();

    for (let i = 0; i < ROWS; i++) {
      const x = 112, y = 90 + 30 * i;
      const a = s.army[i];
      if (i < s.n && a) {
        const t = G.g.types.byId[a.type];
        if (s.mark[i] != null) abits(448, s.mark[i] === slotsMod.TICK ? 16 : 0, 32, 16, x - 32, y + 5);
        kit.army(a.type, side.index, x, y, (side.index + s.group[i]) % 8 + 2, !s.inGroup[i]);
        if (a.type !== armytype.HERO && (t.bonus[48] || 0) === 0) {
          for (let m = 0; m < Math.min(MEDALS.length, a.medals || 0); m++) {
            abits(MEDALS[m][0], MEDALS[m][1], 8, 8, x + 32 + Math.floor(m / 2) * 8, y + 8 + (m % 2) * 9);
          }
        }
        gfx.setColor(1, 1, 1);
        f.draw(a.type === armytype.HERO ? (a.name || "") : (t.name || ""), x + 48, y + 5);
        let str = a.strength || 0;
        if (a.type === armytype.HERO) str += combat.battleItems(a);
        f.draw(fmt("%d", Math.min(9, str)), x + 168, y + 5);
        if (s.inGroup[i] && fight.has(a)) f.draw(fmt("(%d)", fight.get(a)), x + 184, y + 5);
        f.draw(fmt("%d", a.moves || 0), x + 232, y + 5);
        let src = null;
        if (flies(a, t)) src = [184, 30];
        else if (a.atSea) src = [424, 30];
        else if (t.woodsMove && t.hillsMove) src = [216, 30];
        else if (t.woodsMove) src = [248, 30];
        else if (t.hillsMove) src = [152, 30];
        if (src) abits(src[0], src[1], 32, 10, x + 256, y + 8);
        gfx.setColor(1, 1, 1);
        f.draw(armybonus.bonusText(t, a), x + 304, y + 5);
      } else {
        kit.army(null, side.index, x, y, 1);
      }
    }
    const hidden = {};
    for (let i = 0; i < ROWS; i++) { hidden[ARMY + i] = true; hidden[MARK + i] = true; }
    kit.drawControls(d.view, hidden);
  };

  const keep = () => {
    kit.pop(d);
    G.selection.slots = d.s;
    if (G.afterSlotChange) G.afterSlotChange();
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y);
    if (!c || d.view.state[c.id] === uidata.DISABLED) return;
    const id = c.id;
    if (id === OK) return keep();
    else if (id === CANCEL) return kit.pop(d);
    else if (id === GROUP) slotsMod.all(d.s, G.g);
    else if (id === UNGROUP) slotsMod.single(d.s, G.g);
    else if (id >= ARMY && id < ARMY + ROWS) slotsMod.toggle(d.s, G.g, id - ARMY);
    else if (id >= MARK && id < MARK + ROWS) slotsMod.pickGroup(d.s, G.g, id - MARK);
    refresh();
  };

  // the right button on a row (sub-ids 37-44, 89e0:1747)
  d.info = (sub, sx, sy) => {
    const n = sub - 37;
    if (n < d.s.n) infobox.army(sx, sy, d.s.army[n]);
    else infobox.lines(sx, sy, "Select Army", "Select armies when present");
    return true;
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter") keep();
    else if (key === "escape") kit.pop(d);
  };

  refresh();
  return kit.push(d);
}
