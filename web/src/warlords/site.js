// Ruins, temples and sages: what they hold, and what searching them does.
//
// docs/rules.md > Ruins, temples and sages. The contents are rolled once at
// game start (setup_random_sites, 66d4:0000); searching is site_search,
// 6536:0000.

import * as armytype from "./armytype.js";
import * as rules from "./rules.js";
import * as scn from "./scn.js";
import * as move from "./move.js";
import * as game from "./game.js";
import * as heroMod from "./hero.js";
import * as combat from "./combat.js";
import * as history from "./history.js";
import * as quest from "./quest.js";

// what a site holds
export const EMPTY = 0, TEMPLE = 1, ITEM = 2, SAGE = 3, GOLD = 4, ALLIES = 5;
export const CONTENT_NAMES = ["empty", "temple", "a magic item", "a sage", "gold", "allies"];

export const CAPITAL_RANGE = 15;       // "a capital within 15 tiles"
export const RICH_SHARE = 3;           // sites * 3 / 10 are rich
export const BLESSING_TEMPLES = 4;     // only the first four temples can bless

// The content tables, by band. 3 = sage, 4 = gold, 5 = allies.
export const CONTENT = {
  rich: [5, 5, 4],
  far: [3, 4, 5, 3, 4],
  near: [3, 4, 5],
};

// Ally army types, by band (army type ids).
export const ALLY_TYPES = {
  rich: [25, 23, 27, 19],   // Dragons, Wizards, Devils, Archons
  far: [24, 20, 26],        // Ghosts, Giant Worms, Demons
  near: [22, 20],           // Elementals, Giant Worms
};

/** Items that may only be hidden in a rich ruin (item_reserved, 66d4:08f4). */
export function itemReserved(item) {
  return item.type === rules.ITEM_FLIGHT || item.type === rules.ITEM_DOUBLE_MOVE
      || item.type === rules.ITEM_STANDARD
      || (item.type === rules.ITEM_COMMAND && item.value >= 2);
}

/** Refill item records 8..21 from the scenario's .ITM pool (66d4:04ef). */
export function fillItemPool(g, reserved) {
  const pool = g.map.itemPool;
  if (!pool) return;
  const byIndex = {}, used = {};
  for (const it of g.map.items) byIndex[it.index] = it;
  for (let idx = 8; idx <= 21; idx++) {
    const item = byIndex[idx];
    if (item) {
      const want = idx < 8 + reserved;
      const choices = [];
      pool.forEach((p, i) => {
        if (!used[i] && itemReserved(p) === want) choices.push(i);
      });
      const pick = g.rng.pick(choices);
      if (pick !== undefined) {
        used[pick] = true;
        item.name = pool[pick].name; item.type = pool[pick].type; item.value = pool[pick].value;
      }
    }
  }
}

/** Mark `sites * 3 / 10` non-temple sites rich (mark_rich_sites, 66d4:091e). */
export function markRich(g) {
  const ruins = [];
  for (const s of g.map.sites) {
    s.rich = false; s.revealed = 0xff;
    if (s.type !== TEMPLE) ruins.push(s);
  }
  const want = Math.floor(g.map.sites.length * RICH_SHARE / 10);
  g.rng.shuffle(ruins);
  for (let i = 0; i < Math.min(want, ruins.length); i++) {
    ruins[i].rich = true;
    if (g.map.options.quests !== 0) ruins[i].revealed = 0;
  }
}

function band(g, s, capitals) {
  if (s.rich) return "rich";
  for (const c of capitals) {
    if (Math.max(Math.abs(s.x - c.x), Math.abs(s.y - c.y)) < CAPITAL_RANGE) return "near";
  }
  return "far";
}

/** Roll what every site holds (setup_random_sites, 66d4:0000). */
export function setup(g) {
  const capitals = [];
  for (const s of g.sides) if (s.capital) capitals.push(s.capital);

  markRich(g);
  let temples = 0;
  for (const s of g.map.sites) {
    s.content = s.type === TEMPLE ? TEMPLE : EMPTY;
    if (s.content === TEMPLE) { s.templeIndex = temples; temples++; }
    s.item = undefined; s.guardian = undefined; s.allyType = undefined; s.searched = false;
    s.band = band(g, s, capitals);
  }

  const reserved = Math.floor(g.map.sites.length * 2 / 10);
  fillItemPool(g, reserved);

  const bandHi = reserved;
  const bandLo = Math.min(g.rng.dice(2, 3, 1), bandHi);
  const last = Math.min(22, Math.floor(g.map.sites.length / 3) + g.rng.dice(1, 5, -3) + 8);

  const free = g.map.sites.filter((s) => s.content === EMPTY);
  g.rng.shuffle(free);

  const byIndex = {};
  for (const it of g.map.items) {
    byIndex[it.index] = it;
    it.status = 0;
  }

  for (let idx = 8; idx <= last - 1; idx++) {
    const item = byIndex[idx];
    // the band between bandLo and bandHi is held back, probably for quests
    if (item && !(idx >= 8 + bandLo && idx < 8 + bandHi)) {
      const want = itemReserved(item);
      for (let i = 0; i < free.length; i++) {
        const s = free[i];
        if (s.rich === want) {
          s.content = ITEM; s.item = idx;
          item.status = 2;                        // hidden in a ruin
          free.splice(i, 1);
          break;
        }
      }
    }
  }

  for (const s of g.map.sites) {
    if (s.content === EMPTY) s.content = g.rng.pick(CONTENT[s.band]);
    if (s.content === TEMPLE) {
      s.guardian = undefined;
    } else if (s.content === ALLIES) {
      s.allyType = g.rng.pick(ALLY_TYPES[s.band]);
      s.guardian = g.rng.dice(1, 9, 0);
    } else {
      s.guardian = g.rng.dice(1, 9, 0);
    }
  }
  g.map.siteAt = {};
  for (const s of g.map.sites) g.map.siteAt[s.y * g.map.width + s.x] = s;
}

function heroIn(stack) {
  return stack.find((a) => a.type === armytype.HERO) || null;
}

/** Does the hero survive the ruin's guardian? */
export function survivesGuardian(g, h, stack, monsterStrength) {
  const margin = 90 + 5 * ((h.strength || 0) + combat.battleItems(h) - monsterStrength)
               + 3 * stack.length;
  return g.rng.dice(1, 100, 0) <= margin;
}

/** The strength of a site's guardian, from the scenario's monster table. */
export function guardianStrength(g, s) {
  if (s.guardian == null) return 0;
  const m = g.map.monsters[s.guardian];
  return m ? m.strength : 0;
}

/** Bless a stack at a temple: +1 strength (max 9), once per temple per army;
 *  only the first four temples can bless. */
export function bless(g, s, stack) {
  if ((s.templeIndex || 0) >= BLESSING_TEMPLES) return 0;
  const bit = s.templeIndex;
  let blessed = 0;
  for (const a of stack) {
    a.blessings = a.blessings || {};
    if (!a.blessings[bit]) {
      a.blessings[bit] = true;
      a.strength = Math.min(9, (a.strength || 0) + 1);
      blessed++;
      if (a.type === armytype.HERO) heroMod.addExperience(g, a, 1);
    }
  }
  return blessed;
}

/** Search the site under a stack (site_search, 6536:0000). A human chooses at
 *  a temple; a found item is left on the ground for Take to pick up. */
export function search(g, stack, x, y, human) {
  const s = g.map.siteAt && g.map.siteAt[y * g.map.width + x];
  if (!s || s.searched) return null;
  const h = heroIn(stack);

  if (s.content === TEMPLE) {
    if (human) return { site: s, kind: "temple", hero: h };
    const blessed = bless(g, s, stack);
    const out = { site: s, kind: "temple", blessed };
    if (h && g.map.options.quests !== 0) {
      const side = g.map.sides[h.owner];
      out.quest = quest.assign(g, side, h);
    }
    return out;
  }

  if (!h) return { site: s, kind: "no hero" };
  s.searched = true;

  if (s.content === SAGE) {
    heroMod.addExperience(g, h, 3);
    history.deed(g, g.map.sides[h.owner], history.FINDS, history.SAGE, 0, h.name);  // 6536:013c
    return { site: s, kind: "sage", hero: h };
  }

  heroMod.addExperience(g, h, 3);

  let beaten;
  if (s.guardian != null && s.guardian > 0) {
    const str = guardianStrength(g, s);
    beaten = g.map.monsters[s.guardian];
    if (!survivesGuardian(g, h, stack, str)) {
      heroMod.dropItems(g, h, x, y);
      history.deed(g, g.map.sides[h.owner], history.KILLED, history.SEARCHING, 0, h.name);  // 6536:02ef
      const i = g.armies.indexOf(h);
      if (i >= 0) g.armies.splice(i, 1);
      return { site: s, kind: "killed", monster: g.map.monsters[s.guardian], hero: h };
    }
  }

  if (s.content === ITEM) {
    let found;
    for (const it of g.map.items) if (it.index === s.item) found = it;
    if (found && human) {
      found.status = 1; found.x = x; found.y = y;    // on the ground, to take
    } else if (found) {
      found.status = 3;                              // carried
      h.items = h.items || [];
      h.items.push(found);
    }
    const side = g.map.sides[h.owner];
    if (found) history.deed(g, side, history.FINDS, found.index, 0, h.name);   // 6536:0571
    const q = quest.event(g, side, "item", { hero: h });
    return { site: s, kind: "item", item: found, hero: h, quest: q, guardian: beaten };
  } else if (s.content === GOLD) {
    const gold = s.rich ? g.rng.dice(3, 1000, 1000) : g.rng.dice(3, 500, 500);
    const side = g.map.sides[h.owner];
    side.gold += gold;
    return { site: s, kind: "gold", gold, hero: h, guardian: beaten };
  } else if (s.content === ALLIES) {
    const type = g.types.byId[s.allyType] || g.types.byId[armytype.SCOUTS];
    const n = s.rich ? g.rng.dice(1, 2, 2) : g.rng.dice(1, 2, 0);
    const joined = [];
    for (let k = 0; k < n; k++) {
      let ax = x, ay = y;
      if (game.armiesAt(g, ax, ay).length >= rules.MAX_STACK) {
        for (let dx = -1; dx <= 1; dx++) {
          for (let dy = -1; dy <= 1; dy++) {
            const nx = x + dx, ny = y + dy;
            if (nx >= 0 && ny >= 0 && nx < g.map.width && ny < g.map.height
                && game.armiesAt(g, nx, ny).length < rules.MAX_STACK
                && move.COST[scn.terrainAt(g.map, nx, ny)] !== 0) {
              ax = nx; ay = ny;
            }
          }
        }
      }
      if (game.armiesAt(g, ax, ay).length < rules.MAX_STACK) {
        const a = heroMod.newAlly(g, type, ax, ay, h.owner, h.homeCity);
        g.armies.push(a);
        joined.push(a);
      }
    }
    history.deed(g, g.map.sides[h.owner], history.FINDS, history.ALLIES, 0, h.name);  // 6536:07a1
    return { site: s, kind: "allies", armies: joined, type, hero: h, guardian: beaten };
  }
  return { site: s, kind: "empty", hero: h, guardian: beaten };
}

// What a sage can tell a side, for its hero at (hx, hy) (6536:1610).
export const SAGE_RANGE = 35;

function sageDistance(s, hx, hy) {
  return Math.floor(Math.sqrt((s.x - hx) ** 2 + (s.y - hy) ** 2));   // 2012:1199
}

export function shownTo(s, side) {
  return (((s.revealed || 0) >> side.index) & 1) === 1;
}

function unshown(g, side, s, hx, hy) {
  return s.rich && !s.searched && !shownTo(s, side) && sageDistance(s, hx, hy) < SAGE_RANGE;
}

export function sageList(g, side, hx, hy) {
  const list = [];
  let gold = false, allies = false;
  for (const s of g.map.sites) {
    if (unshown(g, side, s, hx, hy)) {
      if (s.content === GOLD && !gold) {
        gold = true;
        list.push({ kind: "gold", name: "Gold" });
      } else if (s.content === ALLIES && !allies) {
        allies = true;
        list.push({ kind: "allies", name: "Allies" });
      } else if (s.content === ITEM) {
        for (const it of g.map.items) {
          if (it.index === s.item) list.push({ kind: "item", item: it, name: it.name });
        }
      }
    }
  }
  return list;
}

/** Show the side where an entry of sageList lies (6536:0e85). */
export function sageShow(g, side, entry, hx, hy) {
  let found = null, best = null;
  for (const s of g.map.sites) {
    if (entry.kind === "item") {
      if (s.content === ITEM && s.item === entry.item.index) { found = s; break; }
    } else if (unshown(g, side, s, hx, hy)
               && s.content === (entry.kind === "gold" ? GOLD : ALLIES)) {
      const d = sageDistance(s, hx, hy);
      if (best === null || d < best) { found = s; best = d; }
    }
  }
  if (!found) return null;
  if (!shownTo(found, side)) found.revealed = (found.revealed || 0) + (1 << side.index);
  game.reveal(g, side.index, found.x, found.y, false);
  return found;
}

/** The sage's gem (6536:0b1a): 3d500 + 500 gold. */
export function sageGem(g, side) {
  const n = g.rng.dice(3, 500, 500);
  side.gold += n;
  return n;
}

/** Uncover a patch of the map round the tile pointed at (6536:0cd6):
 *  [x, y, w, h] in tiles. */
export function sageMap(g, side, cx, cy) {
  const x0 = Math.max(0, cx - g.rng.dice(1, 5, 8));
  const y0 = Math.max(0, cy - g.rng.dice(1, 5, 8));
  let w = g.rng.dice(1, 10, 15), h = g.rng.dice(1, 10, 15);
  if (x0 + w >= g.map.width) w = g.map.width - x0 - 1;
  if (y0 + h >= g.map.height) h = g.map.height - y0 - 1;
  for (let x = x0; x < x0 + w; x++) {
    for (let y = y0; y < y0 + h; y++) game.reveal(g, side.index, x, y, false);
  }
  return [x0, y0, w, h];
}
