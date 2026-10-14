// Quests: taking one at a temple, checking it off, and the reward.
//
// docs/rules.md > Quests. quest_assign is 4976:0d7a, quest_check 4976:1ded
// and quest_choose_reward 4976:1909. One quest per side at a time.

import * as armytype from "./armytype.js";
import * as site from "./site.js";
import * as game from "./game.js";
import * as heroMod from "./hero.js";
import * as history from "./history.js";
import { fmt } from "../util.js";

export const SLAY_HERO = 0, RETRIEVE_ITEM = 1, SLAY_TYPE = 2;
export const SLAUGHTER = 3, OCCUPY = 4, RAZE = 5, PILLAGE_GOLD = 6;

// DS:00a0, rolled with 1d10: types 4, 5 and 6 come up twice as often.
export const TYPE_TABLE = [0, 1, 2, 3, 4, 5, 6, 4, 5, 6];

export const DESCRIPTIONS = [
  "slay the enemy hero",
  "retrieve a magic item",
  "slay a unit of an enemy army type",
  "slaughter %d armies of one side",
  "force a city into submission and occupy it",
  "conquer a city and raze it",
  "sack and pillage %d gold",
];

// how far a target may be from the hero: a human's, a computer's
export const ITEM_RANGE = 50, CITY_RANGE = 60;
export const AI_ITEM_RANGE = 30, AI_CITY_RANGE = 40, RAZE_RANGE = 60;

// what a quest's target *is*, so it can be saved and restored by reference
export const TARGET_KIND = ["army", "item", "armytype", "side", "city", "city", "none"];
export const EXPERIENCE = 10;

const dist = (x1, y1, x2, y2) => Math.floor(Math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2));

/** Pick a target for a quest of this type (quest_assign, 4976:0d7a), or null
 *  if there is none. `computer` is quest_assign(1): shorter ranges. */
export function pickTarget(g, side, type, h, computer) {
  if (type === SLAY_HERO) {
    // another side's hero, counted from the top of the army table down
    const heroes = [];
    for (let i = g.armies.length - 1; i >= 0; i--) {
      const a = g.armies[i];
      if (a.type === armytype.HERO && a.owner !== side.index) heroes.push(a);
    }
    if (heroes.length === 0) return null;
    return heroes[g.rng.dice(1, heroes.length, -1)];
  } else if (type === RETRIEVE_ITEM) {
    // an ordinary item still hidden in a ruin, less than 50 (30) away each way
    const range = computer ? AI_ITEM_RANGE : ITEM_RANGE;
    const choices = [];
    for (const it of g.map.items) {
      if (it.index <= 21 && it.status === 2 && !site.itemReserved(it)) {
        const s = g.map.sites.find((x) => x.content === site.ITEM && x.item === it.index);
        if (s && Math.abs(h.x - s.x) < range && Math.abs(h.y - s.y) < range) choices.push({ item: it, site: s });
      }
    }
    if (choices.length === 0) return null;
    const pick = choices[g.rng.dice(1, choices.length, -1)];
    // the priests show the side where it lies
    pick.site.revealed = (pick.site.revealed || 0) | (1 << side.index);
    game.reveal(g, side.index, pick.site.x, pick.site.y, false);
    return pick.item;
  } else if (type === SLAY_TYPE) {
    // up to five magical types, one of which some other army on the map is
    for (let t = 0; t < 5; t++) {
      const want = heroMod.magicalType(g);
      for (let i = g.armies.length - 1; i >= 0; i--) {
        const a = g.armies[i];
        if (a.type === want.id && a.owner !== side.index && !a.transit && a.x != null) return want;
      }
    }
    return null;
  } else if (type === SLAUGHTER) {
    if (g.won) return null;
    for (let t = 0; t < 200; t++) {
      const o = g.map.sides[g.rng.dice(1, 8, -1)];
      if (o && o.index !== side.index && o.inUse && o.alive) return o;
    }
    return null;
  } else if (type === OCCUPY || type === RAZE) {
    // any standing city not the side's, near the hero -- after a hundred
    // tries, anywhere
    if (g.won) return null;
    const range = type === RAZE ? RAZE_RANGE : (computer ? AI_CITY_RANGE : CITY_RANGE);
    for (let t = 0; t < 200; t++) {
      const c = g.map.cities[g.rng.dice(1, g.map.cities.length, -1)];
      if (c && c.ownerIndex !== side.index && !c.razed
          && (dist(h.x, h.y, c.x, c.y) <= range || t > 99)) {
        return c;
      }
    }
    return null;
  } else if (type === PILLAGE_GOLD) {
    return true;                                  // no target, just a count
  }
  return null;
}

/** The type of a quest's `n`th try (from 1). A human's is rolled from the
 *  table; a computer's (quest_assign(1)) is raze or occupy first, then
 *  slaughter, then pillage gold. */
function questType(g, n, computer) {
  if (!computer) return TYPE_TABLE[g.rng.dice(1, 10, -1)];
  if (n === 1) return g.rng.dice(1, 5, -1) === 0 ? RAZE : OCCUPY;
  if (n === 2) return SLAUGHTER;
  return PILLAGE_GOLD;
}

/** Take a quest at a temple, or null (quest_assign, 4976:0d7a): tries until
 *  a quest finds a target -- pillage gold always does. `computer` for a
 *  computer player's (5ad0:15c3). */
export function assign(g, side, h, computer) {
  if (side.quest) return null;                    // one at a time
  if (g.map.options.quests === 0) return null;
  history.deed(g, side, history.QUEST_GIVEN, 0, 0, h && h.name);   // 4976:0dae
  for (let n = 1; ; n++) {
    const type = questType(g, n, computer);
    const target = pickTarget(g, side, type, h, computer);
    if (target) {
      const q = { type, hero: h, target, done: 0, targetKind: TARGET_KIND[type] };
      if (type === SLAUGHTER) q.required = g.rng.dice(1, 12, 10);
      else if (type === PILLAGE_GOLD) q.required = g.rng.dice(3, 300, 500);
      side.quest = q;
      return q;
    }
  }
}

/** A line of prose for a quest. */
export function describe(q) {
  const text = DESCRIPTIONS[q.type];
  if (q.required != null) return fmt(text, q.required);
  if (q.type === OCCUPY || q.type === RAZE || q.type === SLAY_TYPE) return `${text}: ${q.target.name}`;
  return text;
}

function heroInStack(q, stack) {
  return stack.includes(q.hero);
}

// The quest is over. A failure carries the STRING.DAT group quest_check tells
// a human it with; a human side keeps the outcome as `questNews`.
function finish(g, side, reason, why) {
  const q = side.quest;
  side.quest = undefined;
  if (!q) return null;
  let out;
  if (reason === "done") {
    history.deed(g, side, history.QUEST_DONE, 0, 0, q.hero && q.hero.name);   // 4976:1da8
    heroMod.addExperience(g, q.hero, EXPERIENCE);
    out = { quest: q, reward: reward(g, side, q) };
  } else {
    out = { quest: q, failed: reason, why };
  }
  if (!side.computer) side.questNews = out;
  return out;
}

/** Tell the quest what just happened: "battle", "item", "pillage", "occupy",
 *  "raze" or "turn" (quest_check, 4976:1ded). */
export function event(g, side, ev, data = {}) {
  const q = side.quest;
  if (!q) return null;

  if (ev === "turn") {
    const alive = g.armies.some((a) => a === q.hero && a.owner === side.index);
    if (!alive) return finish(g, side, "the hero is lost", 0x20);
    if (q.type === OCCUPY || q.type === RAZE) {
      if (q.target.razed) return finish(g, side, "the city is ruins", 0x21);
      if (q.target.ownerIndex === side.index) return finish(g, side, "another took the city", 0x2a);
    } else if (q.type === SLAUGHTER) {
      if (!q.target.alive) return finish(g, side, "that side is gone", 0x27);
    } else if (q.type === SLAY_HERO) {
      if (!g.armies.includes(q.target)) return finish(g, side, "the quarry is gone", 0x29);
    } else if (q.type === RETRIEVE_ITEM) {
      if (q.target.status === 0) return finish(g, side, "the item is lost", 0x28);
    }
    return null;
  }

  if (ev === "battle") {
    if (!heroInStack(q, data.stack || [])) return null;
    const killed = data.killed || [];
    if (q.type === SLAY_HERO) {
      for (const d of killed) if (d === q.target) return finish(g, side, "done");
    } else if (q.type === SLAY_TYPE) {
      for (const d of killed) if (d.type === q.target.id) return finish(g, side, "done");
    } else if (q.type === SLAUGHTER) {
      for (const d of killed) if (d.owner === q.target.index) q.done++;
      if (q.done >= q.required) return finish(g, side, "done");
    }
  } else if (ev === "item") {
    if (q.type === RETRIEVE_ITEM && q.hero.items) {
      const i = q.hero.items.indexOf(q.target);
      if (i >= 0) {
        const it = q.hero.items[i];
        q.hero.items.splice(i, 1);                // the priests take it away
        it.status = 0;
        return finish(g, side, "done");
      }
    }
  } else if (ev === "pillage") {
    if (q.type === PILLAGE_GOLD && heroInStack(q, data.stack || [])) {
      q.done += data.gold || 0;
      if (q.done >= q.required) return finish(g, side, "done");
    } else if ((q.type === OCCUPY || q.type === RAZE) && data.city === q.target) {
      return finish(g, side, "that was not to pillage", 0x24);
    }
  } else if (ev === "occupy") {
    if (q.type === OCCUPY && data.city === q.target) {
      if (heroInStack(q, data.stack || [])) return finish(g, side, "done");
      return finish(g, side, "the hero was not there", 0x22);
    } else if (q.type === RAZE && data.city === q.target) {
      return finish(g, side, "the quest was to raze it", 0x23);
    }
  } else if (ev === "raze") {
    if (q.type === RAZE && data.city === q.target) {
      if (heroInStack(q, data.stack || [])) return finish(g, side, "done");
      return finish(g, side, "the hero was not there", 0x25);
    } else if (q.type === OCCUPY && data.city === q.target) {
      return finish(g, side, "the quest was to keep it", 0x26);
    }
  }
  return null;
}

// The allies a quest gives are always Archons (4976:1d5b pushes 19).
export const REWARD_ALLY = 19;

/** Choose and give the reward (quest_choose_reward, 4976:1909, and
 *  quest_give_reward, 4976:1c02). */
export function reward(g, side, q) {
  const cities = game.sideCities(g, side).length;
  const h = q && q.hero;

  // Archons, on the quest hero's tile (create_ally_army)
  const allies = (n) => {
    const type = g.types.byId[REWARD_ALLY];
    const joined = [];
    for (let i = 0; i < n; i++) {
      const a = h ? heroMod.placeAlly(g, type, h.x, h.y, side) : null;
      if (a) joined.push(a);
    }
    return { kind: "allies", armies: joined, type, count: n };
  };

  const gold = () => {
    const n = g.rng.dice(2, 1000, 1000);
    side.gold += n;
    return { kind: "gold", gold: n };
  };

  if (cities < 10 && g.turn > 15) return allies(g.rng.dice(1, 3, 5));
  if (side.gold < 100) return gold();

  // an unused reserved item -- of the standards only the side's own
  const unused = g.map.items.filter((it) => it.index <= 21 && (it.status || 0) === 0
    && site.itemReserved(it) && (it.index > 7 || it.index === side.index));
  // a rich site the side does not know of yet, not emptied
  const unknown = g.map.sites.filter((s) => s.rich && !site.shownTo(s, side) && s.content !== site.EMPTY);
  if (unused.length > 0 && g.rng.dice(1, 3, 0) === 1) {
    const it = unused[g.rng.dice(1, unused.length, -1)];
    if (h) {
      h.items = h.items || [];
      h.items.push(it);
      it.status = 3;
    }
    return { kind: "item", item: it };
  }
  if (unused.length === 0 && unknown.length > 0 && g.rng.dice(1, 3, 0) < 3) {
    // the nearest to the hero, the first of equals
    let best = null, bestD = 10000;
    for (const s of unknown) {
      const d = h ? Math.max(Math.abs(s.x - h.x), Math.abs(s.y - h.y)) : 0;
      if (d < bestD) { best = s; bestD = d; }
    }
    best.revealed = (best.revealed || 0) | (1 << side.index);
    if (!side.computer || side.observe) game.reveal(g, side.index, best.x, best.y, false);
    return { kind: "revealed", site: best };
  }
  if (g.rng.dice(1, 2, 0) === 1) return allies(g.rng.dice(1, 3, 2));
  return gold();
}
