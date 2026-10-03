// Game > Settings (64d2:0000(1)) -- the same screen that sets the sides up
// for a new game.
//
// Popup 4, (120, 50) 400x360, dialog 9 (64d2:0137): a row a side -- its name
// in its colours, "Deceased!" for a side out of play, else the Human box and
// "Human" or its level, Enhanced, and for a computer Observe -- and the boxes
// for Music, Effects and Speech. 252-259 turn a side human or computer,
// 260-267 Enhanced, 268-275 Observe, 276-278 the sounds (64d2:0576 writes
// OPTIONS.SND straight back); OK (251).

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as uidata from "../warlords/uidata.js";
import * as sound from "../sound.js";

const R = { x: 120, y: 50, w: 400, h: 360 };     // popup 4
const DIALOG = 9, OK = 251;
const HUMAN = 252, ENHANCED = 260, OBSERVE = 268, SOUND = 276;
const LEVELS = ["Knight", "Lord", "Warlord"];    // 4125:0bf2
const SOUNDS = [[152, 340, "Music", "music"], [152, 370, "Effects", "effects"], [256, 340, "Speech", "speech"]];

/** `after` runs when OK is pressed. */
export function open(after) {
  const G = kit.G;
  const g = G.g;
  const d = { view: kit.view(DIALOG) };

  const refresh = () => {
    const st = d.view.state;
    st[OK] = uidata.NORMAL;
    st[SOUND] = uidata.NORMAL; st[SOUND + 1] = uidata.NORMAL; st[SOUND + 2] = uidata.NORMAL;
    d.hidden = {};
    for (let i = 0; i < 8; i++) {
      const s = g.map.sides[i];
      if (s && s.inUse) {
        st[HUMAN + i] = uidata.NORMAL; st[ENHANCED + i] = uidata.NORMAL; st[OBSERVE + i] = uidata.NORMAL;
      } else {
        d.hidden[HUMAN + i] = true; d.hidden[ENHANCED + i] = true; d.hidden[OBSERVE + i] = true;
      }
    }
  };

  const box = (on, x, y) => {
    gfx.setColor(1, 1, 1);
    gfx.draw(G.abits, gfx.newQuad(320, on ? 0 : 20, 24, 20), x, y);
  };

  d.draw = () => {
    kit.popup(R);
    const f = kit.font(2);
    const me = G.player;
    const head = f.colours(me.colour ?? 15, me.edge ?? 0);
    gfx.setColor(1, 1, 1);
    kit.right(head, "Name", 248, 60);
    kit.centred(head, "Human", 280, 60);
    kit.centred(head, "Enhanced", 396, 60);
    kit.centred(head, "Observe", 468, 60);
    for (let i = 0; i < 8; i++) {
      const s = g.map.sides[i];
      const y = 90 + 30 * i;
      if (s) {
        gfx.setColor(1, 1, 1);
        kit.right(f.colours(s.colour ?? 15, s.edge ?? 0), s.name || "", 248, y);
        const playing = s.inUse && s.alive !== false;
        if (!playing) {
          if (s.name !== "Not used") f.draw("Deceased!", 280, y);
        } else {
          box(!s.computer, 256, y);
          gfx.setColor(1, 1, 1);
          f.draw(s.computer ? (LEVELS[s.level || 0] || "") : "Human", 280, y);
          box(s.enhanced, 400, y);
          if (s.computer) box(s.observe, 448, y);
        }
      }
    }
    const on = sound.options();
    SOUNDS.forEach((sd, k) => {
      box(on[k], sd[0], sd[1]);
      gfx.setColor(1, 1, 1);
      f.draw(sd[2], sd[0] + 24, sd[1]);
    });
    kit.drawControls(d.view, d.hidden);
  };

  d.mousepressed = (x, y) => {
    const c = kit.controlAt(d.view, x, y, d.hidden);
    if (!c || d.view.state[c.id] === uidata.DISABLED) return;
    const id = c.id;
    if (id === OK) {
      kit.pop(d);
      if (after) after();
      return;
    }
    if (id >= HUMAN && id < HUMAN + 8) {
      const s = g.map.sides[id - HUMAN];
      s.computer = !s.computer;
    } else if (id >= ENHANCED && id < ENHANCED + 8) {
      const s = g.map.sides[id - ENHANCED];
      s.enhanced = !s.enhanced;
    } else if (id >= OBSERVE && id < OBSERVE + 8) {
      const s = g.map.sides[id - OBSERVE];
      if (s.computer) s.observe = !s.observe;
    } else if (id >= SOUND && id < SOUND + 3) {
      sound.toggle(SOUNDS[id - SOUND][3]);
    }
    refresh();
  };

  d.keypressed = (key) => {
    if (key === "return" || key === "kpenter" || key === "escape") {
      kit.pop(d);
      if (after) after();
    }
  };

  refresh();
  return kit.push(d);
}
