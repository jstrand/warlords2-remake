// The computer players' hero phase (ai_phase_move_hero, 6087:0000): each hero
// weighs a quest temple, a site, an item lying about and an enemy city --
// 100 - distance + 1d20 each -- and goes for the best, twice at most.
// docs/re/ai.md; addresses are Ghidra's.

import * as siteMod from "../site.js";
import * as quest from "../quest.js";
import * as core from "./core.js";
import * as cities from "./cities.js";
import * as moves from "./moves.js";

function blessed(h, s) {
  return s.templeIndex != null && h.blessings != null && h.blessings[s.templeIndex] === true;
}

/** Is a site still worth the hero's while (6087:1637)? */
function worth(g, side, s, h) {
  const dd = core.dist(s.x, s.y, h.x, h.y);
  const reach = g.turn * 2 + (s.rich ? 15 : 3);
  return dd < reach && core.explored(g, side.index, s.x, s.y) && core.siteOpen(s);
}

/** Is an item lying where a hero may take it (6087:1736)? */
function lying(g, side, it) {
  if (it.status !== 1 || it.x == null || it.planted) return false;
  if (!core.explored(g, side.index, it.x, it.y)) return false;
  const c = core.cityAt(g, it.x, it.y);
  return !c || !core.standing(c) || c.ownerIndex === side.index;
}

/** The side's heroes, six at most, with the city or site each stands on
 *  (6087:065d). */
function survey(g, side) {
  const d = core.data(g, side);
  const info = { heroes: [], city: [], site: [] };
  d.heroes = 0;
  for (const a of core.armies(g, side.index)) {
    if (core.isHero(a) && !a.transit && info.heroes.length < 6) {
      const k = info.heroes.length;
      info.heroes[k] = a;
      const c = core.cityAt(g, a.x, a.y);
      if (c && core.standing(c)) info.city[k] = c;
      const s = core.siteAt(g, a.x, a.y);
      if (s) info.site[k] = s;
      d.heroes++;
    }
  }
  return info;
}

/** Temples and ruins in reach (6087:0fda). */
function sites(g, side, k, info) {
  const h = info.heroes[k];
  const range = h.aiOrder === core.ORDER_CITY ? 5 : 25;
  let questD = 100, questS = null, siteD = 100, site = null;
  for (const s of g.map.sites) {
    if (worth(g, side, s, h)) {
      const dd = core.dist(s.x, s.y, h.x, h.y);
      let ok = true;
      if (s.content === siteMod.TEMPLE) {
        const free = g.map.options.quests !== 0 && side.quest == null && dd < range && dd < questD;
        if (free) { questD = dd; questS = s; }
        else if (blessed(h, s)) ok = false;
      } else {
        info.heroes.forEach((o, j) => {
          if (j !== k && o.aiOrder === core.ORDER_SITE && o.aiDest === s.index) ok = false;
        });
      }
      if (ok && dd <= range && dd < siteD) { siteD = dd; site = s; }
    }
  }
  info.quest = questS; info.questD = questD; info.target = site; info.siteD = siteD;
}

/** Items lying about within 5 (sent at a city) or 15 (6087:11f8). */
function items(g, side, k, info) {
  const h = info.heroes[k];
  const range = h.aiOrder === core.ORDER_CITY ? 5 : 15;
  let best = null, bestD = 100;
  for (let i = g.map.items.length - 1; i >= 0; i--) {
    const it = g.map.items[i];
    if (lying(g, side, it)) {
      let taken = false;
      info.heroes.forEach((o, j) => {
        if (j !== k && o.aiOrder === core.ORDER_ITEM && o.aiDest === it.index) taken = true;
      });
      if (!taken) {
        const dd = core.dist(it.x, it.y, h.x, h.y);
        if (dd <= range && dd < bestD) { best = it; bestD = dd; }
      }
    }
  }
  info.item = best; info.itemD = bestD;
}

/** An enemy city the hero's stack beats (6087:1348). */
function enemy(g, side, k, info) {
  const d = core.data(g, side);
  const h = info.heroes[k];
  info.enemy = null; info.enemyD = 20;
  const dest = h.aiOrder === core.ORDER_CITY ? h.aiDest : undefined;
  const list = core.collectOrdered(g, side.index, h.x, h.y, h.aiGroup, h.aiOrder, 12);
  if (list.length === 0) return;
  const sel = core.select(g, list);
  const need = list.length < 3 ? 95 : 75;
  let bestOdds = 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (core.standing(c) && !core.cflag(d, c, core.CF_UNSEEN) && c.ownerIndex != null
        && core.state(g, side.index, c.ownerIndex) === 2) {
      const dd = core.dist(c.x, c.y, h.x, h.y);
      if (dd <= info.enemyD || dest === c.index) {
        const o = core.odds(g, sel, c.x, c.y) + (dest === c.index ? 20 : 0);
        if (o >= need && (bestOdds < o || (o === bestOdds && dd < info.enemyD))) {
          info.enemy = c; info.enemyD = dd; bestOdds = o;
        }
      }
    }
  }
}

/** Pick (ai_choose_target, 6087:0efc): 1 the quest temple, 3 the site, 4 the
 *  enemy city, 2 the item, 0 nothing. */
function choose(g, side, k, info) {
  const d = core.data(g, side);
  let r = g.rng.dice(1, 20, 0);
  let pick = info.quest ? 1 : 0;
  let best = 0;
  if (info.quest) best = 100 - info.questD + r;
  r = g.rng.dice(1, 20, 0);
  let s = 100 - info.siteD + r;
  if (info.target && best < s) { pick = 3; best = s; }
  const c = info.city[k];
  if (!c || (d.garrison[c.index] || 0) > 4) {
    r = g.rng.dice(1, 20, 0);
    s = 100 - info.enemyD + r;
    if (info.enemy && best < s) { pick = 4; best = s; }
    r = g.rng.dice(1, 20, 0);
    if (info.item && best < 100 - info.itemD + r) pick = 2;
  }
  return pick;
}

/** Go for the site (3) or the item (2) (6087:07ea). 1 when it got there. */
function* go(g, side, k, kind, info) {
  const d = core.data(g, side);
  const h = info.heroes[k];
  const target = kind === 3 ? info.target : info.item;
  if (!target) return 0;
  const tx = target.x, ty = target.y;
  let list = core.collectOrdered(g, side.index, h.x, h.y, h.aiGroup, h.aiOrder, 12);
  let n = list.length;
  if (n === 0) return 0;
  const c = info.city[k];
  let near = 100;
  if (c) {
    near = cities.nearestArmy(g, side, h.x, h.y);
    let flier = null;
    for (let i = list.length - 1; i >= 0; i--) if (core.flies(g, list[i])) flier = list[i];
    if (!flier) {
      if (near >= 15) { list = [h]; n = 1; }
    } else {
      list = [h, flier]; n = 2;
    }
  }
  if (!c || (d.garrison[c.index] || 0) !== n || n > 2 || near > 14) {
    let sel;
    if (kind === 3) {
      sel = core.order(g, list, core.ORDER_SITE, target.index, 0);
    } else {
      sel = core.order(g, list, core.ORDER_ROAM, 0, 0);
      if (sel) sel.leader.target = { x: tx, y: ty };
    }
    if (sel && (yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y)) === 4) return 1;
  }
  return 0;
}

/** Go for the enemy city (6087:0b75). */
function* attack(g, side, k, info) {
  const d = core.data(g, side);
  const h = info.heroes[k];
  const target = info.enemy;
  if (!target) return 0;
  const list = core.collectOrdered(g, side.index, h.x, h.y, h.aiGroup, h.aiOrder, 12);
  if (list.length === 0) return 0;
  const c = info.city[k];
  if (c && (d.garrison[c.index] || 0) === list.length) {
    const near = cities.nearestArmy(g, side, h.x, h.y);
    if (near < (g.turn < 6 ? 10 : 15)) return 0;
  }
  const sel = core.order(g, list, core.ORDER_CITY, target.index, 0);
  if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
  return 0;
}

/** The quest hero goes for the city it is to take or raze (6087:0cf3). */
function* questGo(g, side, k, info) {
  const d = core.data(g, side);
  const h = info.heroes[k];
  const q = side.quest;
  if (!(q && (q.type === quest.OCCUPY || q.type === quest.RAZE))) return false;
  const c = q.target;
  if (!c || core.cflag(d, c, core.CF_UNSEEN)) return false;
  const need = (c.ownerIndex == null || g.turn < 8) ? 1 : 2;
  const list = core.collect(g, side.index, h.x, h.y, 8);
  if (list.length < need) return false;
  if (!(h.aiOrder === core.ORDER_CITY && h.aiDest === c.index)) {
    const sel = core.select(g, list);
    if (core.odds(g, sel, c.x, c.y) < 75) return false;
  }
  const sel = core.order(g, list, core.ORDER_CITY, c.index, 0);
  if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
  return true;
}

/** move Hero (ai_phase_move_hero, 6087:0000). */
export function* phase(g, side) {
  const d = core.data(g, side);
  d.questCity = undefined;
  const q = side.quest;
  if (g.map.options.quests !== 0 && q && (q.type === quest.OCCUPY || q.type === quest.RAZE)
      && q.target && q.target.index != null) {
    d.questCity = q.target.index;
  }
  const info = survey(g, side);
  for (let k = info.heroes.length - 1; k >= 0; k--) {
    const h = info.heroes[k];
    if (h && core.alive(g, h) && h.owner === side.index && core.isHero(h)) {
      if (info.city[k]) yield* cities.garrison(g, side, info.city[k], true);
      const skip = g.map.options.quests !== 0 && side.quest && side.quest.hero === h
                   && (yield* questGo(g, side, k, info));
      if (!skip && core.alive(g, h)) {
        if (h.aiOrder === core.ORDER_SITE) {
          const s = g.map.sites[h.aiDest != null ? h.aiDest : -1];
          if (!s || !worth(g, side, s, h)) {
            for (const a of core.onTile(g, side.index, h.x, h.y)) {
              if ((a.aiOrder || 0) === (h.aiOrder || 0)) {
                core.clearOrder(a);
                a.target = undefined;
              }
            }
          } else {
            info.target = s;
            yield* go(g, side, k, 3, info);
          }
        }
        let step = 1, tries = 0, moved = false;
        while (step !== 0 && tries < 2 && core.alive(g, h) && (h.moves || 0) > 3) {
          tries++;
          sites(g, side, k, info);
          items(g, side, k, info);
          enemy(g, side, k, info);
          const pick = choose(g, side, k, info);
          if (pick === 1) {
            step = 0;
          } else if (pick === 2 || pick === 3) {
            moved = true;
            step = yield* go(g, side, k, pick, info);
          } else if (pick === 4) {
            moved = true;
            step = yield* attack(g, side, k, info);
          } else {
            if (moved && g.map.options.hiddenMap !== 0 && g.turn < 10
                && core.collect(g, side.index, h.x, h.y, 8).length === 1) {
              yield* moves.explore(g, side, h, null);
            }
            step = 0;
          }
          if (!core.alive(g, h) || h.owner !== side.index) step = 0;
        }
      }
    }
  }
}
