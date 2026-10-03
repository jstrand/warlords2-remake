// What the game plays, and when: the choices behind the music and the
// advisor's voice, as WARLORD2.EXE makes them. docs/re/sound.md.
//
// Headless. The file names come from DATA/FILE.DAT (uidata.strings). A game
// should not play out differently because the music is on, so the caller
// passes a roll of its own: roll(n) -> 1..n.

import * as armytype from "./armytype.js";

// The argument of 6dda:0000, the music selector.
export const TITLE = 0, PLAY = 1, COMPUTER = 2, TRIUMPH = 3, HERO = 4, TEMPLE = 5;
export const SAGE = 6, PROMOTION = 7, MEDAL = 8, SURRENDER = 9, DEFIANCE = 10, BEGIN = 11;

// FILE.DAT group and index of each cue's song; index -1 is a random entry.
const SONG = {
  0: [8, 0], 1: [10, -1], 3: [9, -1], 4: [12, 0], 5: [13, -1], 6: [14, -1],
  7: [15, 0], 8: [16, 0], 9: [17, 0], 10: [18, 0], 11: [19, 0],
};

function entry(files, group, index, roll) {
  const g = files[group] || [];
  if (index < 0) index = roll(g.length) - 1;
  if (index > g.length - 1) index = g.length - 1;
  return g[index];
}

/** The song for a cue: [FILE.DAT name, loops]. */
export function song(files, cue, sides, roll) {
  if (cue === COMPUTER) {
    // with a human still in play, group 11; in a game of computers alone, 5%
    // group 12, 47% group 11 and 48% group 10
    let human = false;
    for (const s of sides || []) {
      if (s.alive !== false && !s.computer) { human = true; break; }
    }
    let group = 11;
    if (!human) {
      const r = roll(100);
      if (r < 6) group = 12; else if (r >= 53) group = 10;
    }
    return [entry(files, group, -1, roll), false];
  }
  const s = SONG[cue];
  if (!s) return [null, false];
  return [entry(files, s[0], s[1], roll), true];
}

// The advisor's clips are FILE.DAT groups 40-62, each with its subtitle 30
// groups on -- or, for the four set phrases, at index 1 of its own.
export const GREET = 59, MOMENT = 60, BEGIN_WAR = 61, QUIT = 62;

const LOSING = { 5: 40, 10: 41, 15: 42, 20: 43, 25: 44, 30: 45 };
const WINNING = { 10: 48, 15: 49, 20: 50, 25: 51, 30: 52, 35: 53 };

/** What the advisor says as `side`'s turn opens (6dda:026f(5)): a FILE.DAT
 *  group, or null. Updates side.advisor as the original does. */
export function advisor(g, side, roll) {
  let cities = 0, heroes = 0;
  for (const c of g.map.cities) if (c.ownerIndex === side.index && !c.razed) cities++;
  if (cities > 39) return null;
  for (const a of g.armies) if (a.owner === side.index && a.type === armytype.HERO) heroes++;
  side.advisor = side.advisor || { dir: 0, mark: 0 };
  const adv = side.advisor;
  let group;
  if (cities < adv.mark) {
    const m = Math.floor(cities / 5) * 5;
    adv.dir = 2; adv.mark = m;
    group = LOSING[m + 5] || 46;
  } else if (adv.mark + 5 <= cities) {
    const m = Math.floor(cities / 5) * 5;
    adv.dir = 1; adv.mark = m;
    group = WINNING[m] || 47;
  } else {
    if (g.turn % 7 !== 0) return null;
    if (side.gold < 100) group = 54;
    else if (side.gold > 2800) group = 55;
    else if (heroes < 1) group = 56;
    else if (heroes > 4) group = 57;
    else if (roll(5) === 1) group = 58;
    else return null;
  }
  if (g.turn === 1) return null;
  return group;
}

/** The clip's sample and subtitle names for an advisor group: [sample, text]. */
export function clip(files, group, roll) {
  if (group >= GREET) return [entry(files, group, 0, roll), entry(files, group, 1, roll)];
  const g = files[group] || [];
  const i = roll(g.length);
  const t = files[group + 30] || [];
  return [g[i - 1], t[Math.min(i, t.length) - 1]];
}
