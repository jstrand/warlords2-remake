// The advisor: a horned helmet that speaks (6dda:026f). docs/re/sound.md.
//
// He waits for any sample still sounding (255e:0736), then draws VOICE.PCK
// at (152, 15) through its mask, colour 10. While his clip plays he blinks:
// a count starts at dice(1, 30, 10), goes up a BIOS tick at a time, and past
// 40 it restarts at 0 and the eyes -- VOICEBIT.PCK, three 160x47 frames, open,
// half shut and shut -- go over his at (232, 261): half, shut, half, open, two
// ticks each. When the clip ends the game goes on. With Speech off he never
// appears.

import * as gfx from "../gfx.js";
import * as kit from "./kit.js";
import * as pck from "../warlords/pck.js";
import * as sound from "../sound.js";
import { now } from "../util.js";

const AT = { x: 152, y: 15 };
const EYES = { x: 232, y: 261, w: 160, h: 47 };
const BLINK = [1, 2, 1, 0];            // VOICEBIT rows, 47 apart
const TICK = 1 / 18.2;
const VOICE_KEY = 10;

function images(G) {
  if (!G.voicePic) {
    G.voicePic = pck.toImage(G.dataDir + "/PICS/VOICE.PCK", G.palette, VOICE_KEY)[0];
    G.voiceBits = pck.toImage(G.dataDir + "/PICS/VOICEBIT.PCK", G.palette)[0];
  }
  return [G.voicePic, G.voiceBits];
}

/** Have him say a clip -- a FILE.DAT group, as warlords/cues.js names them --
 *  and run `after` once he has finished. */
export function say(group, after) {
  after = after || (() => {});
  if (group == null || !sound.speechOn()) return after();
  const G = kit.G;
  const d = { waiting: true };

  const finish = () => {
    kit.pop(d);
    after();
  };

  const start = () => {
    d.waiting = false;
    const len = sound.speak(group, finish);
    if (len == null) {
      kit.pop(d);
      return after();
    }
    d.started = now();
    d.count = 11 + Math.floor(Math.random() * 30);
    d.nextTick = d.started + TICK;
    d.blink = null;
  };

  d.update = () => {
    if (d.waiting) {
      if (!sound.busy()) start();
      return;
    }
    const t = now();
    while (d.nextTick && t >= d.nextTick) {
      d.nextTick += TICK;
      if (d.blink) {
        d.blink.ticks++;
        if (d.blink.ticks >= 2) {
          d.blink.ticks = 0;
          d.blink.frame++;
          if (d.blink.frame > BLINK.length) d.blink = null;
        }
      } else {
        d.count++;
        if (d.count > 40) { d.count = 0; d.blink = { frame: 1, ticks: 0 }; }
      }
    }
  };

  d.draw = () => {
    if (d.waiting) return;
    const [pic, bits] = images(G);
    gfx.setColor(1, 1, 1);
    gfx.draw(pic, AT.x, AT.y);
    if (d.blink) {
      const row = BLINK[d.blink.frame - 1] || 0;
      gfx.draw(bits, gfx.newQuad(0, row * EYES.h, EYES.w, EYES.h), EYES.x, EYES.y);
    }
  };

  // he holds the game while he speaks: input goes nowhere
  d.mousepressed = () => {};
  d.keypressed = () => {};

  kit.push(d);
  if (!sound.busy()) start();
  return d;
}
