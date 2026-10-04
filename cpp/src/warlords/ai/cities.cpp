#include "warlords/ai/cities.hpp"

#include <algorithm>
#include <set>

#include "util/util.hpp"
#include "warlords/ai/core.hpp"
#include "warlords/ai/groups.hpp"
#include "warlords/ai/moves.hpp"
#include "warlords/game.hpp"
#include "warlords/rules.hpp"

namespace w2::ai::cities {

using namespace core;

// garrison wanted on the first tile, by how many armies the city holds
// (DS:0824), 8 from thirteen up
static const int KEEP[13] = {0, 0, 0, 0, 3, 3, 4, 5, 5, 6, 7, 7, 7};

// the city's four tiles in the order the garrison fills them (DS:0814/081c)
static const int TILE_DX[4] = {1, 0, 0, 1};
static const int TILE_DY[4] = {0, 0, 1, 1};

// the roles that make a city try a quick attack (DS:0932)
static const std::set<int> QUICK_ROLES = {5, 8, 6, 4, 14, 7};

std::vector<City*> own(const Game& g, const Side& side) {
  std::vector<City*> out;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == side.index) out.push_back(&c);
  }
  return out;
}

std::vector<NeutralNeighbour> neutralNeighbours(Game& g, Side& side, const City& c) {
  AIData& d = data(g, side);
  auto [nb, nd] = neighbours(g, c);
  std::vector<NeutralNeighbour> out;
  for (size_t j = 0; j < nb.size(); j++) {
    City* n = nb[j];
    if (n->index != d.questCity && standing(*n) && n->ownerIndex == NONE && (out.empty() || j < 2 || nd[j] < 20)) {
      out.push_back(NeutralNeighbour{n, nd[j], (int)j + 1});
    }
  }
  return out;
}

int nearestArmy(const Game& g, const Side& side, int x, int y, int of) {
  int best = 1000;
  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    Army* a = g.armies[i];
    if (a->owner != NONE && !a->transit) {
      bool ok = of == NONE ? a->owner != side.index : a->owner == of;
      if (ok && explored(g, side.index, a->x, a->y)) {
        int dd = dist(x, y, a->x, a->y);
        if (dd < best) best = dd;
      }
    }
  }
  return best;
}

static int slotStrength(const Game& g, const Slot& slot) {
  int s = slot.strength;
  if (g.types.byId(slot.type)->siege) s += 2;
  return s;
}

std::pair<bool, int> buildsWell(Game& g, Side& side, const City& c) {
  AIData& d = data(g, side);
  int slot = rules::bestSlot(c.slots, 3, g.types, side.enhanced);
  if (slot == NONE) return {false, NONE};
  int s = slotStrength(g, c.slots[slot]);
  if ((d.own < 12 || s > 2) && (d.own < 8 || s > 1)) return {true, slot};
  return {false, slot};
}

// The slot to buy over (623c:1ae9), from 0: the first empty one, else the
// weakest under 10. NONE when all four are full of strong types.
static int slotToReplace(const Game& g, const City& c) {
  if ((int)c.slots.size() < rules::PRODUCTION_SLOTS) return (int)c.slots.size();
  int best = NONE, least = 10;
  for (size_t i = 0; i < c.slots.size(); i++) {
    int s = slotStrength(g, c.slots[i]);
    if (s < least) { best = (int)i; least = s; }
  }
  return best;
}

bool building(const City& c) {
  if (c.producing == NONE) return false;
  if (c.producing >= (int)c.slots.size()) return false;
  const Slot& slot = c.slots[c.producing];
  return c.countdown != 0 && c.countdown != slot.time;
}

// set_city_production (623c:0e5a): refused after turn 5 when the side has
// less than the type's cost + 30 in gold.
static bool setProduction(Game& g, Side& side, City& c, int slot) {
  if (slot < 0 || slot >= (int)c.slots.size()) return false;
  if (side.gold < c.slots[slot].cost + 30 && g.turn > 5) return false;
  game::setProduction(g, c, slot);
  return true;
}

static void produceFor(Game& g, Side& side, City& c, int purpose) {
  int slot = rules::bestSlot(c.slots, purpose, g.types, side.enhanced);
  if (slot != NONE) setProduction(g, side, c, slot);
}

void vector(Game& g, City& c, const City* dest) {
  if (!building(c)) c.vectorTo = NONE;
  else c.vectorTo = dest ? dest->index : NONE;
}

// ai_buy_production_type (5db9:0bd1), if the side has 30 over its price.
static bool buyType(Game& g, Side& side, City& c, int typeId) {
  const ArmyType* t = g.types.byId(typeId);
  if (!t) return false;
  if (!(side.gold > t->price + 30)) return false;
  int n = slotToReplace(g, c);
  if (n == NONE) return false;
  game::buyProduction(g, side, c, n, typeId);
  game::sortProduction(c);
  AIData& d = data(g, side);
  d.bought++;
  c.producing = NONE;
  c.countdown = 0;
  c.vectorTo = NONE;
  setRole(d, c, BUILDING);
  return true;
}

// Buy the first flying type the side can afford into a city (623c:11d5).
static void buyFlier(Game& g, Side& side, City& c) {
  int n = slotToReplace(g, c);
  if (n == NONE) return;
  AIData& d = data(g, side);
  for (int id = 0; id <= 28; id++) {
    const ArmyType* t = g.types.byId(id);
    if (t && t->flies && t->bonus[48] == 0 && t->price + 30 <= side.gold) {
      game::buyProduction(g, side, c, n, id);
      game::sortProduction(c);
      setCflag(d, c, CF_FLIER);
      c.producing = NONE;
      c.countdown = 0;
      c.vectorTo = NONE;
      return;
    }
  }
}

static void markFliers(Game& g, AIData& d) {
  for (auto& c : g.map->cities) {
    if (standing(c)) {
      clearCflag(d, c, CF_FLIER);
      for (auto& slot : c.slots) {
        if (g.types.byId(slot.type)->flies) setCflag(d, c, CF_FLIER);
      }
    }
  }
}

void clearUnseen(Game& g, Side& side) {
  if (g.map->options.hiddenMap == 0) return;
  AIData& d = data(g, side);
  for (auto& c : g.map->cities) {
    if (cflag(d, c, CF_UNSEEN)) {
      bool seen = false;
      for (int x = c.x - 1; x <= c.x + 2; x++) {
        for (int y = c.y - 1; y <= c.y + 2; y++) {
          if (x >= 0 && y >= 0 && x < g.map->width && y < g.map->height && game::seen(g, side.index, x, y)) seen = true;
        }
      }
      if (seen) clearCflag(d, c, CF_UNSEEN);
    }
  }
}

void evaluate(Game& g, Side& side, int who) {
  AIData& d = data(g, side);
  markFliers(g, d);
  clearUnseen(g, side);
  d.own = 0; d.enemy = 0; d.neutral = 0; d.unseen = 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (standing(c)) {
      if (c.ownerIndex == who) {
        d.own++;
        d.held[c.index] = get(d.held, c.index) + 1;
        if (role(d, c) == 0) setRole(d, c, JUST_TAKEN);
      } else {
        if (cflag(d, c, CF_UNSEEN)) d.unseen++;
        setRole(d, c, 0);
        d.held[c.index] = 0;
        if (c.ownerIndex == NONE) d.neutral++;
        else d.enemy++;
      }
    } else {
      setRole(d, c, 0);
      d.held[c.index] = 0;
    }
  }
  if (d.own != 0) {
    for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
      City& c = g.map->cities[i];
      if (role(d, c) == STOP && d.unseen != 0 && d.explorers < 5 && cflag(d, c, CF_FLIER)) {
        setRole(d, c, EXPLORER2);
      }
      if (role(d, c) == JUST_TAKEN) {
        setRole(d, c, !neutralNeighbours(g, side, c).empty() ? NEAR_NEUTRAL : BUILDING);
      }
    }
    d.turns++;
  }
}

int wanted(Game& g, const Side& side, const City& c) {
  auto nb = neighbours(g, c).first;
  int foreign = 0, war = 0;
  for (int j = (int)nb.size() - 1; j >= 0; j--) {
    City* n = nb[j];
    if (n->ownerIndex != NONE && n->ownerIndex != side.index && standing(*n)) {
      foreign++;
      if (state(g, side.index, n->ownerIndex) == 2) war++;
    }
  }
  if (war != 0) return 8;
  if (foreign >= 2) return 4;
  if (foreign == 1) return 3;
  return 2;
}

// Put the city's armies in the order they stand in (5ca7:0b5c): the ones to
// keep on the first tile first. {list, keep}.
static std::pair<std::vector<Army*>, int> arrange(Game& g, Side& side, const City& c, const std::vector<Army*>& list) {
  AIData& d = data(g, side);
  int n = (int)list.size();
  int keepWanted = n < 13 ? KEEP[n] : 8;
  int mask = 0, minMax = 0, keep = 0;
  bool attack = false;
  std::vector<Army*> pool = list;
  if (role(d, c) == RALLY) {
    int fliers = 0, heroes = 0;
    for (int i = n - 1; i >= 0; i--) {
      Army* a = pool[i];
      if (isHero(a)) heroes++;
      else if (flies(g, a) && (a->strength > 3 || fliers == 0)) fliers++;
    }
    if (fliers != 0 && heroes != 0) fliers += heroes;
    if ((fliers > 3 && n < 17) || (fliers > 2 && n < 9)) attack = true;
    for (int gi = MAX_GROUPS; gi >= 1; gi--) {
      const Group& grp = *d.groups[gi];
      if (grp.active != 0 && grp.rally == c.index) {
        if (has(grp.flags, GF_MOVE12)) minMax = 12;
        else if (has(grp.flags, GF_MOVE16)) minMax = 16;
        else minMax = grp.size;
        if (has(grp.flags, GF_FLY)) attack = true;
      }
    }
  }
  int k = 3 - get(d.held, c.index) / 3;
  if (k > 0 && k < n && n - keepWanted < k) keepWanted = n - k;

  std::vector<Army*> out;
  for (;;) {
    int bestKeep = -1, bestKeepI = NONE, bestOther = -1, bestOtherI = NONE;
    for (int i = n - 1; i >= 0; i--) {
      Army* a = pool[i];
      if (a) {
        int score = a->strength;
        if ((int)out.size() < keepWanted) {
          int ab = ability(g, a);
          if (!has(mask, 1) && isHero(a)) {
            score += 1000;
          } else if (minMax <= a->maxMoves) {
            if (!has(mask, 0x10) && flies(g, a)) score += 900;
            else if (!attack) {
              if (!has(mask, 2) && ab == 2) score += 800;
              else if (!has(mask, 4) && ab == 3) score += 700;
              else if (!has(mask, 0x80) && magical(g, a)) score += 600;
              else if (!has(mask, 8) && ab == 1) score += 500;
              else if (!has(mask, 0x20) && woods(g, a)) score += 400;
              else if (!has(mask, 0x40) && hills(g, a)) score += 300;
            }
          }
        } else {
          mask = 0xfff;
        }
        if (has(mask, 1) && isHero(a)) score = 1;
        if (has(mask, 0x80) && magical(g, a)) score = 2;
        if (a->maxMoves < minMax || (attack && !flies(g, a) && !isHero(a))) {
          if (bestOther < score) { bestOther = score; bestOtherI = i; }
        } else if (bestKeep < score) {
          bestKeep = score;
          bestKeepI = i;
        }
      }
    }
    if (bestKeepI == NONE && bestOtherI == NONE) break;
    if (mask != 0xfff) {
      Army* a = pool[bestKeepI != NONE ? bestKeepI : bestOtherI];
      int ab = ability(g, a);
      if (isHero(a)) mask = set(mask, 1);
      else {
        if (ab == 2) mask = set(mask, 2);
        if (ab == 3) mask = set(mask, 4);
        if (flies(g, a)) mask = set(mask, 0x10);
        if (magical(g, a)) mask = set(mask, 0x80);
        if (ab == 1) mask = set(mask, 8);
        if (woods(g, a)) mask = set(mask, 0x20);
        if (hills(g, a)) mask = set(mask, 0x40);
      }
    }
    if (bestKeepI == NONE) {
      out.push_back(pool[bestOtherI]);
      pool[bestOtherI] = nullptr;
    } else {
      out.push_back(pool[bestKeepI]);
      pool[bestKeepI] = nullptr;
      keep++;
    }
  }
  if (keepWanted < keep) keep = keepWanted;
  return {out, keep};
}

static void disband(Game& g, Side& side, Army* a) { game::disband(g, side, {a}); }

// Hand items round between a city's heroes (6087:17db, 1a3d).
static void shareItems(Game& g, AIData& d, const std::vector<Army*>& heroes) {
  auto count = [](Army* h, const std::set<int>& types) {
    int n = 0;
    for (Item* it : h->items) if (types.count(it->type)) n++;
    return n;
  };
  auto give = [&](Army* from, Army* to, const std::set<int>& types) {
    auto& items = from->items;
    for (size_t i = 0; i < items.size(); i++) {
      Item* it = items[i];
      if (types.count(it->type)) {
        items.erase(items.begin() + i);
        to->items.push_back(it);
        d.itemsPassed++;
        return true;
      }
    }
    return false;
  };
  for (const std::set<int>& t : {std::set<int>{6}, std::set<int>{5}, std::set<int>{7}}) {
    bool moved = true;
    while (moved) {
      moved = false;
      for (Army* h : heroes) {
        if (count(h, t) > 1) {
          for (Army* o : heroes) {
            if (o != h && count(o, t) == 0 && count(h, t) > 1) {
              if (give(h, o, t)) moved = true;
            }
          }
        }
      }
    }
  }
  // battle, command and standard items: each hero passes on only what it
  // started with, so nothing goes back and forth
  std::set<int> fightT = {1, 2, 8};
  std::vector<std::vector<Item*>> start(heroes.size());
  std::vector<int> ownN(heroes.size());
  for (size_t i = 0; i < heroes.size(); i++) {
    for (Item* it : heroes[i]->items) if (fightT.count(it->type)) start[i].push_back(it);
    ownN[i] = (int)start[i].size();
  }
  bool moved = true;
  while (moved) {
    moved = false;
    for (size_t i = 0; i < heroes.size(); i++) {
      Army* h = heroes[i];
      if (!start[i].empty()) {
        int least = ownN[i], to = NONE;
        for (size_t j = 0; j < heroes.size(); j++) {
          if (j != i && ownN[j] < least) { least = ownN[j]; to = (int)j; }
        }
        if (to != NONE) {
          Item* it = start[i].front();
          start[i].erase(start[i].begin());
          auto k = std::find(h->items.begin(), h->items.end(), it);
          if (k != h->items.end()) h->items.erase(k);
          heroes[to]->items.push_back(it);
          d.itemsPassed++;
          ownN[i]--;
          ownN[to]++;
          moved = true;
        }
      }
    }
  }
}

bool garrison(Game& g, Side& side, City& c, bool force) {
  AIData& d = data(g, side);
  int cx = c.x, cy = c.y;
  int last = get(d.garrison, c.index);
  setCflag(d, c, CF_CLEANED);
  d.garrison[c.index] = 0;
  std::vector<Army*> list, heroes;
  int ordered = 0, fliers = 0, magic = 0;
  Army* lastHero = nullptr;
  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    if (i >= (int)g.armies.size()) continue;
    Army* a = g.armies[i];
    if (a && !a->transit && a->x != NONE && a->x >= cx && a->x <= cx + 1 && a->y >= cy && a->y <= cy + 1) {
      if (a->owner != side.index) {
        Side* s = g.map->side(a->owner == NONE ? 8 : a->owner);
        disband(g, s ? *s : side, a);
      } else {
        d.garrison[c.index]++;
        if (a->aiOrder == ORDER_CITY && a->aiDest == c.index) clearOrder(a);
        if (a->aiOrder != 0) ordered++;
        if (isHero(a) && heroes.size() < 8) { heroes.push_back(a); lastHero = a; }
        if (flies(g, a)) fliers++;
        if (magical(g, a)) magic++;
        if (list.size() < 32) list.push_back(a);
        else disband(g, side, a);
      }
    }
  }
  if (lastHero) {
    Sel s;
    s.hero = lastHero;
    s.armies = {lastHero};
    pickUp(g, s);
  }
  if (heroes.size() > 1) shareItems(g, d, heroes);
  clearCflag(d, c, CF_HERO);
  clearCflag(d, c, CF_MAGIC);
  if (!heroes.empty()) setCflag(d, c, CF_HERO);
  if (magic != 0) setCflag(d, c, CF_MAGIC);
  if (list.size() > 16) { last = 0; ordered = 0; }

  int rl = role(d, c);
  if (!((ordered < 3 || rl == RALLY) &&
        (force || !heroes.empty() || get(d.garrison, c.index) != last || g.rng.dice(1, 6, -1) == 0 ||
         !game::armiesAt(g, cx + 1, cy + 1).empty()))) {
    return true;
  }
  d.keep[c.index] = 0;
  if (rl == WEAK || rl == BUILDING || rl == STOP) {
    int want = wanted(g, side, c);
    if (list.size() < 2) setRole(d, c, WEAK);
    else if ((int)list.size() < want) setRole(d, c, BUILDING);
    else setRole(d, c, STOP);
  }
  auto [sorted, keepN] = arrange(g, side, c, list);
  int n = (int)sorted.size();
  if (!force && side.gold < 300 && side.income < side.upkeepTotal * 2) {
    if (role(d, c) != RALLY && get(d.held, c.index) > 10 && (int)heroes.size() + magic + 4 < n &&
        nearestArmy(g, side, cx, cy) > 10) {
      for (int i = (int)heroes.size() + magic + 4; i < n; i++) {
        if (sorted[i]) { disband(g, side, sorted[i]); sorted[i] = nullptr; }
      }
    }
  }
  if (n > 24) {
    for (int i = 24; i < n; i++) {
      if (sorted[i]) { disband(g, side, sorted[i]); sorted[i] = nullptr; }
    }
  }
  if ((g.map->options.quickStart == 0 || g.turn > 2) && n < 2) setRole(d, c, WEAK);
  if (g.map->options.hiddenMap != 0 && role(d, c) == EXPLORER2 && d.explorers < 5) {
    int go = fliers;
    int shortBy = 2 - (n - fliers);
    if (shortBy > 0) go = fliers - shortBy;
    if (g.map->options.quickStart != 0 && g.turn < 3) go = 1;
    for (int i = 0; i < n; i++) {
      Army* a = sorted[i];
      if (go > 0 && a && alive(g, a) && flies(g, a)) {
        d.explorers++;
        a->aiExplore = true;
        moves::explore(g, side, a, nullptr);
        sorted[i] = nullptr;
        go--;
      }
    }
  }
  int tile, perTile;
  int r = role(d, c);
  if (r == NEAR_NEUTRAL || r == TAKING_NEUTRAL) {
    tile = 1;
    perTile = 8;
  } else {
    tile = keepN == 0 ? 1 : 0;
    perTile = keepN == 0 ? 8 : keepN;
  }
  for (int i = 0; i < n; i++) {
    Army* a = sorted[i];
    if (a && alive(g, a) && tile <= 3) {
      if (tile == 0) d.keep[c.index] = get(d.keep, c.index) + 1;
      a->x = cx + TILE_DX[tile];
      a->y = cy + TILE_DY[tile];
      a->atSea = false;
      a->target.reset();
      a->group = 0;
      a->aiGroup = 0;
      a->done = false;
      perTile--;
      if (perTile == 0) { tile++; perTile = 8; }
    }
  }
  return true;
}

void clean(Game& g, Side& side) {
  AIData& d = data(g, side);
  for (City* c : own(g, side)) {
    if (!cflag(d, *c, CF_CLEANED)) garrison(g, side, *c, false);
  }
}

// Send stacks from a city at the neutral cities round it (57ea:06c7).
static void takeNeutrals(Game& g, Side& side, City& c, int count, std::map<int, int>& heading, std::vector<Army*> stack) {
  AIData& d = data(g, side);
  bool neutralOn = g.map->options.neutralCities != 0;
  int oddsV = 100;
  City* from = &c;
  City* lastFrom = nullptr;
  City* lastPick = nullptr;
  for (;;) {
    std::vector<Army*> keep;
    for (Army* a : stack) if (alive(g, a) && a->owner == side.index) keep.push_back(a);
    stack = keep;
    if (stack.empty()) return;
    Sel sel = select(g, stack);
    auto nb = neutralNeighbours(g, side, *from);
    if (nb.empty()) return;
    City* best = nullptr;
    int bestScore = 1000;
    for (int j = (int)nb.size() - 1; j >= 0; j--) {
      const NeutralNeighbour& e = nb[j];
      City* n = e.city;
      if (!cflag(d, *n, CF_UNSEEN) && get(heading, n->index) < 3) {
        if (neutralOn) oddsV = odds(g, sel, n->x, n->y);
        if (oddsV > 74) {
          int score = g.rng.dice(1, 10, 0) + e.dist + get(heading, n->index) * 10 + (e.dist >= 41 ? 10 : 0) +
                      (e.dist >= 51 ? 30 : 0) + (100 - oddsV);
          if (score < bestScore) { best = n; bestScore = score; }
        }
      }
    }
    if (!best) break;
    heading[best->index] = get(heading, best->index) + count;
    lastFrom = &c;
    lastPick = best;
    auto s = order(g, stack, ORDER_CITY, best->index, 0x80);
    if (s) moveTo(g, *s, s->leader->target->first, s->leader->target->second);
    if (best->ownerIndex != side.index) return;
    if (d.cautious != 0) return;
    from = best;
    if (nearestArmy(g, side, best->x, best->y) < 10) return;
  }
  if (g.map->options.hiddenMap != 0 && lastFrom == &c && lastPick) {
    order(g, stack, ORDER_CITY, lastPick->index, 0x80);
  }
}

// How many armies a city holds back (57ea:03aa): {extra, lost, near}.
static std::tuple<int, int, int> holdBack(Game& g, Side& side, const City& c) {
  AIData& d = data(g, side);
  int near = nearestArmy(g, side, c.x, c.y);
  int extra = (near < 5 ? 2 : 0) + (near < 15 ? 1 : 0) + (d.cautious != 0 ? 1 : 0);
  int lost = 0;
  for (int i = 0; i < 8; i++) lost += d.citiesLost[i];
  return {extra, std::min(2, lost), near};
}

// The idle armies on a city's corner tile, beyond the ones held back, the
// fastest first.
template <class Test>
static std::vector<Army*> idle(Game& g, Side& side, const City& c, int reserve, Test test) {
  std::vector<Army*> found;
  for (Army* a : onTile(g, side.index, c.x, c.y)) {
    if (a->aiGroup == 0 && !a->done && a->aiOrder == 0 && test(a)) {
      if (reserve > 0) reserve--;
      else if (found.size() < 8) found.push_back(a);
    }
  }
  luaSort(found, [](Army* p, Army* q) { return p->maxMoves > q->maxMoves; });
  return found;
}

// Send a city's idle armies at its neutral neighbours (57ea:03aa). True with
// the map hidden (the city then sends explorers).
static bool attackNeutrals(Game& g, Side& side, City& c, std::map<int, int>& heading) {
  auto [extra, lost, near] = holdBack(g, side, c);
  (void)near;
  auto list = idle(g, side, c, extra + lost, [](Army*) { return true; });
  if (list.empty()) return false;
  size_t per = g.map->options.neutralCities != 0 ? 8 : 1;
  size_t i = 0;
  for (;;) {
    std::vector<Army*> stack;
    while (i < list.size() && stack.size() < per) { stack.push_back(list[i]); i++; }
    if (stack.empty()) break;
    if (extra + lost == 0 && g.map->options.hiddenMap != 0 && nearestArmy(g, side, c.x, c.y) < 15) break;
    takeNeutrals(g, side, c, (int)stack.size(), heading, stack);
  }
  return g.map->options.hiddenMap != 0;
}

// Send a city's idle armies to explore, one by one (57ea:0b19).
static void sendExplorers(Game& g, Side& side, City& c, bool fliersOnly) {
  AIData& d = data(g, side);
  bool grouping = false;
  for (int i = 1; i <= MAX_GROUPS; i++) if (d.groups[i]->active != 0) grouping = true;
  auto [extra, lost, near] = holdBack(g, side, c);
  (void)near;
  auto list = idle(g, side, c, extra + lost, [&](Army* a) {
    if (isHero(a) && grouping) return false;
    if (fliersOnly && !flies(g, a)) return false;
    return true;
  });
  for (Army* a : list) {
    if (extra + lost == 0 && g.map->options.hiddenMap != 0 && nearestArmy(g, side, c.x, c.y) < 15) return;
    if (alive(g, a)) moves::explore(g, side, a, &c);
  }
}

City* openNeutral(Game& g, Side& side, const City& c, std::map<int, int>& heading) {
  auto nb = neutralNeighbours(g, side, c);
  for (int j = (int)nb.size() - 1; j >= 0; j--) {
    City* n = nb[j].city;
    if (get(heading, n->index) <= 2) return n;
  }
  return nullptr;
}

// One pass of the neutral phase (57ea:00b5).
static int neutralPass(Game& g, Side& side, int pass, std::map<int, int>& heading) {
  AIData& d = data(g, side);
  int acted = 0;
  for (City* c : own(g, side)) {
    if (c->ownerIndex == side.index && (pass == 0 || role(d, *c) == JUST_TAKEN)) {
      if (role(d, *c) == JUST_TAKEN) {
        garrison(g, side, *c, true);
        setRole(d, *c, NEAR_NEUTRAL);
      }
      bool act = !neutralNeighbours(g, side, *c).empty();
      if (!act) {
        int r = role(d, *c);
        if (r == NEAR_NEUTRAL || r == TAKING_NEUTRAL) setRole(d, *c, WEAK);
        if (g.map->options.quickStart == 0 && role(d, *c) == BUILDING && g.turn < 6) act = true;
      }
      if (act) {
        acted++;
        if (!attackNeutrals(g, side, *c, heading)) {
          setRole(d, *c, openNeutral(g, side, *c, heading) ? NEAR_NEUTRAL : WEAK);
        } else {
          sendExplorers(g, side, *c, g.turn > 10);
          setRole(d, *c, TAKING_NEUTRAL);
        }
      }
    }
  }
  return acted;
}

void neutral(Game& g, Side& side) {
  std::map<int, int> heading;
  for (Army* a : armies(g, side.index)) {
    if (a->aiOrder == ORDER_CITY && a->aiDest != NONE && a->aiDest < (int)g.map->cities.size()) {
      heading[a->aiDest] = get(heading, a->aiDest) + 1;
    }
  }
  for (int pass = 0; pass <= 9; pass++) {
    if (neutralPass(g, side, pass, heading) == 0) break;
  }
}

// A strong garrison strikes at a weak neighbour (5e97:053f).
static void quickStrike(Game& g, Side& side, City& c) {
  AIData& d = data(g, side);
  int rl = role(d, c);
  int x = c.x;
  int y = c.y;
  if (rl != RALLY) x++;
  if (!(rl != RALLY || get(d.garrison, c.index) > 11)) return;
  int need = (side.gold < 40 && side.income < side.upkeepTotal) ? 65 : 85;
  auto list = collect(g, side.index, x, y, 8);
  int n = (int)list.size();
  if (n == 0) return;
  if (n < 4 && (rl == MEMBER || rl == RALLY)) return;
  Sel sel = select(g, list);
  auto [nb, nd] = neighbours(g, c);
  bool danger = false;
  City* best = nullptr;
  int bestOdds = 0, bestTurns = 100;
  for (int j = (int)nb.size() - 1; j >= 0; j--) {
    City* t = nb[j];
    if (!cflag(d, *t, CF_UNSEEN) && t->ownerIndex != side.index && standing(*t) && t->index != d.questCity &&
        (t->ownerIndex == NONE || state(g, side.index, t->ownerIndex) == 2)) {
      int o = odds(g, sel, t->x, t->y);
      if (o == 0) {
        if ((n < 4 && nd[j] < 25) || (n < 6 && nd[j] < 15)) danger = true;
      } else if (o >= need) {
        int turns = nd[j] / std::max(1, sel.minMoves - 2) + 1;
        if (turns < bestTurns || (turns == bestTurns && bestOdds < o)) {
          best = t;
          bestOdds = o;
          bestTurns = turns;
        }
      }
    }
  }
  if (!danger && best) {
    auto s = order(g, list, ORDER_CITY, best->index, 0);
    if (s) moveTo(g, *s, s->leader->target->first, s->leader->target->second);
  }
}

void quickAttack(Game& g, Side& side) {
  AIData& d = data(g, side);
  if (d.cautious != 0) return;
  for (City* c : own(g, side)) {
    if (c->ownerIndex == side.index && QUICK_ROLES.count(role(d, *c)) && get(d.garrison, c->index) > 3) {
      quickStrike(g, side, *c);
    }
  }
}

void updateHide(Game& g, Side& side) {
  AIData& d = data(g, side);
  clearUnseen(g, side);
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (role(d, c) == TAKING_NEUTRAL) {
      for (auto& e : neutralNeighbours(g, side, c)) {
        if (!cflag(d, *e.city, CF_UNSEEN)) {
          setRole(d, c, NEAR_NEUTRAL);
          break;
        }
      }
    }
  }
}

// The city that should buy a new type (ai_city_for_type, 5db9:0c83).
static City* cityForType(Game& g, Side& side) {
  AIData& d = data(g, side);
  City* best = nullptr;
  int bestScore = -1;
  for (City* c : own(g, side)) {
    int r = role(d, *c);
    bool candidate = false;
    int younger = 0;
    if (r == TAKING_NEUTRAL || r == NEAR_NEUTRAL) {
      if (get(d.held, c->index) > 3) {
        younger = 4;
        candidate = !buildsWell(g, side, *c).first;
      }
    } else if (r == WEAK || r == BUILDING) {
      candidate = !buildsWell(g, side, *c).first;
    } else if (r == STOP) {
      if (!buildsWell(g, side, *c).first) {
        setRole(d, *c, NOWHERE);
        candidate = true;
      }
    } else if (r == NOWHERE) {
      if (!buildsWell(g, side, *c).first) candidate = true;
      else setRole(d, *c, STOP);
    }
    if (candidate) {
      int score = get(d.held, c->index) - younger;
      if (bestScore < score) { best = c; bestScore = score; }
    }
  }
  return best;
}

void rebuild(Game& g, Side& side) {
  AIData& d = data(g, side);
  int limit = std::max(5, (int)floorDiv((long)d.rebuildLimit * (long)g.map->cities.size(), 80));
  if ((d.heroes > 4 || d.own <= limit * d.heroes) && side.gold > 499) {
    int t = side.gold < 2001 ? d.rebuildType : d.rebuildTypeRich;
    City* c = cityForType(g, side);
    if (c && side.gold > 499) buyType(g, side, *c, t);
  }
}

// The member cities of an assault group (5f19:0b72).
static void pickMembers(Game& g, Side& side, Group& grp) {
  AIData& d = data(g, side);
  int minMax = has(grp.flags, GF_MOVE12) ? 12 : has(grp.flags, GF_MOVE16) ? 16 : grp.size;
  // 1-based, as the group's member slots are
  std::map<int, City*> members;
  std::map<int, int> strength;
  int count = 0;
  for (City* c : own(g, side)) {
    if (groups::free(g, side, *c, false)) {
      int f = get(d.flags, c->index);
      bool fits = (!has(grp.flags, GF_MOVE12) || has(f, CF_MOVE12)) && (!has(grp.flags, GF_MOVE16) || has(f, CF_MOVE16)) &&
                  (!has(grp.flags, GF_FLY) || has(f, CF_FLYGROUP));
      bool plain = has(grp.flags, GF_MOVE16) || has(grp.flags, GF_MOVE12) || has(grp.flags, GF_FLY) ||
                   !(has(f, CF_MOVE12) || has(f, CF_FLYGROUP) || has(f, CF_MOVE16));
      if (fits && plain) {
        auto [good, slotI] = buildsWell(g, side, *c);
        if (good && minMax <= c->slots[slotI].move) {
          const Slot& slot = c->slots[slotI];
          int at = NONE;
          if (count < 4) {
            at = count + 1;
          } else {
            int least = 100;
            for (int k = 1; k <= 4; k++) {
              if (strength.count(k) && strength[k] < least) { least = strength[k]; at = k; }
            }
            if (at != NONE && !(least <= slot.strength)) at = NONE;
          }
          if (at != NONE) {
            if (!members.count(at)) count++;
            members[at] = c;
            strength[at] = slot.strength;
          }
        }
      }
    }
  }
  grp.members.clear();
  for (int k = 1; k <= count; k++) {
    City* c = members[k];
    grp.members[k] = c->index;
    setRole(d, *c, MEMBER);
  }
}

void production(Game& g, Side& side) {
  AIData& d = data(g, side);
  for (City* c : own(g, side)) {
    if (role(d, *c) == MEMBER) {
      setRole(d, *c, STOP);
      c->vectorTo = NONE;
    }
  }
  for (int gi = MAX_GROUPS; gi >= 1; gi--) {
    if (d.groups[gi]->active != 0) pickMembers(g, side, *d.groups[gi]);
  }
  if (g.map->options.hiddenMap != 0) {
    // a city whose neutral neighbours are all unseen goes looking (57ea:02ff)
    for (City* c : own(g, side)) {
      if (role(d, *c) == NEAR_NEUTRAL) {
        auto nb = neutralNeighbours(g, side, *c);
        if (!nb.empty()) {
          bool seen = false;
          for (auto& e : nb) if (!cflag(d, *e.city, CF_UNSEEN)) seen = true;
          if (!seen) setRole(d, *c, TAKING_NEUTRAL);
        }
      }
    }
  }
  for (City* c : own(g, side)) {
    c->vectorTo = NONE;
    if (!building(*c)) {
      if (side.gold < 40 && side.income < side.upkeepTotal) return;
      int r = role(d, *c);
      int purpose = 3;
      if (r == TAKING_NEUTRAL || r == EXPLORER) {
        if (!cflag(d, *c, CF_FLIER)) buyFlier(g, side, *c);
        purpose = 4;
      } else if (r == NEAR_NEUTRAL) {
        purpose = g.map->options.neutralCities != 0 ? 2 : 1;
      } else if (r == WEAK) {
        purpose = 2;
      } else if (r == EXPLORER2) {
        purpose = 4;
      } else if (r == STOP) {
        purpose = NONE;
        c->producing = NONE;
        c->countdown = 0;
      }
      if (purpose != NONE) produceFor(g, side, *c, purpose);
    }
  }
}

void vectoring(Game& g, Side& side) {
  AIData& d = data(g, side);
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    Group& grp = *d.groups[gi];
    City* rally = grp.rally != NONE ? g.map->city(grp.rally) : nullptr;
    if (grp.active != 0 && rally && rally->ownerIndex == side.index) {
      setRole(d, *rally, RALLY);
      for (int k = 4; k >= 1; k--) {
        City* m = grp.members.count(k) ? g.map->city(grp.members[k]) : nullptr;
        if (m && role(d, *m) == MEMBER) vector(g, *m, rally);
      }
    }
  }
}

}  // namespace w2::ai::cities
