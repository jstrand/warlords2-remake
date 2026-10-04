#include "warlords/ai/moves.hpp"

#include <algorithm>
#include <cstdlib>

#include "util/util.hpp"
#include "warlords/ai/cities.hpp"
#include "warlords/ai/diplomacy.hpp"
#include "warlords/game.hpp"
#include "warlords/move.hpp"
#include "warlords/scn.hpp"
#include "warlords/site.hpp"

namespace w2::ai::moves {

using namespace core;

static City* city(Game& g, int i) { return i != NONE ? g.map->city(i) : nullptr; }

static bool onCityTile(const Game& g, const Army* a) {
  City* c = cityAt(g, a->x, a->y);
  return c && standing(*c);
}

// The computer's visit to a sage (5e97:0080).
static void sage(Game& g, Side& side) {
  AIData& d = data(g, side);
  int gold = g.rng.dice(3, 500, 500);
  City* best = nullptr;
  int bestCount = -1, bestD = 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == NONE && cflag(d, c, CF_UNSEEN)) {
      bool nextToUs = false;
      for (City* n : neighbours(g, c).first) if (n->ownerIndex == side.index) nextToUs = true;
      if (nextToUs) {
        int count = 0, dOwn = 1000;
        for (auto& o : g.map->cities) {
          if (o.ownerIndex == NONE) {
            if (cflag(d, o, CF_UNSEEN) && dist(c.x, c.y, o.x, o.y) < 20) count++;
          } else if (o.ownerIndex == side.index) {
            int dd = dist(c.x, c.y, o.x, o.y);
            if (dd < dOwn) dOwn = dd;
          }
        }
        if (count > 3 && (bestCount < count || (count == bestCount && bestD < dOwn))) {
          best = &c;
          bestCount = count;
          bestD = dOwn;
        }
      }
    }
  }
  if (g.map->options.hiddenMap == 0 || !best) {
    side.gold += gold;
    return;
  }
  int x = std::max(0, best->x + g.rng.dice(1, 11, -6));
  int y = std::max(0, best->y + g.rng.dice(1, 11, -6));
  x = std::min(x, g.map->width - 1);
  y = std::min(y, g.map->height - 1);
  site::sageMap(g, side, x, y);
  move::invalidate(g);
}

int searchSite(Game& g, Sel* sel, Army* h, Site& s) {
  if (!(h && alive(g, h) && h->x == s.x && h->y == s.y && h->moves != 0)) return 0;
  Side& side = *g.map->side(h->owner);
  if (siteOpen(s)) {
    std::vector<Army*> stack = sel ? sel->armies : std::vector<Army*>{h};
    auto r = site::search(g, stack, s.x, s.y, false);
    if (r && r->kind == "sage") sage(g, side);
    if (!isHero(h)) return 0;
    for (Army* a : stack) a->done = false;
    if (g.map->options.hiddenMap != 0 && s.content == site::ALLIES) {
      for (Army* a : onTile(g, side.index, s.x, s.y)) {
        if (a->aiOrder == 0 && magical(g, a)) {
          a->aiExplore = true;
          explore(g, side, a, nullptr);
        }
      }
    }
  }
  return 2;
}

int party(Game& g, Side& side, std::vector<Army*> list) {
  Army* hero = nullptr;
  Army* flier = nullptr;
  for (Army* a : list) {
    if (!hero && isHero(a)) hero = a;
    if (!flier && flies(g, a)) flier = a;
  }
  if (!hero) return 0;
  if (!flier) return 1;
  AIData& d = data(g, side);
  list = {hero, flier};
  clearOrder(hero);
  clearOrder(flier);
  int score = -1;
  Site* bestSite = nullptr;
  City* bestCity = nullptr;
  for (int i = (int)g.map->sites.size() - 1; i >= 0; i--) {
    Site& s = g.map->sites[i];
    bool taken = false;
    for (Army* o : armies(g, side.index)) {
      if (o != hero && isHero(o) && o->aiOrder == ORDER_SITE && o->aiDest == s.index) taken = true;
    }
    if (!taken && explored(g, side.index, s.x, s.y) && s.content != site::TEMPLE && !s.searched) {
      int dd = dist(hero->x, hero->y, s.x, s.y);
      int base = NONE;
      if (dd < 15 && score < 215 - dd) base = 215;
      else if (!(dd > 39 || 90 - dd <= score)) base = 90;
      if (base != NONE) { score = base - dd; bestSite = &s; }
    }
  }
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == side.index) {
      int dd = dist(hero->x, hero->y, c.x, c.y);
      if (role(d, c) == RALLY) dd -= 80;
      int base = NONE;
      if (dd < 15 && score < 115 - dd) base = 115;
      else if (!(dd > 39 || 40 - dd <= score)) base = 40;
      if (base != NONE) { score = base - dd; bestCity = &c; }
    }
  }
  if (bestSite) {
    auto sel = order(g, list, ORDER_SITE, bestSite->index, 0x100);
    if (sel) moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
    return searchSite(g, sel ? &*sel : nullptr, hero, *bestSite);
  }
  if (bestCity && dist(hero->x, hero->y, bestCity->x, bestCity->y) > 2) {
    auto sel = order(g, list, ORDER_CITY, bestCity->index, 0);
    if (sel) moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
  }
  return 0;
}

namespace {
struct Scan {
  bool found = false;
  int tx = NONE, ty = NONE;
  int kind = 0;
  int ex = NONE, ey = NONE;
};

// Look over the 10 x 10 tiles round an explorer (57ea:177f) for where to go.
Scan scan(Game& g, Side& side, Army* a, const Sel& sel) {
  int hx, hy;
  if (City* hc = city(g, a->homeCity)) { hx = hc->x; hy = hc->y; }
  else if (side.capital) { hx = side.capital->x; hy = side.capital->y; }
  else { hx = a->x; hy = a->y; }
  bool coast = a->atSea;
  if (g.rng.dice(1, 4, -1) == 0 && onCityTile(g, a) && !isHero(a)) {
    City* c = cityAt(g, a->x, a->y);
    if (c && move::isPort(g, *c)) coast = true;
  }
  bool flying = sel.mode == move::FLYING;
  bool hl = move::stackMode(g, sel.armies).hills;
  Scan out;
  int best = 0;
  auto exploredRound = [&](int x, int y) {
    for (int nx = x - 1; nx <= x + 1; nx++) {
      for (int ny = y - 1; ny <= y + 1; ny++) {
        if (nx >= 0 && ny >= 0 && nx < g.map->width && ny < g.map->height && game::seen(g, side.index, nx, ny)) return true;
      }
    }
    return false;
  };
  for (int x = a->x - 5; x <= a->x + 4; x++) {
    for (int y = a->y - 5; y <= a->y + 4; y++) {
      if (x >= 0 && y >= 0 && x < g.map->width && y < g.map->height) {
        int t = terrain(g, x, y);
        City* c = cityAt(g, x, y);
        if (t == move::CITY && (!c || c->ownerIndex != side.index)) {
          int dd = flying ? 2 : dist(a->x, a->y, x, y);
          if (dd < 4 && (!a->atSea || (c && move::isPort(g, *c))) && exploredRound(x, y)) {
            out.kind = 1;
            out.ex = x;
            out.ey = y;
          }
        }
        if (t == move::SITE && out.kind == 0 && !a->atSea && exploredRound(x, y)) {
          out.kind = 2;
          out.ex = x;
          out.ey = y;
        }
        bool road = scn::roadAt(*g.map, x, y) % 0x20 != 0;
        if (t != move::CITY && (flying || road || hl || t != move::HILLS) && !game::seen(g, side.index, x, y) &&
            (flying || move::COST[t] != 0)) {
          int unseen = 0;
          for (int nx = x - 1; nx <= x + 1; nx++) {
            for (int ny = y - 1; ny <= y + 1; ny++) {
              if (nx >= 0 && ny >= 0 && nx < g.map->width && ny < g.map->height && !game::seen(g, side.index, nx, ny)) unseen++;
            }
          }
          if (unseen > 1) {
            int bonus = 0;
            bool wet = t == move::WATER || t == move::SHORE;
            if (coast) {
              if (wet) bonus += 200;
            } else if (!wet) {
              if (t != move::HILLS) bonus = 100;
              if (road) bonus += 200;
            }
            int fromHome = dist(x, y, hx, hy);
            int s = g.rng.dice(1, 10, 0) + dist(x, y, a->x, a->y) * 2 + fromHome + unseen * 10 + bonus;
            if (coast && bonus != 0) s += std::min(fromHome, 50) * 10;
            if (best < s) {
              out.tx = x;
              out.ty = y;
              out.found = true;
              best = s;
            }
          }
        }
      }
    }
  }
  return out;
}

// A flier's fallback: the nearest unseen city, give or take 1d10 (57ea:164e);
// it aims one up and left of it.
std::pair<int, int> unseenCity(Game& g, Side& side, Army* a) {
  AIData& d = data(g, side);
  City* best = nullptr;
  int bestD = NONE;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (standing(c) && cflag(d, c, CF_UNSEEN)) {
      int dd = dist(c.x, c.y, a->x, a->y) + g.rng.dice(1, 10, 0);
      if (bestD == NONE || dd < bestD) { best = &c; bestD = dd; }
    }
  }
  if (best) return {best->x - 1, best->y - 1};
  return {NONE, NONE};
}

// An explorer next to a city goes for it (57ea:10a8) if it may and the odds
// are good. True if it went.
bool tryCity(Game& g, Side& side, Army* a, int ex, int ey) {
  AIData& d = data(g, side);
  City* pick = nullptr;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (standing(c) && c.ownerIndex != side.index && d.questCity != c.index && dist(c.x, c.y, ex, ey) <= 2 &&
        diplo::canAttack(g, side, c)) {
      if (cflag(d, c, CF_UNSEEN)) cities::clearUnseen(g, side);
      if (!cflag(d, c, CF_UNSEEN)) { pick = &c; break; }
    }
  }
  if (!pick) return false;
  auto sel = order(g, {a}, ORDER_ROAM, 0, 0x20);
  if (!sel) return false;
  int o = odds(g, *sel, pick->x, pick->y);
  int need = 75;
  if (!isHero(a) && a->strength < 4) {
    if (d.neutral < (int)g.map->cities.size() / 10) need = 50;
    else if (d.own <= std::min(g.turn / 2, 10)) need = 40;
  }
  if (o < need) return false;
  a->aiExplore = false;
  sel = order(g, {a}, ORDER_CITY, pick->index, 0x80);
  if (sel) moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
  return true;
}

// An explorer next to a site visits it (57ea:12eb). True if it went.
bool trySite(Game& g, Side& side, Army* a, int sx, int sy) {
  Site* s = siteAt(g, sx, sy);
  if (!s || !siteOpen(*s) || a->atSea) return false;
  bool temple = s->content == site::TEMPLE;
  if (temple && s->templeIndex != NONE && a->blessings.count(s->templeIndex)) return false;
  if (!(isHero(a) || temple)) return false;
  auto there = game::armiesAt(g, sx, sy);
  if (!there.empty() && there[0]->owner != side.index) return false;
  auto sel = order(g, {a}, ORDER_ROAM, 0, 0);
  if (!sel) return false;
  sel->leader->target = std::make_pair(sx, sy);
  int r = moveTo(g, *sel, sx, sy);
  if (r == 1) return false;
  if (alive(g, a)) {
    if (a->x == sx && a->y == sy) {
      if (isHero(a)) searchSite(g, &*sel, a, *s);
      else site::search(g, {a}, sx, sy, false);
      if (s->content == site::ALLIES) {
        for (Army* b : onTile(g, side.index, sx, sy)) {
          if (b->aiOrder == 0 && magical(g, b)) b->aiExplore = true;
        }
      }
    } else {
      if (temple && a->atSea && s->templeIndex != NONE) a->blessings.insert(s->templeIndex);
      a->done = true;
    }
    a->aiExplore = true;
    clearOrder(a);
    a->target.reset();
  }
  return true;
}

// Walk an explorer's stack to (x, y) (57ea:1581).
void walkTo(Game& g, Army* a, int x, int y) {
  a->target = std::make_pair(x, y);
  Sel sel = selectStackOf(g, a);
  int r = moveTo(g, sel, x, y);
  if (alive(g, a)) {
    if (r == 1) {
      a->aiExplore = false;
      clearOrder(a);
    }
    a->done = true;
    a->target.reset();
    a->group = 0;
  }
}

// One step of an explorer (57ea:0f46).
void exploreStep(Game& g, Side& side, Army* a, City* home) {
  Sel sel = select(g, {a});
  Scan sc = scan(g, side, a, sel);
  int tx = sc.tx, ty = sc.ty;
  if (!sc.found) {
    if (flies(g, a)) std::tie(tx, ty) = unseenCity(g, side, a);
    if (tx == NONE) {
      std::map<int, int> none;
      City* n = home ? cities::openNeutral(g, side, *home, none) : nullptr;
      if (!n) {
        clearOrder(a);
        a->aiExplore = false;
        a->target.reset();
        a->group = 0;
        return;
      }
      tx = n->x;
      ty = n->y;
    }
  } else if (sc.kind == 1) {
    if (tryCity(g, side, a, sc.ex, sc.ey)) return;
  } else if (sc.kind == 2) {
    if (trySite(g, side, a, sc.ex, sc.ey)) return;
  }
  walkTo(g, a, tx, ty);
}
}  // namespace

void explore(Game& g, Side& side, Army* a, City* home) {
  if (!alive(g, a)) return;
  order(g, {a}, ORDER_ROAM, 0, 0x20);
  int steps = a->moves / 5 + 1;
  for (;;) {
    if (!alive(g, a) || a->moves < 2) return;
    int px = a->x, py = a->y;
    exploreStep(g, side, a, home);
    if (!alive(g, a) || (a->x == px && a->y == py)) return;
    if (a->moves < 4) return;
    if (steps == 0) return;
    if (!a->aiExplore) return;
    steps--;
  }
}

void search(Game& g, Side& side) {
  AIData& d = data(g, side);
  bool rally = false;
  for (auto& c : g.map->cities) {
    if (c.ownerIndex == side.index && role(d, c) == RALLY) rally = true;
  }
  d.searchers = 0;
  d.explorers = 0;
  for (int pass = 0; pass <= 1; pass++) {
    for (Army* a : armies(g, side.index)) {
      if (alive(g, a) && !a->transit && a->aiExplore && ((pass == 0) == isHero(a))) {
        if ((isHero(a) && rally) || (flies(g, a) && d.explorers > 7 && a->strength < 4)) {
          clearOrder(a);
          a->aiExplore = false;
        } else {
          explore(g, side, a, nullptr);
          if (alive(g, a) && a->owner == side.index) {
            if (flies(g, a)) d.explorers++;
            else d.searchers++;
          }
        }
      }
    }
  }
}

void heroParties(Game& g, Side& side) {
  for (Army* a : armies(g, side.index)) {
    if (alive(g, a) && !a->transit && a->aiParty) {
      a->aiParty = false;
      if (isHero(a)) {
        bool going = true;
        int guard = 0;
        while (going && guard < 20) {
          guard++;
          if (!alive(g, a)) break;
          auto list = collectOrdered(g, side.index, a->x, a->y, a->aiGroup, a->aiOrder, 0);
          if (list.empty() || a->moves < 3) break;
          int px = a->x, py = a->y;
          int r = party(g, side, list);
          if (r == 0 || r == 1) going = false;
          if (alive(g, a) && a->x == px && a->y == py && a->maxMoves > a->moves) break;
        }
      }
    }
  }
}

// Is the stack's neutral target still the thing to do (5ad0:05ff)?
static bool neutralStillOn(Game& g, Side& side, Sel& sel) {
  AIData& d = data(g, side);
  Army* lead = sel.leader;
  City* dest = city(g, lead->aiDest);
  if (!dest) return true;
  if (dest->ownerIndex == side.index) {
    City* here = cityAt(g, lead->x, lead->y);
    auto nb = here ? cities::neutralNeighbours(g, side, *here) : std::vector<cities::NeutralNeighbour>{};
    if (nb.empty()) nb = cities::neutralNeighbours(g, side, *dest);
    int oddsV = 100;
    City* best = nullptr;
    int bd = 1000;
    for (int j = (int)nb.size() - 1; j >= 0; j--) {
      City* n = nb[j].city;
      if (!cflag(d, *n, CF_UNSEEN)) {
        if (g.map->options.neutralCities != 0) oddsV = odds(g, sel, n->x, n->y);
        int dd = dist(lead->x, lead->y, n->x, n->y);
        if (oddsV > 74 && dd < bd) { best = n; bd = dd; }
      }
    }
    if (!best) {
      if (g.map->options.hiddenMap != 0 && g.turn < 10) {
        std::vector<Army*> list = sel.armies;
        for (Army* a : list) explore(g, side, a, nullptr);
        return false;
      }
      return true;
    }
    for (Army* a : sel.armies) {
      a->aiOrder = ORDER_CITY;
      a->aiDest = best->index;
      setTarget(g, a, ORDER_CITY, best->index);
    }
    return true;
  }
  if (!diplo::canAttack(g, side, *dest)) {
    for (Army* a : sel.armies) {
      clearOrder(a);
      a->target.reset();
    }
    return false;
  }
  return true;
}

void moveAll(Game& g, Side& side) {
  AIData& d = data(g, side);
  for (Army* a : armies(g, side.index)) {
    if (!a->transit) a->aiMoved = a->moves < 2;
  }
  if (!d.cursor) d.cursor = std::make_pair(side.capX, side.capY);
  for (;;) {
    Army* pick = nullptr;
    int pd = NONE;
    for (Army* a : armies(g, side.index)) {
      if (!a->transit && a->x != NONE && !a->done && !a->aiMoved) {
        int dd = std::abs(a->x - d.cursor->first) + std::abs(a->y - d.cursor->second);
        if (dd == 0) dd = 9000;
        if (pd == NONE || dd < pd) { pick = a; pd = dd; }
      }
    }
    if (!pick) break;
    d.cursor = std::make_pair(pick->x, pick->y);
    pick->aiMoved = true;
    if (pick->target && pick->target->first != NONE && pick->target->first >= 0 && pick->moves > 1) {
      Sel sel = selectStackOf(g, pick);
      Army* lead = sel.leader;
      int ord = lead->aiOrder;
      City* dest = city(g, lead->aiDest);
      if (ord == ORDER_NONE || (ord == ORDER_CITY && (!dest || lead->aiDest == d.questCity || !standing(*dest)))) {
        for (Army* a : sel.armies) {
          clearOrder(a);
          a->target.reset();
          a->done = true;
        }
      } else if (!lead->aiNeutral || neutralStillOn(g, side, sel)) {
        dest = city(g, lead->aiDest);
        if (lead->aiOrder == ORDER_CITY && dest && dest->ownerIndex == side.index) {
          lead->target = std::make_pair(dest->x + 1, dest->y + 1);
        }
        if (lead->target && alive(g, lead)) {
          moveTo(g, sel, lead->target->first, lead->target->second);
        }
      }
    }
  }
}

// The best city for an idle stack by the flood (5ad0:0f3c).
static City* idleTarget(Game& g, Side& side, const Sel& sel, const Flood& flood, int n) {
  AIData& d = data(g, side);
  City* best = nullptr;
  int bestD = 1000;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    int ow = owner(c);
    if (standing(c) && !cflag(d, c, CF_UNSEEN) && d.questCity != c.index &&
        (ow == NEUTRAL || ow == side.index || state(g, side.index, ow) == 2)) {
      auto [dd, px, py] = cityDistance(g, flood, c);
      (void)py;
      if (px != NONE) {
        if (ow == side.index) {
          dd += role(d, c) == RALLY ? 10 : 30;
          if (n > 3) dd += 100;
          if (dd < bestD) { best = &c; bestD = dd; }
        } else if (odds(g, sel, c.x, c.y) > 74 && dd < bestD) {
          best = &c;
          bestD = dd;
        }
      }
    }
  }
  return best;
}

void sendIdle(Game& g, Side& side, Army* a) {
  int home = 1000;
  for (auto& c : g.map->cities) {
    if (c.ownerIndex == side.index && standing(c)) {
      int dd = dist(a->x, a->y, c.x, c.y);
      if (dd < home) home = dd;
    }
  }
  bool weak = a->strength < 3 || a->maxMoves < 8;
  if (!flies(g, a) && weak) {
    game::disband(g, side, {a});
    return;
  }
  auto list = collect(g, side.index, a->x, a->y, 8);
  if (list.empty()) return;
  int fl = 0, he = 0, ot = 0;
  for (Army* b : list) {
    if (flies(g, b)) fl++;
    else if (isHero(b)) he++;
    else ot++;
  }
  if (fl != 0 && he != 0 && ot != 0) {
    std::vector<Army*> keep;
    for (Army* b : list) if (flies(g, b) || isHero(b)) keep.push_back(b);
    list = keep;
  }
  if (list.size() < 2 && weak) {
    game::disband(g, side, {a});
    return;
  }
  Sel sel = select(g, list);
  int range = sel.hero ? 50 : (a->strength / 2) * 10;
  range = std::min(range, home + 10);
  Flood flood = floodFrom(g, side.index, a->x, a->y, range, &sel);
  City* c = idleTarget(g, side, sel, flood, (int)list.size());
  if (!c) {
    game::disband(g, side, list);
    return;
  }
  auto s = order(g, list, ORDER_CITY, c->index, 0);
  if (s) moveTo(g, *s, s->leader->target->first, s->leader->target->second);
}

void rescue(Game& g, Side& side) {
  for (Army* a : armies(g, side.index)) {
    if (alive(g, a) && !a->transit && a->x != NONE && a->maxMoves <= a->moves) {
      if (a->moves == a->maxMoves + 2 && !onCityTile(g, a) && a->aiOrder == ORDER_CITY) {
        City* dest = city(g, a->aiDest);
        if (!dest || !standing(*dest)) {
          int gone = a->aiDest;
          for (Army* b : onTile(g, side.index, a->x, a->y)) {
            if (b->aiOrder == ORDER_CITY && b->aiDest == gone) {
              clearOrder(b);
              b->aiGroup = 0;
            }
          }
        }
      }
      if (alive(g, a) && !a->aiExplore && a->aiOrder == 0 && !onCityTile(g, a)) sendIdle(g, side, a);
    }
  }
}

void lastRescue(Game& g, Side& side) {
  for (Army* a : armies(g, side.index)) {
    if (alive(g, a) && !a->transit && a->x != NONE && a->moves == a->maxMoves + 2 && !onCityTile(g, a)) {
      a->aiExplore = false;
      a->aiNeutral = false;
      clearOrder(a);
      a->target.reset();
      sendIdle(g, side, a);
    }
  }
}

void specials(Game& g, Side& side) {
  AIData& d = data(g, side);
  for (City* c : cities::own(g, side)) {
    if (cflag(d, *c, CF_HERO) && role(d, *c) != RALLY) {
      auto list = collectOrdered(g, side.index, c->x + 1, c->y, 0, 0, 0);
      if (!list.empty() && party(g, side, list) == 1 && !cities::building(*c)) setRole(d, *c, EXPLORER);
    }
  }
}

}  // namespace w2::ai::moves
