#include "warlords/ai/groups.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <set>

#include "util/util.hpp"
#include "warlords/ai.hpp"
#include "warlords/ai/cities.hpp"
#include "warlords/ai/diplomacy.hpp"
#include "warlords/game.hpp"
#include "warlords/move.hpp"

namespace w2::ai::groups {

using namespace core;

// roles a rally city may be picked from (DS:0940, DS:094c)
static const std::set<int> RALLY_ROLES = {5, 8, 6, 4, 14};

static std::vector<City*> own(const Game& g, const Side& side) { return cities::own(g, side); }

static City* city(Game& g, int i) { return i != NONE ? g.map->city(i) : nullptr; }
static City* city(Game& g, const std::map<int, int>& m, int k) {
  auto it = m.find(k);
  return it == m.end() ? nullptr : city(g, it->second);
}

bool free(Game& g, Side& side, const City& c, bool members) {
  AIData& d = data(g, side);
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && grp->rally == c.index) return false;
  }
  int r = role(d, c);
  if (r == STOP || r == BUILDING) return true;
  return members && r == MEMBER;
}

bool isRally(Game& g, Side& side, const City& c) {
  AIData& d = data(g, side);
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && grp->rally == c.index) return true;
  }
  return false;
}

void cancel(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  City* r = city(g, grp->rally);
  if (r) setRole(d, *r, STOP);
  for (int k = 4; k >= 1; k--) {
    City* m = city(g, grp->members, k);
    if (m) setRole(d, *m, STOP);
  }
  d.groups[gi] = emptyGroup();
}

int sackValue(const Game& g, const City& c) {
  int v = 0;
  for (size_t i = 1; i < c.slots.size(); i++) v += std::abs(g.types.byId(c.slots[i].type)->price) / 2;
  return v;
}

void pillage(Game& g, Side& side, City& c) {
  game::pillage(g, side, c, {});
  if (hooks.onSpoils) hooks.onSpoils(g, side, c, "pillaged");
}

void sack(Game& g, Side& side, City& c) {
  if (sackValue(g, c) == 0) return pillage(g, side, c);
  game::sack(g, side, c, {});
  if (hooks.onSpoils) hooks.onSpoils(g, side, c, "sacked");
}

City* raze(Game& g, Side& side, City& c, bool force, Sel* sel) {
  if (!force) {
    auto [nb, nd] = neighbours(g, c);
    int n = 0;
    for (int j = (int)nb.size() - 1; j >= 0; j--) {
      if (nb[j]->ownerIndex == side.index && nd[j] < 45) n++;
    }
    if (n > 2) return nullptr;
  }
  if (sackValue(g, c) >= 400) {
    sack(g, side, c);
    return nullptr;
  }
  game::raze(g, side, c, sel ? sel->armies : std::vector<Army*>{});
  if (hooks.onSpoils) hooks.onSpoils(g, side, c, "razed");
  if (!sel || sel->armies.empty()) return nullptr;
  Flood flood = floodFrom(g, side.index, sel->leader->x, sel->leader->y, 15, sel);
  return bestCity(g, *sel, flood, true);
}

bool earlyVengeance(Game& g, int me, City& c, int was) {
  Side& side = *g.map->side(me);
  AIData& d = data(g, side);
  if (side.computer && was != NONE && was != NEUTRAL && !isComputer(g, was) && d.early != 0 && g.turn < 10 &&
      d.questCity != c.index) {
    if (sackValue(g, c) < 200) raze(g, side, c, false);
    else sack(g, side, c);
    return true;
  }
  return false;
}

// What the group will do to the cities it takes (5f19:06a0).
static void rollSpoils(Game& g, Side& side, Group& grp) {
  AIData& d = data(g, side);
  int bias = d.own * d.perCity;
  if (!side.computer) bias += d.bonusHuman;
  else if (side.level == 2) bias += d.bonusWarlord;
  else if (side.level == 1) bias += d.bonusLord;
  else if (side.level == 0) bias += d.bonusKnight;
  grp.flags = 0;
  if (d.raze != 0 && g.rng.dice(1, 1000, 0) < d.raze + bias) { grp.flags = GF_RAZE; return; }
  if (side.gold < 100) bias += d.poor;
  if (d.sack != 0 && g.rng.dice(1, 1000, 0) < d.sack + bias) { grp.flags = GF_SACK; return; }
  if (d.pillage != 0 && g.rng.dice(1, 1000, 0) < d.pillage + bias) grp.flags = GF_PILLAGE;
}

void prepare(Game& g, Side& side) {
  AIData& d = data(g, side);
  int active = 0;
  for (int gi = d.maxGroups; gi >= 1; gi--) if (d.groups[gi]->active != 0) active++;
  d.minStrength = 0; d.flyCities = 0; d.strongCities = 0; d.fastCities = 0;
  std::vector<int> top;
  for (City* c : own(g, side)) {
    clearCflag(d, *c, CF_MOVE12);
    clearCflag(d, *c, CF_FLYGROUP);
    clearCflag(d, *c, CF_MOVE16);
    if (role(d, *c) != RALLY) {
      auto [good, slotI] = cities::buildsWell(g, side, *c);
      if (good) {
        const Slot& slot = c->slots[slotI];
        top.push_back(slot.strength + (g.types.byId(slot.type)->siege ? 2 : 0));
      }
    }
  }
  luaSort(top, [](int p, int q) { return p > q; });
  int least = 100;
  for (size_t i = 0; i < std::min<size_t>(8, top.size()); i++) if (top[i] < least) least = top[i];
  d.minStrength = std::min(4, least);

  auto strong = [&](const Slot& slot) { return g.types.byId(slot.type)->siege || d.minStrength <= slot.strength; };

  if (active == 4) {
    for (City* c : own(g, side)) {
      if (free(g, side, *c, true) && cflag(d, *c, CF_FLIER)) {
        bool ok = false;
        for (auto& slot : c->slots) if (g.types.byId(slot.type)->flies && slot.strength > 4) ok = true;
        if (ok) {
          d.flyCities++;
          setCflag(d, *c, CF_FLYGROUP);
        }
      }
    }
  }
  bool flyEnough = d.flyCities >= 4;
  int left = flyEnough ? d.flyCities - 4 : d.flyCities;
  if (active > 2) {
    for (City* c : own(g, side)) {
      if (free(g, side, *c, true)) {
        bool skip = false;
        if (cflag(d, *c, CF_FLYGROUP)) {
          if (left == 0) skip = true;
          else { clearCflag(d, *c, CF_FLYGROUP); left--; }
        }
        if (!skip) {
          bool ok = false;
          for (auto& slot : c->slots) if (strong(slot) && slot.move > 15) ok = true;
          if (ok) {
            d.strongCities++;
            setCflag(d, *c, CF_MOVE16);
          }
        }
      }
    }
  }
  bool manyStrong = d.strongCities > 7;
  int fastBuilders = 0;
  bool strongEnough = d.strongCities >= 4;
  left = strongEnough ? d.strongCities - 4 : d.strongCities;
  if (active > 1) {
    for (City* c : own(g, side)) {
      if (free(g, side, *c, true) && !cflag(d, *c, CF_FLYGROUP)) {
        bool skip = false;
        if (cflag(d, *c, CF_MOVE16)) {
          if (left == 0) skip = true;
          else { clearCflag(d, *c, CF_MOVE16); left--; }
        }
        if (!skip) {
          bool ok = false;
          for (auto& slot : c->slots) {
            if (strong(slot) && slot.move > 11) {
              fastBuilders++;
              if (!manyStrong || slot.move > 15) ok = true;
            }
          }
          if (ok) {
            d.fastCities++;
            setCflag(d, *c, CF_MOVE12);
          }
        }
      }
    }
  }
  bool fastEnough = d.fastCities >= 4;
  left = fastEnough ? d.fastCities - 4 : d.fastCities;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == side.index && !cflag(d, c, CF_FLYGROUP) && !cflag(d, c, CF_MOVE16) && cflag(d, c, CF_MOVE12) &&
        left != 0) {
      clearCflag(d, c, CF_MOVE12);
      left--;
    }
  }
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && gi <= 4) {
      grp->size = 8;
      if (gi == 1) {
        grp->size = fastBuilders < 8 ? 8 : 12;
      } else if (gi == 2) {
        grp->flags = clear(grp->flags, GF_MOVE12);
        if (fastEnough) { grp->flags = set(grp->flags, GF_MOVE12); grp->size = 12; }
      } else if (gi == 3) {
        grp->flags = clear(grp->flags, GF_MOVE16);
        if (strongEnough) { grp->flags = set(grp->flags, GF_MOVE16); grp->size = 16; }
      } else if (gi == 4) {
        grp->flags = clear(grp->flags, GF_FLY);
        if (flyEnough) { grp->flags = set(grp->flags, GF_FLY); grp->size = 12; }
      }
    }
  }
}

// Drop staged stacks that are gone or have left the group (563e:1251).
static void cleanStaged(Game& g, Side& side, int gi) {
  auto grp = data(g, side).groups[gi];
  for (int s = 1; s <= 4; s++) {
    auto it = grp->staged.find(s);
    if (it == grp->staged.end()) continue;
    Army* a = it->second;
    if (a && !(alive(g, a) && a->owner == side.index && a->aiGroup == gi)) grp->staged.erase(it);
  }
}

// Re-check the plan (563e:041c). Returns the number of targets.
static int checkPlan(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  int n = 0;
  for (int k = 1; k <= 6; k++) {
    City* t = city(g, grp->cities, k);
    if (t) {
      if (d.questCity == t->index || owner(*t) != grp->target) grp->cities.erase(k);
      else n++;
    }
  }
  City* rally = city(g, grp->rally);
  if (!rally) return n;
  auto [nb, nd] = neighbours(g, *rally);
  for (int j = (int)nb.size() - 1; j >= 0; j--) {
    City* m = nb[j];
    if (owner(*m) == grp->target && !cflag(d, *m, CF_UNSEEN)) {
      bool have = false;
      for (int k = 1; k <= 6; k++) if (hasKey(grp->cities, k) && grp->cities[k] == m->index) have = true;
      if (!have) {
        n++;
        int slot = NONE;
        bool worstSet = false;
        int worst = 0;
        for (int k = 6; k >= 1; k--) {
          if (!hasKey(grp->cities, k)) { slot = k; worst = -1; worstSet = true; break; }
          if ((worstSet ? worst : 0) < get(grp->dist, k)) { slot = k; worst = get(grp->dist, k); worstSet = true; }
        }
        if (slot != NONE && (worst == -1 || nd[j] < worst)) {
          grp->cities[slot] = m->index;
          grp->dist[slot] = nd[j];
        }
      }
    }
  }
  return n;
}

// With one target left, add its neighbours of the target side, and move the
// rally to one of the side's own cities next to it (563e:1579).
static void adjustRally(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  int only = NONE, count = 0;
  for (int k = 1; k <= 6; k++) if (hasKey(grp->cities, k)) { only = grp->cities[k]; count++; }
  if (count != 1) return;
  City* oc = city(g, only);
  if (!oc) return;
  auto nb = neighbours(g, *oc).first;
  City* next = nullptr;
  bool rallyNear = false;
  for (size_t j = 0; j < nb.size(); j++) {
    City* m = nb[j];
    if (!cflag(d, *m, CF_UNSEEN)) {
      if (owner(*m) == grp->target) {
        for (int k = 1; k <= 6; k++) {
          if (!hasKey(grp->cities, k)) { grp->cities[k] = m->index; break; }
        }
      } else if (m->ownerIndex == side.index) {
        if (m->index == grp->rally) rallyNear = true;
        if (!isRally(g, side, *m) && !next) next = m;
      }
    }
  }
  if (!rallyNear && next) grp->rally = next->index;
}

// Move the rally to the side's nearest neighbouring city that is no other
// group's rally (563e:02ed); cancel the group when there is none.
static bool relocate(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  City* r = city(g, grp->rally);
  City* best = nullptr;
  if (r) {
    auto nb = neighbours(g, *r).first;
    int bestD = 1000;
    for (int j = (int)nb.size() - 1; j >= 0; j--) {
      City* m = nb[j];
      if (m->ownerIndex == side.index && !isRally(g, side, *m)) {
        int dd = dist(m->x, m->y, r->x, r->y);
        if (dd < bestD) { best = m; bestD = dd; }
      }
    }
  }
  if (!best) {
    cancel(g, side, gi);
    return false;
  }
  grp->rally = best->index;
  return true;
}

// A city the group has taken (563e:134c).
static bool takeCity(Game& g, Side& side, int gi, City& c) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  if (grp->rally == c.index) return false;
  cities::garrison(g, side, c, true);
  for (int k = 1; k <= 6; k++) {
    if (hasKey(grp->cities, k) && grp->cities[k] == c.index) { grp->cities.erase(k); break; }
  }
  auto near = [&](const City& x) {
    auto [nb, nd] = neighbours(g, x);
    int n = 0;
    for (size_t j = 0; j < nb.size(); j++) {
      if (owner(*nb[j]) == grp->target) {
        if (nd[j] < 10) n++;
        if (nd[j] < 20) n++;
        if (nd[j] < 30) n++;
        if (nd[j] < 50) n++;
      }
    }
    return n;
  };
  int here = near(c);
  City* rally = city(g, grp->rally);
  int there = rally ? near(*rally) : 0;
  City* pick = there < here ? &c : rally;
  if (pick) {
    bool have = false;
    for (int k = 1; k <= 6; k++) if (hasKey(grp->taken, k) && grp->taken[k] == pick->index) have = true;
    if (!have) {
      for (int k = 1; k <= 6; k++) {
        if (!hasKey(grp->taken, k)) { grp->taken[k] = pick->index; break; }
      }
    }
  }
  if (here == 0 && there == 0) {
    cancel(g, side, gi);
    return false;
  }
  grp->rally = pick->index;
  setRole(d, c, RALLY);
  followUp(g, side, gi);
  return true;
}

int march(Game& g, Side& side, int gi, std::vector<Army*> list, City* c) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  Army* lead = list.empty() ? nullptr : list[0];
  if (lead && alive(g, lead) && !standing(*c)) {
    City* best = nullptr;
    int bestD = 1000;
    for (int k = 1; k <= 6; k++) {
      City* t = city(g, grp->cities, k);
      if (t) {
        int dd = dist(lead->x, lead->y, t->x, t->y);
        if (dd < bestD && standing(*t)) { best = t; bestD = dd; }
      }
    }
    if (best) c = best;
  }
  for (;;) {
    int n = 0;
    for (int k = 1; k <= 6; k++) if (hasKey(grp->cities, k)) n++;
    std::vector<Army*> keep;
    for (Army* a : list) if (alive(g, a) && a->owner == side.index) keep.push_back(a);
    list = keep;
    int was = owner(*c);
    auto sel = order(g, list, ORDER_CITY, c->index, 0);
    if (!sel) { cleanStaged(g, side, gi); return 3; }
    int r = moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
    if (r == 1) {
      game::disband(g, side, sel->armies);
      cleanStaged(g, side, gi);
      return 3;
    }
    cleanStaged(g, side, gi);
    if (c->ownerIndex != side.index) return 3;
    if (was == side.index) return 3;
    bool capital = side.capital == c;
    int flags = grp->flags;
    if (!capital && n >= 2 && has(flags, GF_RAZE)) {
      City* nextCity = raze(g, side, *c, false, &*sel);
      if (!nextCity) return 1;
      c = nextCity;
      list = sel->armies;
    } else {
      if (!capital && has(flags, GF_SACK)) sack(g, side, *c);
      else if (!capital && has(flags, GF_PILLAGE)) pillage(g, side, *c);
      takeCity(g, side, gi, *c);
      return 1;
    }
  }
}

// Look for a stack of the target side worth hitting near a staged stack
// (563e:0c32). Returns 2 if it went.
static int hitArmies(Game& g, Side& side, int gi, std::vector<Army*>& list, int target, int range, const Flood& flood,
                     const Sel& sel) {
  Army* lead = list.empty() ? nullptr : list[0];
  if (!lead || lead->atSea) return 0;
  int bx = NONE, by = NONE, bestOdds = 0;
  for (int x = lead->x - range; x <= lead->x + range - 1; x++) {
    for (int y = lead->y - range; y <= lead->y + range - 1; y++) {
      if (x >= 0 && y >= 0 && x < g.map->width && y < g.map->height) {
        auto here = game::armiesAt(g, x, y);
        int t = terrain(g, x, y);
        if (!here.empty() && here[0]->owner == target && t != move::CITY && t != move::SHORE && t != move::WATER &&
            floodAt(flood, x, y) <= sel.minMoves - 1 && game::seen(g, side.index, x, y)) {
          int count = (int)here.size();
          int strength = 0, heroes = 0;
          for (Army* a : here) {
            strength += a->strength;
            if (isHero(a)) heroes++;
          }
          int o = odds(g, sel, x, y);
          if (o > 75 && ((count > 2 && strength > 10) || heroes != 0) && bestOdds < o) {
            bx = x;
            by = y;
            bestOdds = o;
          }
        }
      }
    }
  }
  if (bx == NONE) return 0;
  int dest = lead->aiOrder == ORDER_CITY ? lead->aiDest : NONE;
  auto s = order(g, list, ORDER_ROAM, 0, 0x20);
  if (s) {
    s->leader->target = std::make_pair(bx, by);
    moveTo(g, *s, bx, by);
    for (Army* a : list) {
      if (alive(g, a) && a->owner == side.index && dest != NONE) {
        a->aiOrder = ORDER_CITY;
        a->aiDest = dest;
      }
    }
  }
  return 2;
}

// A staged stack's step (563e:0b49).
static int stagedStep(Game& g, Side& side, int gi, std::vector<Army*>& list, int target, const Flood& flood,
                      const Sel& sel) {
  City* nearest = nullptr;
  int nd = 1000;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == target) {
      int dd = std::get<0>(cityDistance(g, flood, c));
      if (dd < 50 && dd < sel.minMoves) {
        int o = odds(g, sel, c.x, c.y);
        if (o > 75 && dd < nd) { nearest = &c; nd = dd; }
      }
    }
  }
  int r = hitArmies(g, side, gi, list, target, 15, flood, sel);
  if (r != 0 && nearest) r = march(g, side, gi, list, nearest);
  return r;
}

// Move the group's staged stacks on (563e:0996).
static void gather(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  for (int s = 4; s >= 1; s--) {
    bool again = true;
    while (again) {
      again = false;
      cleanStaged(g, side, gi);
      auto it = grp->staged.find(s);
      Army* a = it == grp->staged.end() ? nullptr : it->second;
      if (!a) break;
      auto list = collectOrdered(g, side.index, a->x, a->y, a->aiGroup, a->aiOrder, 0);
      if (list.empty()) break;
      Sel sel = select(g, list);
      Flood flood = floodFrom(g, side.index, a->x, a->y, 15, &sel);
      std::vector<std::array<int, 3>> was;
      for (Army* b : list) was.push_back({b->x, b->y, b->moves});
      size_t armies = g.armies.size();
      int r = stagedStep(g, side, gi, list, grp->target, flood, sel);
      if (r == 0) {
        Army* last = list.empty() ? nullptr : list.back();
        if (last && alive(g, last) && last->aiOrder == ORDER_CITY && city(g, last->aiDest)) {
          march(g, side, gi, list, city(g, last->aiDest));
        }
      } else if (r == 2) {
        // The original goes round again for as long as a stack is found to
        // strike. One that cannot take a step would be found every time --
        // until the odds' dice fall short, which for a strong stack is
        // never -- so a pass that changed nothing ends it here.
        again = g.armies.size() != armies;
        for (size_t i = 0; i < list.size() && i < was.size(); i++) {
          Army* b = list[i];
          if (b->x != was[i][0] || b->y != was[i][1] || b->moves != was[i][2]) again = true;
        }
      }
    }
  }
}

// Strike from the rally city (563e:06e9).
static bool strike(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  City* r = city(g, grp->rally);
  if (!r) return false;
  auto list = collect(g, side.index, r->x + 1, r->y, 0);
  int n = (int)list.size();
  if (n == 0) return false;
  Sel sel = select(g, list);
  City* best = nullptr;
  int bestScore = -1;
  for (int k = 1; k <= 6; k++) {
    City* t = city(g, grp->cities, k);
    if (t) {
      int dk = hasKey(grp->dist, k) ? grp->dist[k] : -1;
      int turns = dk / std::max(1, sel.minMoves - 2) + 1;   // truncating, as the original divides
      int o = odds(g, sel, t->x, t->y);
      int score = (turns < 11 ? 10 - turns : 0) + o + get(grp->bonus, k) + g.rng.dice(1, 4, 0);
      if (turns == 1) score += 100;
      if ((o > 75 || n > 7) && bestScore < score) { best = t; bestScore = score; }
    }
  }
  if (!best) return false;
  // the stack's lead -- a hero, else the strongest -- is staged (563e:08b2)
  int slot = NONE;
  for (int s = 4; s >= 1; s--) {
    auto it = grp->staged.find(s);
    if (it == grp->staged.end() || !it->second) { slot = s; break; }
  }
  if (slot != NONE) {
    Army* pick = nullptr;
    int strongest = -1;
    for (int i = (int)list.size() - 1; i >= 0; i--) {
      Army* a = list[i];
      if (isHero(a)) { pick = a; break; }
      if (strongest < a->strength) { pick = a; strongest = a->strength; }
    }
    grp->staged[slot] = pick;
  }
  for (Army* a : list) a->aiGroup = gi;
  march(g, side, gi, list, best);
  return true;
}

void followUp(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  for (int k = 1; k <= 6; k++) {
    City* t = city(g, grp->taken, k);
    if (t && t->index != grp->rally && t->ownerIndex == side.index) {
      auto [nb, nd] = neighbours(g, *t);
      int near = 0;
      for (int j = (int)nb.size() - 1; j >= 0; j--) {
        int o = nb[j]->ownerIndex;
        if (o != NONE && o != side.index && nd[j] < 25) near++;
      }
      if (near == 0) {
        cities::garrison(g, side, *t, true);
        auto list = collectOrdered(g, side.index, t->x + 1, t->y, 0, grp->size, 0);
        if (list.size() > 3) {
          auto sel = order(g, list, ORDER_CITY, grp->rally, 0);
          if (sel) moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
        }
      }
    }
  }
}

// Cancel the longest-running group (563e:0607).
static void cancelOldest(Game& g, Side& side) {
  AIData& d = data(g, side);
  int oldest = NONE, age = -1;
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && age < grp->active) { oldest = gi; age = grp->active; }
  }
  if (oldest != NONE) cancel(g, side, oldest);
}

// A group's turn (563e:00ca).
static bool runGroup(Game& g, Side& side, int gi) {
  AIData& d = data(g, side);
  auto grp = d.groups[gi];
  City* cap = side.capital;
  if (cap && cap->ownerIndex != NONE && cap->ownerIndex != side.index) {
    bool going = false;
    for (int k = d.maxGroups; k >= 1; k--) {
      auto o = d.groups[k];
      if (o->active != 0 && o->target == cap->ownerIndex) going = true;
    }
    if (!going) {
      cancelOldest(g, side);
      return false;
    }
  }
  if (checkPlan(g, side, gi) == 0) {
    cancel(g, side, gi);
    return false;
  }
  adjustRally(g, side, gi);
  bool shared = false;
  for (int k = d.maxGroups; k >= 1; k--) {
    auto o = d.groups[k];
    if (o->active != 0 && k != gi && o->rally == grp->rally) shared = true;
  }
  City* r = city(g, grp->rally);
  if ((!shared && r && r->ownerIndex == side.index) || relocate(g, side, gi)) {
    gather(g, side, gi);
    if (d.groups[gi]->active != 0) {
      City* rc = city(g, d.groups[gi]->rally);
      if (rc) cities::garrison(g, side, *rc, true);
      if (strike(g, side, gi)) {
        rc = city(g, d.groups[gi]->rally);
        if (rc) cities::garrison(g, side, *rc, true);
      }
      if (d.groups[gi]->active != 0) {
        followUp(g, side, gi);
        return true;
      }
    }
  }
  return false;
}

void assault(Game& g, Side& side) {
  AIData& d = data(g, side);
  if (d.turns == 0) return;
  prepare(g, side);
  for (City* c : own(g, side)) {
    if (role(d, *c) == RALLY) setRole(d, *c, STOP);
  }
  for (int gi = 1; gi <= d.maxGroups; gi++) {
    auto grp = d.groups[gi];
    if (grp->active != 0) {
      City* r = city(g, grp->rally);
      if (r) setRole(d, *r, RALLY);
      if (runGroup(g, side, gi)) d.groups[gi]->active++;
    }
  }
}

// The enemy city with most of its own side's cities round it (5f19:04d2).
static City* hub(Game& g, int enemy) {
  City* best = nullptr;
  int most = 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == enemy) {
      int n = 1;
      auto nb = neighbours(g, c).first;
      for (int j = (int)nb.size() - 1; j >= 0; j--) if (nb[j]->ownerIndex == enemy) n++;
      if (n > most || (n == most && g.rng.dice(1, 2, -1) != 0)) { best = &c; most = n; }
    }
  }
  return best;
}

// Distances over the neighbour graph from a city (5f19:058a).
static void spread(Game& g, const City& from, std::vector<char>& reached, std::vector<int>& dist) {
  size_t n = g.map->cities.size();
  reached.assign(n, 0);
  dist.assign(n, 0);
  std::vector<char> done(n, 0);
  reached[from.index] = 1;
  dist[from.index] = 0;
  bool changed = true;
  while (changed) {
    changed = false;
    for (int i = (int)n - 1; i >= 0; i--) {
      City& c = g.map->cities[i];
      if (reached[c.index] && !done[c.index]) {
        changed = true;
        done[c.index] = 1;
        auto [nb, nd] = neighbours(g, c);
        for (int j = (int)nb.size() - 1; j >= 0; j--) {
          City* m = nb[j];
          int nd2 = dist[c.index] + nd[j];
          if (!reached[m->index]) {
            reached[m->index] = 1;
            dist[m->index] = nd2;
          } else if (nd2 < dist[m->index]) {
            dist[m->index] = nd2;
          }
        }
      }
    }
  }
}

// Plan a group against a side (5f19:01bb); the rally city, or null.
static City* plan(Game& g, Side& side, Group& grp, int enemy) {
  AIData& d = data(g, side);
  grp.cities.clear();
  grp.dist.clear();
  City* h = hub(g, enemy);
  if (!h) return nullptr;
  std::vector<char> reached;
  std::vector<int> dist;
  spread(g, *h, reached, dist);
  City* rally = nullptr;
  long best = 1000000000;
  for (City* c : own(g, side)) {
    if (RALLY_ROLES.count(role(d, *c)) && reached[c->index] && dist[c->index] < best) {
      rally = c;
      best = dist[c->index];
    }
  }
  if (!rally) {
    if (!cflag(d, *h, CF_UNSEEN)) {
      long bestD = 1000000000;
      for (City* c : own(g, side)) {
        if (RALLY_ROLES.count(role(d, *c))) {
          int dd = core::dist(c->x, c->y, h->x, h->y);
          if (dd < bestD) { rally = c; bestD = dd; }
        }
      }
      if (rally) { grp.cities[1] = h->index; grp.dist[1] = 100; }
    }
    return rally;
  }
  spread(g, *rally, reached, dist);
  int n = 0;
  for (int k = 1; k <= 6; k++) {
    City* pick = nullptr;
    long pd = 1000000000;
    for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
      City& c = g.map->cities[i];
      if (c.ownerIndex == enemy && !cflag(d, c, CF_UNSEEN) && reached[c.index] && dist[c.index] < pd) {
        pick = &c;
        pd = dist[c.index];
      }
    }
    if (!pick) break;
    grp.cities[k] = pick->index;
    grp.dist[k] = (int)pd;
    reached[pick->index] = 0;
    n++;
  }
  if (n == 0 && !cflag(d, *h, CF_UNSEEN)) {
    grp.cities[1] = h->index;
    grp.dist[1] = 100;
    n = 1;
  }
  if (n == 0) return nullptr;
  return rally;
}

int pickEnemy(Game& g, Side& side) {
  AIData& d = data(g, side);
  int me = side.index;
  City* cap = side.capital;
  int capOwner = cap ? owner(*cap) : NEUTRAL;
  int held[8], score[8], count[8], capHeld[8], fromThem[8], targeted[8], rnd[8];
  for (int s = 0; s < 8; s++) {
    held[s] = 0; score[s] = 0; count[s] = 0; capHeld[s] = 0; fromThem[s] = 0; targeted[s] = 0;
    rnd[s] = g.rng.dice(1, 10, 0);
  }
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    int o = c.ownerIndex;
    if (o != NONE) {
      count[o]++;
      if (!cflag(d, c, CF_UNSEEN)) held[o]++;
      if (o == me && c.claim != NONE && c.claim != NEUTRAL && c.claim >= 0 && c.claim < 8) fromThem[c.claim]++;
    }
  }
  int nGroups = 0;
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && grp->target != NONE) {
      if (grp->target >= 0 && grp->target < 8) targeted[grp->target]++;
      nGroups++;
    }
  }
  if (capOwner != NEUTRAL && capOwner != me) score[capOwner] += 20;
  int others = 0;
  for (int s = 7; s >= 0; s--) {
    if (count[s] != 0) {
      if (s != me) others++;
      City* theirs = g.map->sides[s].capital;
      if (theirs && theirs->ownerIndex == me) capHeld[s] = 1;
    }
  }
  int constant;
  if (!side.computer) constant = d.dieHuman;
  else if (side.level == 2) constant = d.dieWarlord;
  else if (side.level == 1) constant = d.dieLord;
  else constant = d.dieKnight;
  for (int s = 7; s >= 0; s--) {
    const Side& o = g.map->sides[s];
    score[s] += rnd[s] + capHeld[s] * 15 + fromThem[s] * 4 + d.heroesKilled[s] * 4 + d.armiesKilled[s] + d.battles[s] +
                d.lost[s] * 2 + d.cityBattles[s] * 2 + d.citiesLost[s] * 2 + constant;
    score[s] += std::abs(side.diploScore - o.diploScore) / 8;
    score[s] += std::abs(count[me] - count[s]) / 4;
  }
  bool proposingWar = false;
  for (int s = 7; s >= 0; s--) {
    if (inPlay(g, s) && s != me && proposal(g, me, s) == 2) proposingWar = true;
  }
  if (g.greatest && side.computer) {
    auto fights = fightingHumans(g);
    for (int s = 7; s >= 0; s--) {
      if (inPlay(g, s) && isComputer(g, s) && (fights[me] || fights[s])) score[s] = 0;
    }
  }
  int early = g.map->options.quickStart != 0 ? 4 : 8;
  for (int s = 7; s >= 0; s--) {
    int fought = d.battles[s] + d.cityBattles[s];
    if (diplo::spared(g, side, s)) score[s] = 0;
    if (proposingWar && proposal(g, me, s) == 0) score[s] = 0;
    if (nGroups == 0 && held[s] == 0) score[s] = 0;
    if (g.turn < early && fought == 0) score[s] = 0;
    if (others > 1 && count[s] / 4 < targeted[s]) score[s] = 0;
  }
  score[me] = 0;
  int pick = NONE, top = 0;
  for (int s = 7; s >= 0; s--) {
    if (count[s] != 0 && top < score[s]) { pick = s; top = score[s]; }
  }
  if (capOwner != NEUTRAL && capOwner != me) {
    // 5f19:1393 compares the capital's holder with the groups-per-side
    // counts, not with the groups' targets: kept as it is
    bool found = false;
    for (int gi = d.maxGroups - 1; gi >= 0; gi--) if (gi < 8 && capOwner == targeted[gi]) found = true;
    if (!found) pick = capOwner;
  }
  if (pick != NONE && held[pick] == 0) pick = NONE;
  return pick;
}

std::array<bool, 8> fightingHumans(const Game& g) {
  std::array<bool, 8> out{};
  for (int s = 0; s < 8; s++) {
    if (inPlay(g, s) && isComputer(g, s)) {
      for (int h = 0; h < 8; h++) {
        if (inPlay(g, h) && !isComputer(g, h) && state(g, s, h) == 2) out[s] = true;
      }
    }
  }
  return out;
}

void assaultXX(Game& g, Side& side) {
  AIData& d = data(g, side);
  if (d.turns == 0) return;
  int quiet = 0;
  for (City* c : own(g, side)) {
    int r = role(d, *c);
    if (r == BUILDING || r == STOP) quiet++;
  }
  int active = 0;
  for (int gi = 1; gi <= MAX_GROUPS; gi++) if (d.groups[gi]->active != 0) active++;
  if (!(active < d.maxGroups && quiet != 0 && (active == 0 || quiet > 2))) return;
  int slot = NONE;
  for (int gi = d.maxGroups; gi >= 1; gi--) if (d.groups[gi]->active == 0) slot = gi;
  if (slot == NONE) return;
  d.groups[slot] = emptyGroup();
  int enemy = pickEnemy(g, side);
  if (enemy == NONE) return;
  auto grp = d.groups[slot];
  City* rally = plan(g, side, *grp, enemy);
  if (!rally) {
    d.groups[slot] = emptyGroup();
    return;
  }
  grp->active = 1;
  grp->target = enemy;
  grp->rally = rally->index;
  setRole(d, *rally, RALLY);
  rollSpoils(g, side, *grp);
  propose(g, side.index, enemy, 2);
  // the rally city is no group's member any more (5f19:083e)
  for (int gi = d.maxGroups; gi >= 1; gi--) {
    auto o = d.groups[gi];
    if (o->active != 0) {
      for (int k = 1; k <= 4; k++) {
        if (hasKey(o->members, k) && o->members[k] == rally->index) {
          o->members.erase(k);
          rally->vectorTo = NONE;
        }
      }
    }
  }
  // each target's bonus: its own side's cities round it (5f19:09ca)
  for (int k = 1; k <= 6; k++) {
    City* t = city(g, grp->cities, k);
    if (t) {
      int n = 0;
      for (City* m : neighbours(g, *t).first) if (m->ownerIndex == enemy) n++;
      grp->bonus[k] = n;
    }
  }
  prepare(g, side);
}

}  // namespace w2::ai::groups
