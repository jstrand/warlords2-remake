#include "warlords/ai/core.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <map>
#include <mutex>
#include <queue>

#include "util/util.hpp"
#include "warlords/ai.hpp"
#include "warlords/ai/groups.hpp"
#include "warlords/ai/moves.hpp"
#include "warlords/armytype.hpp"
#include "warlords/diplomacy.hpp"
#include "warlords/game.hpp"
#include "warlords/scn.hpp"
#include "warlords/site.hpp"

namespace w2::ai::core {

int dist(int x1, int y1, int x2, int y2) {
  double dx = x1 - x2, dy = y1 - y2;
  return (int)std::floor(std::sqrt(dx * dx + dy * dy));
}

bool inPlay(const Game& g, int index) {
  if (index < 0 || index > 7) return false;
  const Side& s = g.map->sides[index];
  return s.inUse && s.alive;
}

bool isComputer(const Game& g, int index) {
  if (index < 0 || index > 7) return false;
  return g.map->sides[index].computer;
}

int state(const Game& g, int a, int b) {
  if (a == NEUTRAL || b == NEUTRAL || a == NONE || b == NONE) return 2;
  if (a == b) return 0;
  return diplomacy::state(g, a, b);
}

int proposal(const Game& g, int a, int b) {
  if (a == b) return 0;
  return diplomacy::proposal(g, a, b);
}

void propose(Game& g, int a, int b, int v) {
  if (a == b) return;
  diplomacy::propose(g, a, b, v);
}

City* cityAt(const Game& g, int x, int y) { return game::cityAt(g, x, y); }

Site* siteAt(const Game& g, int x, int y) {
  if (g.map->siteAt.empty() || x < 0 || y < 0 || x >= g.map->width || y >= g.map->height) return nullptr;
  return g.map->siteAt[y * g.map->width + x];
}

int terrain(const Game& g, int x, int y) { return scn::terrainAt(*g.map, x, y); }

bool siteOpen(const Site& s) { return s.content == site::TEMPLE || !s.searched; }

bool explored(const Game& g, int sd, int x, int y) {
  if (g.map->options.hiddenMap == 0) return true;
  for (int tx = x - 1; tx <= x + 1; tx++) {
    for (int ty = y - 1; ty <= y + 1; ty++) {
      if (tx >= 0 && ty >= 0 && tx < g.map->width && ty < g.map->height && game::seen(g, sd, tx, ty)) return true;
    }
  }
  return false;
}

bool flies(const Game& g, const Army* a) { return g.types.byId(a->type)->flies; }
bool magical(const Game& g, const Army* a) { return g.types.byId(a->type)->bonus[48] != 0; }
int ability(const Game& g, const Army* a) { return g.types.byId(a->type)->bonus[52]; }
bool woods(const Game& g, const Army* a) { return g.types.byId(a->type)->woodsMove; }
bool hills(const Game& g, const Army* a) { return g.types.byId(a->type)->hillsMove; }

std::vector<Army*> armies(const Game& g, int sideIndex) {
  std::vector<Army*> out;
  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    Army* a = g.armies[i];
    if (a->owner == sideIndex) out.push_back(a);
  }
  return out;
}

std::vector<Army*> onTile(const Game& g, int sideIndex, int x, int y) {
  std::vector<Army*> out;
  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    Army* a = g.armies[i];
    if (a->owner == sideIndex && !a->transit && a->x == x && a->y == y) out.push_back(a);
  }
  return out;
}

AIData newData() {
  AIData d;
  d.groups[0] = emptyGroup();   // never used: groups count from 1
  for (int i = 1; i <= MAX_GROUPS; i++) d.groups[i] = emptyGroup();
  return d;
}

AIData& data(Game& g, Side& sd) {
  if (!sd.ai) sd.ai = std::make_shared<AIData>(newData());
  return *sd.ai;
}

AIData& data(Game& g, int sideIndex) { return data(g, *g.map->side(sideIndex)); }

// The shared neighbour table (623c:0398): each city's six nearest cities by
// land path, with the path length to each. It depends on the map alone, so
// it is kept per scenario. Not the Lua's: the table holds city indices, so a
// second game of the same map (or a loaded save) reads its own cities rather
// than the first game's.
namespace {
std::mutex neighbourLock;
std::map<std::string, std::shared_ptr<Neighbours>> neighbourCache;
}

std::pair<std::vector<City*>, std::vector<int>> neighbours(Game& g, const City& c) {
  if (!g.aiNeighbours) {
    std::string k = g.map->name;
    for (auto& o : g.map->cities) k += ";" + std::to_string(o.x) + "," + std::to_string(o.y);
    std::shared_ptr<Neighbours> found;
    {
      std::lock_guard<std::mutex> lock(neighbourLock);
      auto it = neighbourCache.find(k);
      if (it != neighbourCache.end()) found = it->second;
    }
    if (!found) {
      found = buildNeighbours(g);
      std::lock_guard<std::mutex> lock(neighbourLock);
      neighbourCache[k] = found;
    }
    g.aiNeighbours = found;
  }
  std::pair<std::vector<City*>, std::vector<int>> out;
  if (c.index >= 0 && c.index < (int)g.aiNeighbours->byCity.size()) {
    const auto& e = g.aiNeighbours->byCity[c.index];
    for (int i : e.cities) out.first.push_back(&g.map->cities[i]);
    out.second = e.dist;
  }
  return out;
}

std::shared_ptr<Neighbours> buildNeighbours(Game& g) {
  auto out = std::make_shared<Neighbours>();
  auto& cities = g.map->cities;
  out->byCity.resize(cities.size());
  for (auto& c : cities) {
    struct Cand { City* city; int d; bool taken = false; };
    auto pick = [&](int range, std::vector<int>& picked, std::vector<int>& dists) {
      Flood flood = floodFrom(g, -1, c.x, c.y, range, move::LAND);
      std::vector<Cand> cand;
      for (auto& o : cities) {
        if (&o != &c) {
          int d = std::get<0>(cityDistance(g, flood, o));
          if (d < 100) cand.push_back(Cand{&o, d});
        }
      }
      luaSort(cand, [](const Cand& p, const Cand& q) { return p.d < q.d; });
      while (cand.size() > 50) cand.pop_back();
      int left = 0, right = 0, up = 0, down = 0;
      picked.clear();
      dists.clear();
      while (picked.size() < 6) {
        int best = -1, bestScore = 0;
        bool any = false;
        for (size_t i = 0; i < cand.size(); i++) {
          Cand& e = cand[i];
          if (!e.taken) {
            int score = e.d;
            if ((e.city->x < c.x && left > 2) || (c.x < e.city->x && right > 2) ||
                (e.city->y < c.y && up > 2) || (c.y < e.city->y && down > 2)) {
              score += 50;
            }
            if (!any || score < bestScore) { best = (int)i; bestScore = score; any = true; }
          }
        }
        if (best < 0) break;
        Cand& e = cand[best];
        e.taken = true;
        picked.push_back(e.city->index);
        dists.push_back(e.d);
        if (e.city->x < c.x) left++;
        if (c.x < e.city->x) right++;
        if (e.city->y < c.y) up++;
        if (c.y < e.city->y) down++;
      }
      return (int)cand.size();
    };
    std::vector<int> picked, dists;
    int found = pick(45, picked, dists);
    if (found == 0) pick(60, picked, dists);
    out->byCity[c.index] = Neighbours::Entry{picked, dists};
  }
  return out;
}

Sel& refresh(Game& g, Sel& sel) {
  int least = 100;
  for (Army* a : sel.armies) least = std::min(least, a->moves);
  sel.minMoves = sel.armies.empty() ? 0 : least;
  sel.mode = sel.armies.empty() ? move::LAND : move::modeOf(g, sel.armies);
  sel.hero = nullptr;
  for (Army* a : sel.armies) if (isHero(a) && !sel.hero) sel.hero = a;
  return sel;
}

Sel select(Game& g, const std::vector<Army*>& list) {
  Sel sel;
  for (Army* a : list) {
    if (a && alive(g, a) && !a->transit) sel.armies.push_back(a);
  }
  sel.leader = sel.armies.empty() ? nullptr : sel.armies[0];
  refresh(g, sel);
  return sel;
}

static int fightRank(const Game& g, const Army* a) {
  const auto& row = g.map->fightOrder[a->owner == NONE ? 8 : a->owner];
  return a->type < (int)row.size() ? row[a->type] : 0;
}

int newGroup(const Game& g, int sideIndex) {
  std::set<int> used;
  for (Army* a : g.armies) {
    if (a->owner == sideIndex && !a->transit && a->group) used.insert(a->group);
  }
  for (int n = 2; n <= 254; n++) if (!used.count(n)) return n;
  return 0;
}

std::optional<Sel> order(Game& g, const std::vector<Army*>& list, int ord, int dest, int flags) {
  Sel sel = select(g, list);
  if (sel.armies.empty()) return std::nullopt;
  Army* best = nullptr;
  for (Army* a : sel.armies) if (!best || fightRank(g, a) > fightRank(g, best)) best = a;
  sel.leader = best;
  int grp = sel.armies.size() > 1 ? newGroup(g, best->owner) : 0;
  for (Army* a : sel.armies) {
    a->aiOrder = ord;
    a->aiDest = dest;
    a->group = grp;
    setTarget(g, a, ord, dest);
    a->aiNeutral = false;
    a->aiExplore = false;
    if (flags) {
      if (has(flags, 0x80)) a->aiNeutral = true;
      if (has(flags, 0x20)) a->aiExplore = true;
      if (has(flags, 0x100)) a->aiParty = true;
    }
  }
  refresh(g, sel);
  return sel;
}

void setTarget(Game& g, Army* a, int ord, int dest) {
  if (ord == ORDER_ROAM) return;
  int x = NONE, y = NONE;
  if (ord == ORDER_CITY && dest != NONE) {
    City* c = g.map->city(dest);
    if (c) {
      x = c->x;
      y = c->y;
      if (c->ownerIndex == a->owner) {
        x++;
        y++;
      } else {
        if (x < a->x) x++;
        if (y < a->y) y++;
      }
    }
  } else if (ord == ORDER_ITEM && dest != NONE) {
    if (dest >= 0 && dest < (int)g.map->items.size()) {
      const Item& it = g.map->items[dest];
      if (it.x != NONE) { x = it.x; y = it.y; }
    }
  } else if (ord == ORDER_SITE && dest != NONE) {
    if (dest >= 0 && dest < (int)g.map->sites.size()) {
      const Site& s = g.map->sites[dest];
      x = s.x;
      y = s.y;
    }
  }
  a->target = std::make_pair(x != NONE ? x : a->x, y != NONE ? y : a->y);
}

Sel selectStackOf(Game& g, Army* a) {
  std::vector<Army*> list;
  for (Army* b : onTile(g, a->owner, a->x, a->y)) {
    if (b == a || (a->group != 0 && b->group == a->group)) {
      list.push_back(b);
      b->aiMoved = true;
    }
  }
  Sel sel = select(g, list);
  Army* best = nullptr;
  for (Army* b : sel.armies) if (!best || fightRank(g, b) > fightRank(g, best)) best = b;
  sel.leader = best ? best : a;
  return sel;
}

std::vector<Army*> collect(const Game& g, int sideIndex, int x, int y, int minMoves) {
  std::vector<Army*> out;
  for (Army* a : onTile(g, sideIndex, x, y)) {
    if ((int)out.size() >= rules::MAX_STACK) break;
    if (!minMoves || a->moves >= minMoves) out.push_back(a);
  }
  return out;
}

std::vector<Army*> collectOrdered(const Game& g, int sideIndex, int x, int y, int grp, int ord, int minMax) {
  std::vector<Army*> out;
  for (Army* a : onTile(g, sideIndex, x, y)) {
    if ((int)out.size() >= rules::MAX_STACK) break;
    if (a->aiGroup == grp && a->aiOrder == ord && (!minMax || a->maxMoves >= minMax)) out.push_back(a);
  }
  return out;
}

// move::DIRS in the order Lua's pairs() walks the table the Lua keeps them in:
// 1..7, then 0. Dijkstra's answer does not depend on it, but it is the same.
static const int FLOOD_DIRS[8] = {1, 2, 3, 4, 5, 6, 7, 0};

static Flood flood(Game& g, int sideIndex, int x, int y, int range, int mode, bool wd, bool hl) {
  int W = g.map->width, H = g.map->height;
  const std::vector<int> grid = move::grid(g, sideIndex);
  int x0 = std::max(0, x - range), x1 = std::min(W - 1, x + range);
  int y0 = std::max(0, y - range), y1 = std::min(H - 1, y + range);
  Flood f;
  f.W = W;
  f.H = H;
  f.dist.assign(W * H, -1);
  std::vector<char> done(W * H, 0);
  int start = y * W + x;
  if (start < 0 || start >= W * H) return f;
  f.dist[start] = 0;
  using E = std::pair<int, int>;   // distance, key
  std::priority_queue<E, std::vector<E>, std::greater<E>> heap;
  heap.push({0, start});
  while (!heap.empty()) {
    int k = heap.top().second;
    heap.pop();
    if (done[k]) continue;
    done[k] = 1;
    int d = f.dist[k];
    int kx = k % W, ky = k / W;
    for (int di : FLOOD_DIRS) {
      int nx = kx + move::DIRS[di][0], ny = ky + move::DIRS[di][1];
      if (nx >= x0 && ny >= y0 && nx <= x1 && ny <= y1) {
        int nk = ny * W + nx;
        if (!done[nk]) {
          auto c = move::stepCost(grid[k], grid[nk], mode, wd, hl, move::WATER_PENALTY);
          if (c && (f.dist[nk] < 0 || d + *c < f.dist[nk])) {
            f.dist[nk] = d + *c;
            heap.push({d + *c, nk});
          }
        }
      }
    }
  }
  return f;
}

Flood floodFrom(Game& g, int sideIndex, int x, int y, int range, const Sel* sel) {
  int mode = sel ? sel->mode : move::LAND;
  bool wd = false, hl = false;
  if (sel) {
    move::Mode m = move::stackMode(g, sel->armies);
    wd = m.woods;
    hl = m.hills;
  }
  return flood(g, sideIndex, x, y, range, mode, wd, hl);
}

Flood floodFrom(Game& g, int sideIndex, int x, int y, int range, int mode) {
  return flood(g, sideIndex, x, y, range, mode, false, false);
}

int floodAt(const Flood& f, int x, int y) {
  if (x < 0 || y < 0 || x >= f.W || y >= f.H) return UNREACHED;
  int d = f.dist[y * f.W + x];
  return d >= 0 ? d + 1 : UNREACHED;
}

// the ring round a city's 2x2 footprint, as 59bf:0a85 walks it
static const int RING[12] = {0, 2, 2, 4, 4, 4, 6, 6, 6, 0, 0, 0};
static const int RDX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
static const int RDY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

std::tuple<int, int, int> cityDistance(const Game& g, const Flood& flood, const City& c, bool far) {
  int best = far ? 1000 : 100;
  int bx = NONE, by = NONE;
  int x = c.x, y = c.y;
  for (int dir : RING) {
    int nx = x + RDX[dir], ny = y + RDY[dir];
    if (nx < 0 || ny < 0 || nx >= 112 || ny >= 156) return {best, bx, by};
    x = nx;
    y = ny;
    int d = floodAt(flood, x, y);
    if (d < best) { best = d; bx = x; by = y; }
  }
  return {best, bx, by};
}

int odds(Game& g, const Sel& sel, int x, int y) {
  if (sel.armies.empty()) return 0;
  int own = sel.armies[0]->owner;
  int n = own != NONE ? data(g, own).sims : 10;
  if (n < 1) n = 1;
  combat::Lines l = combat::lines(g, sel.armies, x, y);
  if (l.defenders.empty()) return 100;
  int wins = 0;
  for (int i = 0; i < n; i++) if (combat::resolve(g, l.attackers, l.defenders, x, y).won) wins++;
  return wins * 100 / n;
}

void pickUp(Game& g, const Sel& sel) {
  if (!sel.hero || !alive(g, sel.hero)) return;
  Army* h = sel.hero;
  City* c = cityAt(g, h->x, h->y);
  for (auto& it : g.map->items) {
    if (it.status == 1 && it.x != NONE && !it.planted) {
      bool here;
      if (c) here = it.x >= c->x && it.x <= c->x + 1 && it.y >= c->y && it.y <= c->y + 1;
      else here = it.x == h->x && it.y == h->y;
      if (here) {
        it.status = 3;
        it.x = NONE;
        it.y = NONE;
        h->items.push_back(&it);
      }
    }
  }
}

namespace {
struct StepResult {
  int code = 0;
  int x = NONE, y = NONE;
};

// Walk the selection towards (tx, ty) (1a8b:0c4f and 1a8b:07f9): 1 no path,
// 2 stopped short, 3 an enemy army is next, 4 the path is walked, 5 an enemy
// city is next; with the tile ahead.
StepResult step(Game& g, Sel& sel, int tx, int ty) {
  Army* lead = sel.leader;
  lead->target = std::make_pair(tx, ty);
  auto path = move::findPath(g, sel.armies, lead->x, lead->y, tx, ty);
  if (!path) return {1};
  if (path->empty()) return {4};
  move::WalkResult r = move::walk(g, sel.armies, *path);
  if (r.steps > 0) walked(g, sel.armies, r);
  refresh(g, sel);
  pickUp(g, sel);
  if (r.stopped == "attack") {
    return {r.attack->city ? 5 : 3, r.attack->x, r.attack->y};
  } else if (r.stopped == "at peace") {
    if (r.steps < (int)path->size()) {
      const move::Step& ahead = (*path)[r.steps];
      if (cityAt(g, ahead.x, ahead.y)) return {5, ahead.x, ahead.y};
    }
    return {2};
  } else if (r.stopped == "arrived") {
    return {4};
  }
  return {2};
}
}  // namespace

Battle fight(Game& g, Sel& sel, int x, int y) {
  // decided, shown, and only then taken effect (after_battle, 67cc:0a6b)
  Battle result = game::decideAttack(g, sel.armies, x, y);
  if (hooks.onFight) hooks.onFight(g, sel.armies, x, y, result);
  game::applyAttack(g, result);
  std::vector<Army*> keep;
  for (Army* a : sel.armies) if (alive(g, a)) keep.push_back(a);
  sel.armies = keep;
  if (!alive(g, sel.leader)) sel.leader = keep.empty() ? nullptr : keep[0];
  refresh(g, sel);
  return result;
}

void done(const Sel& sel) {
  for (Army* a : sel.armies) a->done = true;
}

int moveTo(Game& g, Sel& sel, int tx, int ty, bool keep) {
  if (sel.armies.empty() || tx < 0 || ty < 0 || tx >= g.map->width || ty >= g.map->height) return 0;
  int code = 0;
  bool again = true;
  while (again) {
    again = false;
    if (sel.armies.empty()) break;
    StepResult s = step(g, sel, tx, ty);
    code = s.code;
    int ax = s.x, ay = s.y;
    Army* lead = sel.leader;
    if (lead && lead->target && lead->x == lead->target->first && lead->y == lead->target->second) {
      code = 4;
      lead->target.reset();
    }
    if (code == 2) {
      done(sel);
    } else if (code == 5) {
      City* c = cityAt(g, tx, ty);
      if (c && standing(*c)) {
        City* dest = (lead && lead->aiDest != NONE) ? g.map->city(lead->aiDest) : nullptr;
        if (!dest) dest = c;
        auto [a2, nx, ny] = attackCity(g, sel, dest, ax, ay);
        again = a2;
        if (again) { tx = nx; ty = ny; }
      }
    } else if (code == 4) {
      if (lead && lead->aiOrder == ORDER_SITE && sel.hero) {
        Site* st = siteAt(g, sel.hero->x, sel.hero->y);
        if (st) moves::searchSite(g, &sel, sel.hero, *st);
      }
      for (Army* a : sel.armies) {
        clearOrder(a);
        a->aiGroup = 0;
      }
    } else if (code == 3) {
      fight(g, sel, ax, ay);
      if (!sel.armies.empty()) again = true;
    }
  }
  if (!keep) done(sel);
  return code;
}

std::tuple<bool, int, int> attackCity(Game& g, Sel& sel, City* dest, int ax, int ay) {
  int me = sel.leader->owner;
  AIData& d = data(g, me);
  int own = owner(*dest);
  City* city = cityAt(g, ax, ay);
  if (own == NEUTRAL) {
    fight(g, sel, ax, ay);
    if (city && city->ownerIndex == me) setRole(d, *city, JUST_TAKEN);
  } else if (own != me) {
    City* pick = dest;
    if (d.questCity != dest->index || !sel.hero) pick = retarget(g, sel, dest, ax, ay);
    if (pick != dest) {
      for (Army* a : sel.armies) {
        a->aiOrder = ORDER_CITY;
        a->aiDest = pick->index;
      }
      return {true, pick->x, pick->y};
    }
    int was = city ? owner(*city) : NONE;
    fight(g, sel, ax, ay);
    if (city && city->ownerIndex == me) groups::earlyVengeance(g, me, *city, was);
  }
  for (Army* a : sel.armies) clearOrder(a);
  return {false, NONE, NONE};
}

City* retarget(Game& g, Sel& sel, City* c, int x, int y) {
  int me = sel.leader->owner;
  int od = odds(g, sel, x, y);
  int own = owner(*c);
  bool fair = own == NEUTRAL || state(g, own, me) == 2;
  if (own != NEUTRAL && !isComputer(g, own) && g.rng.dice(1, 4, -1) == 0) fair = true;
  if (!fair || od < 51) {
    Flood f = floodFrom(g, me, sel.leader->x, sel.leader->y, 15, &sel);
    City* best = bestCity(g, sel, f, fair);
    return best ? best : c;
  }
  return c;
}

City* bestCity(Game& g, const Sel& sel, const Flood& flood, bool skipOwn) {
  int me = sel.leader->owner;
  AIData& d = data(g, me);
  City* best = nullptr;
  int bestScore = -1;
  auto& cities = g.map->cities;
  for (int i = (int)cities.size() - 1; i >= 0; i--) {
    City& c = cities[i];
    int own = owner(c);
    if (!(skipOwn && own == me) && standing(c) && d.questCity != c.index && !cflag(d, c, CF_UNSEEN) &&
        (own == NEUTRAL || own == me || state(g, own, me) == 2)) {
      auto [dst, px, py] = cityDistance(g, flood, c);
      (void)py;
      if (px != NONE) {
        int od;
        if (own == me) { od = 15; dst += 20; }
        else od = odds(g, sel, c.x, c.y);
        int score;
        if (od >= 51 && dst < sel.minMoves) score = 400 - dst + od * 10;
        else if (od > 10) score = 100 - dst + od * 5;
        else score = -2;
        if (score > bestScore) { best = &c; bestScore = score; }
      }
    }
  }
  return best;
}

}  // namespace w2::ai::core
