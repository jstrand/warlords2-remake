// The computer players' movement phases: move (standing orders), rescue, last
// rescue, move search and move explore, specials -- and the explorer walk and
// hero parties they use. docs/re/ai.md > Movement phases.
//
// Everything that walks is a generator (see core.js).

import * as scn from "../scn.js";
import * as move from "../move.js";
import * as game from "../game.js";
import * as siteMod from "../site.js";
import * as core from "./core.js";
import * as cities from "./cities.js";
import * as diplo from "./diplomacy.js";

function city(g, i) { return i != null ? g.map.cities[i] : undefined; }

function onCityTile(g, a) {
  const c = core.cityAt(g, a.x, a.y);
  return c != null && core.standing(c);
}

/** The computer's visit to a sage (5e97:0080). */
function sage(g, side) {
  const d = core.data(g, side);
  const gold = g.rng.dice(3, 500, 500);
  let best = null, bestCount = -1, bestD = 0;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex == null && core.cflag(d, c, core.CF_UNSEEN)) {
      let nextToUs = false;
      for (const n of core.neighbours(g, c)[0]) if (n.ownerIndex === side.index) nextToUs = true;
      if (nextToUs) {
        let count = 0, dOwn = 1000;
        for (const o of g.map.cities) {
          if (o.ownerIndex == null) {
            if (core.cflag(d, o, core.CF_UNSEEN) && core.dist(c.x, c.y, o.x, o.y) < 20) count++;
          } else if (o.ownerIndex === side.index) {
            const dd = core.dist(c.x, c.y, o.x, o.y);
            if (dd < dOwn) dOwn = dd;
          }
        }
        if (count > 3 && (bestCount < count || (count === bestCount && bestD < dOwn))) {
          best = c; bestCount = count; bestD = dOwn;
        }
      }
    }
  }
  if (g.map.options.hiddenMap === 0 || !best) {
    side.gold += gold;
    return;
  }
  let x = Math.max(0, best.x + g.rng.dice(1, 11, -6));
  let y = Math.max(0, best.y + g.rng.dice(1, 11, -6));
  x = Math.min(x, g.map.width - 1); y = Math.min(y, g.map.height - 1);
  siteMod.sageMap(g, side, x, y);
  move.invalidate(g);
}

/** A hero searches the site it has reached (5ad0:15c3). Returns 2 when it
 *  searched. */
export function* searchSite(g, sel, h, s) {
  if (!(h && core.alive(g, h) && h.x === s.x && h.y === s.y && (h.moves || 0) !== 0)) return 0;
  const side = g.map.sides[h.owner];
  if (core.siteOpen(s)) {
    const stack = sel ? sel.armies : [h];
    const r = siteMod.search(g, stack, s.x, s.y, false);
    if (r && r.kind === "sage") sage(g, side);
    if (!core.isHero(h)) return 0;
    for (const a of stack) a.done = undefined;
    if (g.map.options.hiddenMap !== 0 && s.content === siteMod.ALLIES) {
      for (const a of core.onTile(g, side.index, s.x, s.y)) {
        if ((a.aiOrder || 0) === 0 && core.magical(g, a)) {
          a.aiExplore = true;
          yield* explore(g, side, a, null);
        }
      }
    }
  }
  return 2;
}

/** Send a hero and one flying companion off (5ad0:11a6). Returns 0 when it
 *  went, 1 when the hero has no flier with it, 2 when it searched. */
export function* party(g, side, list) {
  let hero = null, flier = null;
  for (const a of list) {
    if (!hero && core.isHero(a)) hero = a;
    if (!flier && core.flies(g, a)) flier = a;
  }
  if (!hero) return 0;
  if (!flier) return 1;
  const d = core.data(g, side);
  list = [hero, flier];
  core.clearOrder(hero); core.clearOrder(flier);
  let score = -1, bestSite = null, bestCity = null;
  for (let i = g.map.sites.length - 1; i >= 0; i--) {
    const s = g.map.sites[i];
    let taken = false;
    for (const o of core.armies(g, side.index)) {
      if (o !== hero && core.isHero(o) && o.aiOrder === core.ORDER_SITE && o.aiDest === s.index) taken = true;
    }
    if (!taken && core.explored(g, side.index, s.x, s.y) && s.content !== siteMod.TEMPLE && !s.searched) {
      const dd = core.dist(hero.x, hero.y, s.x, s.y);
      let base = null;
      if (dd < 15 && score < 215 - dd) base = 215;
      else if (!(dd > 39 || 90 - dd <= score)) base = 90;
      if (base !== null) { score = base - dd; bestSite = s; }
    }
  }
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (c.ownerIndex === side.index) {
      let dd = core.dist(hero.x, hero.y, c.x, c.y);
      if (core.role(d, c) === core.RALLY) dd -= 80;
      let base = null;
      if (dd < 15 && score < 115 - dd) base = 115;
      else if (!(dd > 39 || 40 - dd <= score)) base = 40;
      if (base !== null) { score = base - dd; bestCity = c; }
    }
  }
  if (bestSite) {
    const sel = core.order(g, list, core.ORDER_SITE, bestSite.index, 0x100);
    if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
    return yield* searchSite(g, sel, hero, bestSite);
  }
  if (bestCity && core.dist(hero.x, hero.y, bestCity.x, bestCity.y) > 2) {
    const sel = core.order(g, list, core.ORDER_CITY, bestCity.index, 0);
    if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
  }
  return 0;
}

/** Look over the 10 x 10 tiles round an explorer (57ea:177f) for where to go:
 *  [found, tx, ty, kind, ex, ey]. */
function scan(g, side, a, sel) {
  const home = city(g, a.homeCity) || side.capital || { x: a.x, y: a.y };
  let coast = a.atSea;
  if (g.rng.dice(1, 4, -1) === 0 && onCityTile(g, a) && !core.isHero(a)) {
    const c = core.cityAt(g, a.x, a.y);
    if (c && move.isPort(g, c)) coast = true;
  }
  const flying = sel.mode === move.FLYING;
  const hills = move.stackMode(g, sel.armies)[2];
  let kind = 0, ex = null, ey = null;
  let tx = null, ty = null, found = false, best = 0;
  const exploredRound = (x, y) => {
    for (let nx = x - 1; nx <= x + 1; nx++) {
      for (let ny = y - 1; ny <= y + 1; ny++) {
        if (nx >= 0 && ny >= 0 && nx < g.map.width && ny < g.map.height && game.seen(g, side.index, nx, ny)) {
          return true;
        }
      }
    }
    return false;
  };
  for (let x = a.x - 5; x <= a.x + 4; x++) {
    for (let y = a.y - 5; y <= a.y + 4; y++) {
      if (x >= 0 && y >= 0 && x < g.map.width && y < g.map.height) {
        const t = core.terrain(g, x, y);
        const c = core.cityAt(g, x, y);
        if (t === move.CITY && (!c || c.ownerIndex !== side.index)) {
          const dd = flying ? 2 : core.dist(a.x, a.y, x, y);
          if (dd < 4 && (!a.atSea || (c && move.isPort(g, c))) && exploredRound(x, y)) {
            kind = 1; ex = x; ey = y;
          }
        }
        if (t === move.SITE && kind === 0 && !a.atSea && exploredRound(x, y)) {
          kind = 2; ex = x; ey = y;
        }
        const road = scn.roadAt(g.map, x, y) % 0x20 !== 0;
        if (t !== move.CITY && (flying || road || hills || t !== move.HILLS)
            && !game.seen(g, side.index, x, y) && (flying || move.COST[t] !== 0)) {
          let unseen = 0;
          for (let nx = x - 1; nx <= x + 1; nx++) {
            for (let ny = y - 1; ny <= y + 1; ny++) {
              if (nx >= 0 && ny >= 0 && nx < g.map.width && ny < g.map.height
                  && !game.seen(g, side.index, nx, ny)) {
                unseen++;
              }
            }
          }
          if (unseen > 1) {
            let bonus = 0;
            const wet = t === move.WATER || t === move.SHORE;
            if (coast) {
              if (wet) bonus += 200;
            } else if (!wet) {
              if (t !== move.HILLS) bonus = 100;
              if (road) bonus += 200;
            }
            const fromHome = core.dist(x, y, home.x, home.y);
            let s = g.rng.dice(1, 10, 0) + core.dist(x, y, a.x, a.y) * 2 + fromHome + unseen * 10 + bonus;
            if (coast && bonus !== 0) s += Math.min(fromHome, 50) * 10;
            if (best < s) { tx = x; ty = y; found = true; best = s; }
          }
        }
      }
    }
  }
  return [found, tx, ty, kind, ex, ey];
}

/** A flier's fallback: the nearest unseen city, give or take 1d10 (57ea:164e);
 *  it aims one up and left of it. [x, y] or [null, null]. */
function unseenCity(g, side, a) {
  const d = core.data(g, side);
  let best = null, bestD = null;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (core.standing(c) && core.cflag(d, c, core.CF_UNSEEN)) {
      const dd = core.dist(c.x, c.y, a.x, a.y) + g.rng.dice(1, 10, 0);
      if (bestD === null || dd < bestD) { best = c; bestD = dd; }
    }
  }
  if (best) return [best.x - 1, best.y - 1];
  return [null, null];
}

/** An explorer next to a city goes for it (57ea:10a8) if it may and the odds
 *  are good. True if it went. */
function* tryCity(g, side, a, ex, ey) {
  const d = core.data(g, side);
  let pick = null;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    if (core.standing(c) && c.ownerIndex !== side.index && d.questCity !== c.index
        && core.dist(c.x, c.y, ex, ey) <= 2 && diplo.canAttack(g, side, c)) {
      if (core.cflag(d, c, core.CF_UNSEEN)) cities.clearUnseen(g, side);
      if (!core.cflag(d, c, core.CF_UNSEEN)) { pick = c; break; }
    }
  }
  if (!pick) return false;
  let sel = core.order(g, [a], core.ORDER_ROAM, 0, 0x20);
  if (!sel) return false;
  const o = core.odds(g, sel, pick.x, pick.y);
  let need = 75;
  if (!core.isHero(a) && (a.strength || 0) < 4) {
    if (d.neutral < Math.floor(g.map.cities.length / 10)) need = 50;
    else if (d.own <= Math.min(Math.floor(g.turn / 2), 10)) need = 40;
  }
  if (o < need) return false;
  a.aiExplore = undefined;
  sel = core.order(g, [a], core.ORDER_CITY, pick.index, 0x80);
  if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
  return true;
}

/** An explorer next to a site visits it (57ea:12eb). True if it went. */
function* trySite(g, side, a, sx, sy) {
  const s = core.siteAt(g, sx, sy);
  if (!s || !core.siteOpen(s) || a.atSea) return false;
  const temple = s.content === siteMod.TEMPLE;
  if (temple && a.blessings && s.templeIndex != null && a.blessings[s.templeIndex]) return false;
  if (!(core.isHero(a) || temple)) return false;
  const there = game.armiesAt(g, sx, sy);
  if (there[0] && there[0].owner !== side.index) return false;
  const sel = core.order(g, [a], core.ORDER_ROAM, 0, 0);
  if (!sel) return false;
  sel.leader.target = { x: sx, y: sy };
  const r = yield* core.moveTo(g, sel, sx, sy);
  if (r === 1) return false;
  if (core.alive(g, a)) {
    if (a.x === sx && a.y === sy) {
      if (core.isHero(a)) yield* searchSite(g, sel, a, s);
      else siteMod.search(g, [a], sx, sy, false);
      if (s.content === siteMod.ALLIES) {
        for (const b of core.onTile(g, side.index, sx, sy)) {
          if ((b.aiOrder || 0) === 0 && core.magical(g, b)) b.aiExplore = true;
        }
      }
    } else {
      if (temple && a.atSea && s.templeIndex != null) {
        a.blessings = a.blessings || {};
        a.blessings[s.templeIndex] = true;
      }
      a.done = true;
    }
    a.aiExplore = true;
    core.clearOrder(a);
    a.target = undefined;
  }
  return true;
}

/** Walk an explorer's stack to (x, y) (57ea:1581). */
function* walkTo(g, a, x, y) {
  a.target = { x, y };
  const sel = core.selectStackOf(g, a);
  const r = yield* core.moveTo(g, sel, x, y);
  if (core.alive(g, a)) {
    if (r === 1) {
      a.aiExplore = undefined;
      core.clearOrder(a);
    }
    a.done = true;
    a.target = undefined;
    a.group = undefined;
  }
}

/** One step of an explorer (57ea:0f46). */
function* exploreStep(g, side, a, home) {
  const sel = core.select(g, [a]);
  let [found, tx, ty, kind, ex, ey] = scan(g, side, a, sel);
  if (!found) {
    if (core.flies(g, a)) [tx, ty] = unseenCity(g, side, a);
    if (tx == null) {
      const n = home ? cities.openNeutral(g, side, home, {}) : null;
      if (!n) {
        core.clearOrder(a);
        a.aiExplore = undefined;
        a.target = undefined;
        a.group = undefined;
        return;
      }
      tx = n.x; ty = n.y;
    }
  } else if (kind === 1) {
    if (yield* tryCity(g, side, a, ex, ey)) return;
  } else if (kind === 2) {
    if (yield* trySite(g, side, a, ex, ey)) return;
  }
  yield* walkTo(g, a, tx, ty);
}

/** Send an army exploring (57ea:0e4b): steps while it has moves. */
export function* explore(g, side, a, home) {
  if (!core.alive(g, a)) return;
  core.order(g, [a], core.ORDER_ROAM, 0, 0x20);
  let steps = Math.floor((a.moves || 0) / 5) + 1;
  for (;;) {
    if (!core.alive(g, a) || (a.moves || 0) < 2) return;
    const px = a.x, py = a.y;
    yield* exploreStep(g, side, a, home);
    if (!core.alive(g, a) || (a.x === px && a.y === py)) return;
    if ((a.moves || 0) < 4) return;
    if (steps === 0) return;
    if (!a.aiExplore) return;
    steps--;
  }
}

/** move search (ai_phase_move_search, 5ad0:0284), with the map hidden. */
export function* search(g, side) {
  const d = core.data(g, side);
  let rally = false;
  for (const c of g.map.cities) {
    if (c.ownerIndex === side.index && core.role(d, c) === core.RALLY) rally = true;
  }
  d.searchers = 0; d.explorers = 0;
  for (let pass = 0; pass <= 1; pass++) {
    for (const a of core.armies(g, side.index)) {
      if (core.alive(g, a) && !a.transit && a.aiExplore && ((pass === 0) === core.isHero(a))) {
        if ((core.isHero(a) && rally) || (core.flies(g, a) && d.explorers > 7 && (a.strength || 0) < 4)) {
          core.clearOrder(a);
          a.aiExplore = undefined;
        } else {
          yield* explore(g, side, a, null);
          if (core.alive(g, a) && a.owner === side.index) {
            if (core.flies(g, a)) d.explorers++;
            else d.searchers++;
          }
        }
      }
    }
  }
}

/** move explore (ai_phase_move_explore, 5ad0:0458). */
export function* heroParties(g, side) {
  for (const a of core.armies(g, side.index)) {
    if (core.alive(g, a) && !a.transit && a.aiParty) {
      a.aiParty = undefined;
      if (core.isHero(a)) {
        let going = true, guard = 0;
        while (going && guard < 20) {
          guard++;
          if (!core.alive(g, a)) break;
          const list = core.collectOrdered(g, side.index, a.x, a.y, a.aiGroup, a.aiOrder, 0);
          if (list.length === 0 || (a.moves || 0) < 3) break;
          const px = a.x, py = a.y;
          const r = yield* party(g, side, list);
          if (r === 0 || r === 1) going = false;
          if (core.alive(g, a) && a.x === px && a.y === py && (a.maxMoves || 0) > (a.moves || 0)) break;
        }
      }
    }
  }
}

/** Is the stack's neutral target still the thing to do (5ad0:05ff)? */
function* neutralStillOn(g, side, sel) {
  const d = core.data(g, side);
  const lead = sel.leader;
  const dest = city(g, lead.aiDest);
  if (!dest) return true;
  if (dest.ownerIndex === side.index) {
    const here = core.cityAt(g, lead.x, lead.y);
    let nb = here ? cities.neutralNeighbours(g, side, here) : [];
    if (nb.length === 0) nb = cities.neutralNeighbours(g, side, dest);
    let odds = 100;
    let best = null, bd = 1000;
    for (let j = nb.length - 1; j >= 0; j--) {
      const n = nb[j].city;
      if (!core.cflag(d, n, core.CF_UNSEEN)) {
        if (g.map.options.neutralCities !== 0) odds = core.odds(g, sel, n.x, n.y);
        const dd = core.dist(lead.x, lead.y, n.x, n.y);
        if (odds > 74 && dd < bd) { best = n; bd = dd; }
      }
    }
    if (!best) {
      if (g.map.options.hiddenMap !== 0 && g.turn < 10) {
        for (const a of sel.armies) yield* explore(g, side, a, null);
        return false;
      }
      return true;
    }
    for (const a of sel.armies) {
      a.aiOrder = core.ORDER_CITY; a.aiDest = best.index;
      core.setTarget(g, a, core.ORDER_CITY, best.index);
    }
    return true;
  }
  if (!diplo.canAttack(g, side, dest)) {
    for (const a of sel.armies) {
      core.clearOrder(a);
      a.target = undefined;
    }
    return false;
  }
  return true;
}

/** move #1 and #2 (ai_phase_move, 5ad0:0000). */
export function* moveAll(g, side) {
  const d = core.data(g, side);
  for (const a of core.armies(g, side.index)) {
    if (!a.transit) a.aiMoved = (a.moves || 0) < 2 || undefined;
  }
  d.cursor = d.cursor || { x: side.capX || 0, y: side.capY || 0 };
  for (;;) {
    let pick = null, pd = null;
    for (const a of core.armies(g, side.index)) {
      if (!a.transit && a.x != null && !a.done && !a.aiMoved) {
        let dd = Math.abs(a.x - d.cursor.x) + Math.abs(a.y - d.cursor.y);
        if (dd === 0) dd = 9000;
        if (pd === null || dd < pd) { pick = a; pd = dd; }
      }
    }
    if (!pick) break;
    d.cursor = { x: pick.x, y: pick.y };
    pick.aiMoved = true;
    if (pick.target && pick.target.x != null && pick.target.x >= 0 && (pick.moves || 0) > 1) {
      const sel = core.selectStackOf(g, pick);
      const lead = sel.leader;
      const ord = lead.aiOrder || 0;
      let dest = city(g, lead.aiDest);
      if (ord === core.ORDER_NONE || (ord === core.ORDER_CITY
          && (!dest || lead.aiDest === d.questCity || !core.standing(dest)))) {
        for (const a of sel.armies) {
          core.clearOrder(a);
          a.target = undefined;
          a.done = true;
        }
      } else if (!lead.aiNeutral || (yield* neutralStillOn(g, side, sel))) {
        dest = city(g, lead.aiDest);
        if (lead.aiOrder === core.ORDER_CITY && dest && dest.ownerIndex === side.index) {
          lead.target = { x: dest.x + 1, y: dest.y + 1 };
        }
        if (lead.target && core.alive(g, lead)) {
          yield* core.moveTo(g, sel, lead.target.x, lead.target.y);
        }
      }
    }
  }
}

/** The best city for an idle stack by the flood (5ad0:0f3c). */
function idleTarget(g, side, sel, flood, n) {
  const d = core.data(g, side);
  let best = null, bestD = 1000;
  for (let i = g.map.cities.length - 1; i >= 0; i--) {
    const c = g.map.cities[i];
    const own = core.owner(c);
    if (core.standing(c) && !core.cflag(d, c, core.CF_UNSEEN) && d.questCity !== c.index
        && (own === core.NEUTRAL || own === side.index || core.state(g, side.index, own) === 2)) {
      let [dd, px] = core.cityDistance(g, flood, c);
      if (px != null) {
        if (own === side.index) {
          dd += core.role(d, c) === core.RALLY ? 10 : 30;
          if (n > 3) dd += 100;
          if (dd < bestD) { best = c; bestD = dd; }
        } else if (core.odds(g, sel, c.x, c.y) > 74 && dd < bestD) {
          best = c; bestD = dd;
        }
      }
    }
  }
  return best;
}

/** An idle army out in the field (5ad0:0c5b). */
export function* sendIdle(g, side, a) {
  let home = 1000;
  for (const c of g.map.cities) {
    if (c.ownerIndex === side.index && core.standing(c)) {
      const dd = core.dist(a.x, a.y, c.x, c.y);
      if (dd < home) home = dd;
    }
  }
  const weak = (a.strength || 0) < 3 || (a.maxMoves || 0) < 8;
  if (!core.flies(g, a) && weak) {
    game.disband(g, side, [a]);
    return;
  }
  let list = core.collect(g, side.index, a.x, a.y, 8);
  if (list.length === 0) return;
  let fl = 0, he = 0, ot = 0;
  for (const b of list) {
    if (core.flies(g, b)) fl++;
    else if (core.isHero(b)) he++;
    else ot++;
  }
  if (fl !== 0 && he !== 0 && ot !== 0) list = list.filter((b) => core.flies(g, b) || core.isHero(b));
  if (list.length < 2 && weak) {
    game.disband(g, side, [a]);
    return;
  }
  let sel = core.select(g, list);
  let range = sel.hero ? 50 : Math.floor((a.strength || 0) / 2) * 10;
  range = Math.min(range, home + 10);
  const flood = core.floodFrom(g, side.index, a.x, a.y, range, sel);
  const c = idleTarget(g, side, sel, flood, list.length);
  if (!c) {
    game.disband(g, side, list);
    return;
  }
  sel = core.order(g, list, core.ORDER_CITY, c.index, 0);
  if (sel) yield* core.moveTo(g, sel, sel.leader.target.x, sel.leader.target.y);
}

/** rescue (ai_phase_rescue, 5ad0:0888). */
export function* rescue(g, side) {
  for (const a of core.armies(g, side.index)) {
    if (core.alive(g, a) && !a.transit && a.x != null && (a.maxMoves || 0) <= (a.moves || 0)) {
      if ((a.moves || 0) === (a.maxMoves || 0) + 2 && !onCityTile(g, a) && a.aiOrder === core.ORDER_CITY) {
        const dest = city(g, a.aiDest);
        if (!dest || !core.standing(dest)) {
          const gone = a.aiDest;
          for (const b of core.onTile(g, side.index, a.x, a.y)) {
            if (b.aiOrder === core.ORDER_CITY && b.aiDest === gone) {
              core.clearOrder(b);
              b.aiGroup = 0;
            }
          }
        }
      }
      if (core.alive(g, a) && !a.aiExplore && (a.aiOrder || 0) === 0 && !onCityTile(g, a)) {
        yield* sendIdle(g, side, a);
      }
    }
  }
}

/** last rescue (ai_phase_last_rescue, 5ad0:0b4d). */
export function* lastRescue(g, side) {
  for (const a of core.armies(g, side.index)) {
    if (core.alive(g, a) && !a.transit && a.x != null
        && (a.moves || 0) === (a.maxMoves || 0) + 2 && !onCityTile(g, a)) {
      a.aiExplore = undefined; a.aiNeutral = undefined;
      core.clearOrder(a);
      a.target = undefined;
      yield* sendIdle(g, side, a);
    }
  }
}

/** specials (ai_phase_specials, 5ad0:10e3). */
export function* specials(g, side) {
  const d = core.data(g, side);
  for (const c of cities.own(g, side)) {
    if (core.cflag(d, c, core.CF_HERO) && core.role(d, c) !== core.RALLY) {
      const list = core.collectOrdered(g, side.index, c.x + 1, c.y, 0, 0, 0);
      if (list.length !== 0 && (yield* party(g, side, list)) === 1 && !cities.building(c)) {
        core.setRole(d, c, core.EXPLORER);
      }
    }
  }
}
