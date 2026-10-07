// Saving and loading a game in progress.
//
// This is the engine's own save format, JSON. Only what cannot be recomputed
// is written: everything derived from the data files -- the map, the army
// types, the terrain -- is reloaded from them. Armies are written with an id
// so the references between them (a quest's hero, a staged stack) survive.
// A random world has no files to reload, so its save carries them, as the
// original's saves carry the map.

import * as game from "./game.js";
import * as scn from "./scn.js";
import * as move from "./move.js";
import * as vfs from "../vfs.js";
import * as randommap from "./randommap.js";
import { latin1, bytesOf } from "../util.js";

export const VERSION = 1;


function saveAI(d, ids) {
  if (!d) return null;
  const out = Object.assign({}, d);
  out.groups = (d.groups || []).map((grp) => {
    if (!grp) return null;
    const copy = Object.assign({}, grp);
    copy.staged = {};
    for (let s = 1; s <= 4; s++) if (grp.staged[s]) copy.staged[s] = ids.get(grp.staged[s]);
    return copy;
  });
  return out;
}

// the hidden map, a side's mask as runs of seen and unseen tiles
function packMask(mask) {
  const runs = [];
  let cur = 0, n = 0;
  for (let i = 0; i < mask.length; i++) {
    const v = mask[i] ? 1 : 0;
    if (v === cur) n++;
    else { runs.push(n); cur = v; n = 1; }
  }
  runs.push(n);
  return runs;
}

function unpackMask(runs, size) {
  const mask = new Uint8Array(size);
  let i = 0, v = 0;
  for (const n of runs) {
    if (v) mask.fill(1, i, i + n);
    i += n;
    v = 1 - v;
  }
  return mask;
}

/** A game as a string. */
export function encode(g) {
  const ids = new Map();
  g.armies.forEach((a, i) => ids.set(a, i));

  const armies = g.armies.map((a) => ({
    x: a.x, y: a.y, owner: a.owner, type: a.type, name: a.name,
    female: a.female || undefined,
    strength: a.strength, moves: a.moves, maxMoves: a.maxMoves,
    upkeep: a.upkeep, homeCity: a.homeCity, atSea: a.atSea || undefined,
    group: (a.group || 0) !== 0 ? a.group : undefined,
    target: a.target ? { x: a.target.x, y: a.target.y } : undefined,
    fortified: a.fortified || undefined,
    level: a.level, experience: a.experience, title: a.title,
    items: a.items && a.items.length > 0 ? a.items.map((it) => it.index) : undefined,
    blessings: a.blessings,
    transit: a.transit ? { turns: a.transit.turns, dest: a.transit.dest } : undefined,
    returning: a.returning || undefined,
    aiOrder: (a.aiOrder || 0) !== 0 ? a.aiOrder : undefined, aiDest: a.aiDest,
    aiGroup: (a.aiGroup || 0) !== 0 ? a.aiGroup : undefined,
    aiExplore: a.aiExplore || undefined, aiNeutral: a.aiNeutral || undefined,
    aiParty: a.aiParty || undefined,
  }));

  const sides = g.sides.map((s) => {
    let quest;
    if (s.quest) {
      const q = s.quest;
      let target;
      if (q.targetKind === "army") target = ids.get(q.target);
      else if (q.targetKind === "armytype") target = q.target.id;
      else if (q.targetKind !== "none") target = q.target.index;
      quest = { type: q.type, hero: ids.get(q.hero), done: q.done, required: q.required,
                targetKind: q.targetKind, target };
    }
    return {
      index: s.index, gold: s.gold, alive: s.alive, computer: s.computer,
      level: s.level, enhanced: s.enhanced, observe: s.observe || undefined,
      diploScore: s.diploScore,
      income: s.income, upkeepTotal: s.upkeepTotal, produced: s.produced,
      ai: saveAI(s.ai, ids), aiSolidarity: s.aiSolidarity, card: s.card,
      advisor: s.advisor,
      quest,
    };
  });

  const cities = g.map.cities.map((c) => ({
    index: c.index, name: c.name, ownerIndex: c.ownerIndex, producing: c.producing,
    countdown: c.countdown, vectorTo: c.vectorTo, razed: c.razed || undefined,
    defence: c.defence, income: c.income, claim: c.claim,
    razedBy: c.razedBy, slots: c.slots,
  }));

  const sites = g.map.sites.map((s) => ({
    index: s.index, content: s.content, item: s.item, guardian: s.guardian,
    allyType: s.allyType, rich: s.rich || undefined, revealed: s.revealed,
    searched: s.searched || undefined, band: s.band, templeIndex: s.templeIndex,
  }));

  const items = g.map.items.map((it) => ({
    index: it.index, name: it.name, type: it.type, value: it.value,
    status: it.status, x: it.x, y: it.y, planted: it.planted,
  }));

  const signs = (g.map.signs || []).map((sg) => [sg.lines[0], sg.lines[1]]);
  const towers = Object.keys(g.towerAt || {}).map(Number).sort((a, b) => a - b);
  const explored = {};
  for (const k in g.explored || {}) explored[k] = packMask(g.explored[k]);

  return JSON.stringify({
    version: VERSION,
    scenario: g.map.name,
    turn: g.turn, current: g.current, seed: g.rng.state,
    won: g.won || undefined, over: g.over || undefined, noHumansSaid: g.noHumansSaid || undefined,
    greatest: g.greatest || undefined,
    surrenderOffered: g.surrenderOffered || undefined,
    options: g.map.options,
    diplomacy: g.diplomacy,
    sides, cities, sites, items, armies,
    signs, fightOrder: g.map.fightOrder, towers, explored,
    history: g.history, deeds: g.deeds, triumphs: g.triumphs,
    tutorialSeen: g.tutorialSeen,
    log: g.log,
    randomWorld: g.map.name === randommap.DIR ? packWorld(g.dataDir) : undefined,
  });
}

// the random world's files, base64 by extension
function packWorld(dataDir) {
  const out = {};
  for (const ext of randommap.FILES) {
    const b = vfs.read(`${dataDir}/${randommap.DIR}/${randommap.DIR}.${ext}`);
    if (b) out[ext] = btoa(latin1(b));
  }
  return out;
}

function unpackWorld(packed) {
  const files = {};
  for (const ext in packed) files[ext] = bytesOf(atob(packed[ext]));
  return files;
}

const bool = (v) => !!v;

/** Rebuild a game from a saved string. */
export function decode(text, dataDir) {
  const state = JSON.parse(text);
  if (state.version !== VERSION) throw new Error("unsupported save version");
  if (state.randomWorld) randommap.install(dataDir, unpackWorld(state.randomWorld));

  const g = game.newGame(dataDir, state.scenario, { seed: 0, options: state.options });
  g.armies = [];
  g.turn = state.turn; g.current = state.current;
  g.rng.state = state.seed;
  g.won = state.won; g.over = state.over; g.surrenderOffered = state.surrenderOffered;
  g.noHumansSaid = state.noHumansSaid;
  g.greatest = state.greatest;
  g.diplomacy = state.diplomacy;
  g.log = state.log || [];

  const itemByIndex = {};
  for (const saved of state.items) {
    for (const it of g.map.items) {
      if (it.index === saved.index) {
        it.name = saved.name; it.type = saved.type; it.value = saved.value;
        it.status = saved.status; it.x = saved.x ?? undefined; it.y = saved.y ?? undefined;
        it.planted = saved.planted;
        itemByIndex[it.index] = it;
      }
    }
  }

  for (const saved of state.cities) {
    const c = g.map.cities[saved.index];
    c.ownerIndex = saved.ownerIndex ?? undefined;
    c.producing = saved.producing ?? undefined;
    c.countdown = saved.countdown;
    c.vectorTo = saved.vectorTo ?? undefined;
    c.razed = bool(saved.razed);
    c.defence = saved.defence;
    c.income = saved.income; c.slots = saved.slots;
    c.claim = saved.claim ?? c.claim;
    c.razedBy = saved.razedBy ?? undefined;
    c.name = saved.name || c.name;
    // ruins belong to nobody; older saves can have them won in a fight
    if (c.razed) c.ownerIndex = undefined;
  }
  scn.refreshCityTiles(g.map);

  for (const saved of state.sites) {
    const s = g.map.sites[saved.index];
    s.content = saved.content; s.item = saved.item ?? undefined; s.guardian = saved.guardian ?? undefined;
    s.allyType = saved.allyType ?? undefined; s.rich = bool(saved.rich); s.revealed = saved.revealed;
    s.searched = bool(saved.searched); s.band = saved.band;
    s.templeIndex = saved.templeIndex ?? undefined;
  }
  if (state.fightOrder) g.map.fightOrder = state.fightOrder;
  g.history = state.history; g.deeds = state.deeds; g.triumphs = state.triumphs;
  g.tutorialSeen = state.tutorialSeen;
  g.towerAt = {};
  for (const k of state.towers || []) g.towerAt[k] = true;
  (state.signs || []).forEach((saved, i) => {
    const sg = g.map.signs[i];
    if (sg) sg.lines = [saved[0], saved[1]];
  });
  if (state.explored) {
    g.explored = {};
    for (const k in state.explored) {
      g.explored[k] = unpackMask(state.explored[k], g.map.width * g.map.height);
    }
  }

  g.map.siteAt = {};
  for (const s of g.map.sites) g.map.siteAt[s.y * g.map.width + s.x] = s;

  const armies = state.armies.map((saved) => {
    const a = {};
    for (const k in saved) if (saved[k] !== null) a[k] = saved[k];
    a.atSea = bool(saved.atSea); a.returning = bool(saved.returning);
    a.items = undefined;
    if (saved.items) a.items = saved.items.map((idx) => itemByIndex[idx]);
    return a;
  });
  g.armies = armies.slice();

  for (const saved of state.sides) {
    const s = g.map.sides[saved.index];
    s.gold = saved.gold; s.alive = saved.alive; s.computer = saved.computer;
    s.level = saved.level; s.enhanced = saved.enhanced; s.diploScore = saved.diploScore;
    s.observe = saved.observe || false;
    s.income = saved.income; s.upkeepTotal = saved.upkeepTotal;
    s.produced = saved.produced;
    if (saved.ai && saved.ai.groups) {
      s.ai = saved.ai;
      s.ai.groups = s.ai.groups.map((grp) => {
        if (!grp) return undefined;
        const staged = {};
        for (let k = 1; k <= 4; k++) if (grp.staged[k] != null) staged[k] = armies[grp.staged[k]];
        grp.staged = staged;
        grp.target = grp.target ?? undefined;
        grp.rally = grp.rally ?? undefined;
        return grp;
      });
      s.ai.questCity = s.ai.questCity ?? undefined;
    }
    s.aiSolidarity = saved.aiSolidarity; s.card = saved.card;
    s.advisor = saved.advisor;
    s.quest = undefined;
    if (saved.quest) {
      const q = { type: saved.quest.type, done: saved.quest.done,
                  required: saved.quest.required ?? undefined, hero: armies[saved.quest.hero] };
      const kind = saved.quest.targetKind, ref = saved.quest.target;
      q.targetKind = kind;
      if (kind === "city") q.target = g.map.cities[ref];
      else if (kind === "side") q.target = g.map.sides[ref];
      else if (kind === "item") q.target = itemByIndex[ref];
      else if (kind === "armytype") q.target = g.types.byId[ref];
      else if (kind === "army") q.target = armies[ref];
      else if (kind === "none") q.target = true;
      if (q.hero && q.target) s.quest = q;
    }
  }

  // saves made before the walk kept to 1a8b:04c8 could put a flier at sea,
  // and a hero flying with one: nothing the original can reach, so undone
  const fliers = new Set(), walkers = new Set();
  for (const a of g.armies) {
    if (a.x != null) {
      const k = a.y * g.map.width + a.x;
      if (g.types.byId[a.type].flies) fliers.add(k);
      else if (a.type !== 28) walkers.add(k);
    }
  }
  for (const a of g.armies) {
    if (a.atSea && a.x != null) {
      const k = a.y * g.map.width + a.x;
      if (g.types.byId[a.type].flies || (a.type === 28 && fliers.has(k) && !walkers.has(k))) a.atSea = false;
    }
  }

  g.side = g.sides[g.current];
  move.invalidate(g);
  return g;
}
