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

export const ITEM_RANGE = 50, CITY_RANGE = 60;

// what a quest's target *is*, so it can be saved and restored by reference
export const TARGET_KIND = ["army", "item", "armytype", "side", "city", "city", "none"];
export const EXPERIENCE = 10;

function otherSides(g, side) {
  return g.sides.filter((s) => s.alive && s.index !== side.index);
}

/** Pick a target for a quest of this type, or null if there is none. */
export function pickTarget(g, side, type, h) {
  if (type === SLAY_HERO) {
    const heroes = g.armies.filter((a) => a.type === armytype.HERO && a.owner != null
                                         && a.owner !== side.index);
    return g.rng.pick(heroes) || null;
  } else if (type === RETRIEVE_ITEM) {
    const choices = [];
    for (const s of g.map.sites) {
      if (s.content === site.ITEM && !s.searched) {
        let item;
        for (const it of g.map.items) if (it.index === s.item) item = it;
        if (item && !site.itemReserved(item) && Math.abs(s.x - h.x) <= ITEM_RANGE
            && Math.abs(s.y - h.y) <= ITEM_RANGE) {
          choices.push({ item, site: s });
        }
      }
    }
    const pick = g.rng.pick(choices);
    if (!pick) return null;
    pick.site.revealed = 0;                       // the priests show where it lies
    return pick.item;
  } else if (type === SLAY_TYPE) {
    for (let t = 0; t < 5; t++) {                 // up to five tries
      const magical = g.types.list.filter((a) => (a.bonus[48] || 0) !== 0);
      const want = g.rng.pick(magical);
      if (want) {
        for (const a of g.armies) {
          if (a.type === want.id && a.owner != null && a.owner !== side.index) return want;
        }
      }
    }
    return null;
  } else if (type === SLAUGHTER) {
    return g.rng.pick(otherSides(g, side)) || null;
  } else if (type === OCCUPY || type === RAZE) {
    const choices = [], any = [];
    for (const c of g.map.cities) {
      if (c.ownerIndex != side.index && !c.razed) {
        any.push(c);
        const d = Math.max(Math.abs(c.x - h.x), Math.abs(c.y - h.y));
        if (d <= CITY_RANGE) choices.push(c);
      }
    }
    return g.rng.pick(choices) || g.rng.pick(any) || null;
  } else if (type === PILLAGE_GOLD) {
    return true;                                  // no target, just a count
  }
  return null;
}

/** Take a quest at a temple, or null (quest_assign, 4976:0d7a). */
export function assign(g, side, h) {
  if (side.quest) return null;                    // one at a time
  if (g.map.options.quests === 0) return null;
  for (let t = 0; t < 20; t++) {
    const type = TYPE_TABLE[g.rng.dice(1, 10, 0) - 1];
    // types 3, 4 and 5 are skipped once the game has been won
    if (!(g.won && (type === SLAUGHTER || type === OCCUPY || type === RAZE))) {
      const target = pickTarget(g, side, type, h);
      if (target) {
        const q = { type, hero: h, target, done: 0, targetKind: TARGET_KIND[type] };
        if (type === SLAUGHTER) q.required = g.rng.dice(1, 12, 10);
        else if (type === PILLAGE_GOLD) q.required = g.rng.dice(3, 300, 500);
        side.quest = q;
        history.deed(g, side, history.QUEST_GIVEN, 0, 0, h && h.name);   // 4976:0dae
        return q;
      }
    }
  }
  return null;
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

/** Choose and give the reward (quest_choose_reward, 4976:1909). */
export function reward(g, side, q) {
  const cities = game.sideCities(g, side).length;

  const allies = (n) => {
    const [type] = heroMod.allies(g);             // one random magical type
    const joined = [];
    const h = q && q.hero;
    const home = g.map.cities[h ? (h.homeCity || 0) : 0] || side.capital;
    for (let i = 0; i < n; i++) {
      const [x, y] = game.freeTileIn(g, home, true);
      if (x != null) {
        const a = heroMod.newAlly(g, type, x, y, side.index, home.index);
        g.armies.push(a);
        joined.push(a);
      }
    }
    return { kind: "allies", armies: joined, type };
  };

  const gold = () => {
    const n = g.rng.dice(2, 1000, 1000);
    side.gold += n;
    return { kind: "gold", gold: n };
  };

  if (cities < 10 && g.turn > 15) return allies(g.rng.dice(1, 3, 5));
  if (side.gold < 100) return gold();

  // an unclaimed magic item, one time in three
  const unclaimed = g.map.items.filter((it) => (it.status || 0) === 0);
  if (unclaimed.length > 0) {
    if (g.rng.dice(1, 3, 0) === 1) {
      const it = g.rng.pick(unclaimed);
      const h = q && q.hero;
      if (h) {
        h.items = h.items || [];
        h.items.push(it);
        it.status = 3;
      }
      return { kind: "item", item: it };
    }
  } else {
    // otherwise the priests may point at a rich site, two times in three
    const hidden = g.map.sites.filter((s) => s.rich && !s.searched);
    if (hidden.length > 0 && g.rng.dice(1, 3, 0) <= 2) {
      const s = g.rng.pick(hidden);
      s.revealed = 0;
      return { kind: "revealed", site: s };
    }
  }
  if (g.rng.dice(1, 2, 0) === 1) return allies(g.rng.dice(1, 3, 2));
  return gold();
}
