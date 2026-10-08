#include "warlords/move.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <functional>
#include <map>

#include "warlords/armytype.hpp"
#include "warlords/diplomacy.hpp"
#include "warlords/game.hpp"
#include "warlords/rules.hpp"
#include "warlords/scn.hpp"

namespace w2 {

struct Grids {
  std::map<int, std::vector<int>> bySide;
};

namespace move {

const char* const TERRAIN_NAMES[12] = {"road", "bridge", "water", "shore", "forest", "hills",
                                       "mountains", "plain", "marsh", "tower", "city", "site"};

Mode stackMode(const Game& g, const Stack& stack) {
  bool atSea = false, anyBoat = false;
  bool allFly = true, anyFly = false, allNonHeroFly = true, heroFlight = false;
  bool woods = false, hills = false;
  for (Army* a : stack) {
    const ArmyType* t = g.types.byId(a->type);
    if (a->atSea) atSea = true;
    if (t->boat) anyBoat = true;
    if (t->flies && !a->atSea) anyFly = true;
    if (!t->flies || a->atSea) {
      allFly = false;
      if (a->type != armytype::HERO) allNonHeroFly = false;
    }
    if (a->type == armytype::HERO) {
      for (Item* it : a->items) if (it->type == rules::ITEM_FLIGHT) heroFlight = true;
    }
    if (t->woodsMove) woods = true;
    if (t->hillsMove) hills = true;
  }
  if (atSea) return Mode{LAND, false, false, true};
  if (anyBoat) return Mode{BOAT, woods, hills, false};
  if (heroFlight || allFly || (allNonHeroFly && anyFly)) return Mode{FLYING, woods, hills, false};
  return Mode{LAND, woods, hills, false};
}

// 1555:109d walks round a city from its top left tile in these directions
static const int PORT_TOUR[12] = {0, 2, 2, 4, 4, 4, 6, 6, 6, 0, 0, 0};

bool isPort(const Game& g, const City& city) {
  int x = city.x, y = city.y;
  for (int d : PORT_TOUR) {
    x += DIRS[d][0];
    y += DIRS[d][1];
    if (x < 0 || y < 0 || x >= g.map->width || y >= g.map->height) return false;
    int t = scn::terrainAt(*g.map, x, y);
    if (t == BRIDGE || t == WATER || t == SHORE) return true;
  }
  return false;
}

std::vector<int>& grid(Game& g, int sideIndex) {
  if (!g.grids) g.grids = std::make_shared<Grids>();
  int keyS = sideIndex;   // NONE (-1) for nobody, as the Lua's -1
  auto it = g.grids->bySide.find(keyS);
  if (it != g.grids->bySide.end()) return it->second;

  const Map& m = *g.map;
  int W = m.width, H = m.height;
  std::vector<int> out(W * H);
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      int i = y * W + x;
      int terrain = scn::terrainAt(m, x, y);
      int byte = scn::roadAt(m, x, y) % 0x20 != 0 ? 1 : COST[terrain];
      if (terrain == BRIDGE) byte |= CROSS_F | WATER_F;
      else if (terrain == WATER || terrain == SHORE) byte |= WATER_F;
      else if (terrain == FOREST) byte |= FOREST_F;
      else if (terrain == HILLS) byte |= HILLS_F;
      if (m.crossing[i]) byte |= CROSS_F;
      out[i] = byte;
    }
  }
  for (const City& c : m.cities) {
    // NONE is nobody, as the Lua's -1: no city is its own
    bool mine = sideIndex != NONE && c.ownerIndex == sideIndex && !c.razed;
    bool port = (mine || c.razed) && isPort(g, c);
    for (int dx = 0; dx <= 1; dx++) {
      for (int dy = 0; dy <= 1; dy++) {
        int x = c.x + dx, y = c.y + dy;
        if (x < W && y < H) {
          int i = y * W + x;
          int byte = out[i];
          if (c.razed) {
            if (port) byte |= CROSS_F | WATER_F;
          } else {
            byte |= CITY_F;
            if (mine) {
              if (port) byte |= CROSS_F | WATER_F;
            } else {
              byte = byte - (byte % 8);          // cost 0: impassable
            }
          }
          out[i] = byte;
        }
      }
    }
  }
  return g.grids->bySide[keyS] = std::move(out);
}

void invalidate(Game& g) { g.grids.reset(); }

std::optional<int> stepCost(int fromByte, int toByte, int mode, bool woods, bool hills, int penalty) {
  int cost = toByte % 8;
  bool toWater = has(toByte, WATER_F), fromWater = has(fromByte, WATER_F);
  bool toCross = has(toByte, CROSS_F);
  if (mode == FLYING) {
    if (cost == 0 && has(toByte, CITY_F)) return std::nullopt;
    if (toWater && !toCross) return 2;
    if (cost == 0 || cost > 2) return 2;
    return cost;
  }
  if (mode == BOAT) {
    if (!toWater && !has(toByte, CITY_F)) return std::nullopt;
    if (cost == 0) return std::nullopt;
    return cost;
  }
  if (cost == 0) return std::nullopt;
  if (toWater != fromWater && !toCross && !has(fromByte, CROSS_F)) return std::nullopt;
  if (cost > 2) {
    if (woods && has(toByte, FOREST_F)) cost = 2;
    if (hills && has(toByte, HILLS_F)) cost = 2;
  }
  if ((!fromWater || has(fromByte, CROSS_F)) && toWater && !toCross) cost += penalty;
  return cost;
}

int walkCost(int byte, int mode, bool woods, bool hills) {
  int cost = byte % 8;
  if (mode == FLYING) {
    if (has(byte, WATER_F) && !has(byte, CROSS_F)) return 2;
    if (cost == 0 || cost > 2) return 2;
    return cost;
  }
  if (mode == LAND && cost > 2) {
    if (woods && has(byte, FOREST_F)) cost = 2;
    if (hills && has(byte, HILLS_F)) cost = 2;
  }
  return cost;
}

bool crossesShore(int byte, bool atSea) {
  if (has(byte, CROSS_F)) return false;
  return has(byte, WATER_F) != atSea;
}

int stackMoves(const Stack& stack) {
  if (stack.empty()) return 0;
  int least = INT_MAX;
  for (Army* a : stack) least = std::min(least, a->moves);
  return least;
}

std::optional<Preview> preview(Game& g, const Stack& stack, int x, int y) {
  if (stack.empty()) return std::nullopt;
  auto path = findPath(g, stack, stack[0]->x, stack[0]->y, x, y);
  if (!path || path->empty()) return std::nullopt;
  int left = stackMoves(stack), reach = 0;
  for (size_t i = 0; i < path->size(); i++) {
    const Step& step = (*path)[i];
    if (left < MIN_MOVE_LEFT || step.cost > left) break;
    left -= step.cost;
    reach = (int)i + 1;
  }
  return Preview{*path, reach};
}

// The search, as 1555:000a runs it: UNREACHED until the flood gets there,
// SHUT where the stack may never go, and then the distance from the
// destination -- negative while the tile still has to spread it further.
static const int UNREACHED = 30000;

// Where the wavefront spreads from a tile, by where the tile lies: inside,
// or along one of the map's edges or corners (4125:00e2, 00f6, 0162)
static const int NB_FIRST[10] = {0, 9, 13, 19, 23, 29, 35, 39, 45, 49};
static const int NB_DX[] = {-1, 0, 1, 1, 1, 0, -1, -1, -10, 1, 1, 0, -10, 1, 1, 0, -1, -1,
                            -10, 0, -1, -1, -10, 0, 1, 1, 1, 0, -10, -1, 0, 0, -1, -1, -10, 0, 1, 1, -10,
                            -1, 0, 1, 1, -1, -10, -1, 0, -1, -10, 0, 1, 0, -1, -10};
static const int NB_DY[] = {-1, -1, -1, 0, 1, 1, 1, 0, -10, 0, 1, 1, -10, 0, 1, 1, 1, 0,
                            -10, 1, 1, 0, -10, -1, -1, 0, 1, 1, -10, -1, -1, 1, 1, 0, -10, -1, -1, 0, -10,
                            -1, -1, -1, 0, 0, -10, -1, -1, 0, -10, -1, 0, 1, 0, -10};

// the order path_trace tries the neighbours in (4125:0212)
static const int TRACE_ORDER[8] = {0, 7, 1, 6, 2, 5, 3, 4};

int direction(int x1, int y1, int x2, int y2) {
  if (x1 == x2 && y1 == y2) return -1;
  if (x1 == x2) return y2 < y1 ? 0 : 4;
  if (y1 == y2) return x2 < x1 ? 6 : 2;
  if (x1 <= x2) return y2 < y1 ? 1 : 3;
  return y2 < y1 ? 7 : 5;
}

int distance(int x1, int y1, int x2, int y2) {
  double ax = x1 - x2, ay = y1 - y2;
  return (int)std::floor(std::sqrt(ax * ax + ay * ay));
}

std::vector<int> prepare(Game& g, const std::vector<int>& grid, int sideIndex, int mode, int dx, int dy) {
  int W = g.map->width, H = g.map->height;
  Side* side = g.map->side(sideIndex);
  bool fog = g.map->options.hiddenMap != 0 && side && !side->computer;
  std::vector<int> dist(W * H);
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      int k = y * W + x;
      int b = grid[k];
      bool hidden = fog && !game::seen(g, sideIndex, x, y);
      bool shut;
      if (mode == LAND) shut = b % 8 == 0 || (hidden && !(x == dx && y == dy));
      else if (mode == BOAT) shut = !has(b, WATER_F) || (hidden && x != dx && y != dy);
      else shut = (b % 8 == 0 && has(b, CITY_F)) || (hidden && x != dx && y != dy);
      dist[k] = shut ? SHUT : UNREACHED;
    }
  }
  return dist;
}

namespace {
struct Query {
  int W, H, sx, sy, dx, dy;
  std::vector<int>* grid;
  int mode;
  bool woods, hills;
  int penalty;
  std::vector<int> dist;
  int ring = 0;
  // the road builder (roadRoute): straight spreading, its own trace
  bool roads = false;
  std::function<bool(int, int)> isBridge;
};

// path_wavefront (1555:0373): sweeps squares ever wider round the
// destination; 6 tiles of margin on the first pass, 50 on the second.
int wavefront(Query& q, int pass) {
  int W = q.W, H = q.H;
  int sx = q.sx, sy = q.sy, dx = q.dx, dy = q.dy;
  std::vector<int>& dist = q.dist;
  const std::vector<int>& grid = *q.grid;
  int mode = q.mode;
  int margin = pass == 0 ? 6 : 50;
  if (pass == 0) q.ring = 0;
  int lox = std::min(sx, dx), loy = std::min(sy, dy);
  int hix = std::max(sx, dx), hiy = std::max(sy, dy);
  int bx0 = lox < margin ? 0 : lox - margin;
  int by0 = loy < margin ? 0 : loy - margin;
  int bx1 = hix + margin < W ? hix + margin : W - 1;
  int by1 = hiy + margin < H ? hiy + margin : H - 1;
  int found = 1;
  bool going = true;
  while (going) {
    bool any = false;
    int r = q.ring;
    int x0 = std::max({dx - r, 0, bx0}), x1 = std::min({dx + r, W - 1, bx1});
    int y0 = std::max({dy - r, 0, by0}), y1 = std::min({dy + r, H - 1, by1});
    for (int x = x0; x <= x1; x++) {
      for (int y = y0; y <= y1; y++) {
        int k = y * W + x;
        int v = dist[k];
        if (v < 1) {
          any = true;
          int cur = grid[k];
          int nd = -v;
          dist[k] = nd;
          bool curWater = has(cur, WATER_F), curCross = has(cur, CROSS_F);
          // the road builder spreads straight only (4125:00e2, class 9)
          int cls = q.roads ? 9 : 0;
          if (y == 0) cls = x == 0 ? 1 : (x == W - 1 ? 3 : 2);
          else if (y == H - 1) cls = x == 0 ? 6 : (x == W - 1 ? 8 : 7);
          else if (x == 0) cls = 4;
          else if (x == W - 1) cls = 5;
          int i = NB_FIRST[cls];
          while (NB_DX[i] != -10) {
            int nx = x + NB_DX[i], ny = y + NB_DY[i];
            i++;
            int nk = ny * W + nx;
            int nv = dist[nk];
            if (nv != SHUT) {
              int nb = grid[nk];
              int c = nb % 8;
              bool ok = true;
              bool nWater = has(nb, WATER_F), nCross = has(nb, CROSS_F);
              if (nx == sx && ny == sy) {
                if (mode == LAND && !curCross && !nCross && nWater != curWater) ok = false;
                c = 1;
              } else if (mode == FLYING) {
                if ((!nWater || nCross) && c != 0 && c < 3) {
                  // as dear as the tile, where that is 1 or 2
                } else {
                  c = 2;
                }
              } else if (mode == BOAT) {
                if (!nWater && !has(nb, CITY_F)) {
                  dist[nk] = SHUT;
                  ok = false;
                }
              } else {
                if (!curCross && !nCross && nWater != curWater) {
                  ok = false;
                } else {
                  if (c > 2 && ((q.hills && has(nb, HILLS_F)) || (q.woods && has(nb, FOREST_F)))) c = 2;
                  if ((!curWater || curCross) && nWater && !nCross) c += q.penalty;
                }
              }
              if (ok && nd + c < std::abs(nv)) dist[nk] = v - c;
            }
          }
          if (x == sx && y == sy) going = false;
        }
      }
    }
    q.ring = r + 1;
    if (!any) { going = false; found = 0; }
  }
  return found;
}

// path_trace (1555:117b): from the start, each step to whichever neighbour
// the flood put nearest the destination; at most 198 steps.
Path trace(Query& q) {
  int W = q.W, H = q.H;
  const std::vector<int>& dist = q.dist;
  const std::vector<int>& grid = *q.grid;
  int x = q.sx, y = q.sy;
  int tx = q.dx, ty = q.dy;
  int d = std::abs(dist[y * W + x]);
  Path steps;
  while (!(x == tx && y == ty)) {
    int cur = grid[y * W + x];
    bool land = q.mode == LAND;
    bool curWater = land && has(cur, WATER_F);
    bool curCross = land && has(cur, CROSS_F);
    int toward = direction(x, y, tx, ty);
    int bx = NONE, by = NONE;
    bool got = false;
    for (int turn : TRACE_ORDER) {
      int dir = (toward + turn) % 8;
      int nx = x + DIRS[dir][0], ny = y + DIRS[dir][1];
      bool inside = nx >= 0 && ny >= 0 && nx < W && ny < H;
      // the road builder steps diagonally only over water that is no bridge
      if (q.roads && dir % 2 == 1 && !(inside && has(grid[ny * W + nx], WATER_F) && !q.isBridge(nx, ny))) continue;
      if (inside) {
        int v = dist[ny * W + nx];
        if (v != UNREACHED && v != SHUT) {
          int nb = grid[ny * W + nx];
          bool blocked = land && !curCross && !has(nb, CROSS_F) && has(nb, WATER_F) != curWater;
          // ... and takes a first neighbour no nearer than here
          bool tie = q.roads && !got && std::abs(v) == d;
          if (!blocked && (std::abs(v) < d || tie)) {
            bx = nx; by = ny; got = true;
            d = std::abs(v);
          }
        }
      }
    }
    if (got && steps.size() < 198) {
      x = bx; y = by;
      steps.push_back(Step{x, y, 0});
    } else {
      tx = x; ty = y;
    }
  }
  return steps;
}

}  // namespace

std::optional<std::vector<std::pair<int, int>>> roadRoute(const RoadGround& m, const int costs[12], int W, int H,
                                                          int sx, int sy, int dx, int dy) {
  if (dx < 0 || dy < 0 || dx >= W || dy >= H) return std::nullopt;
  std::vector<int> grd(W * H);
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      int t = m.terrain(x, y);
      int byte = m.road(x, y) ? 1 : costs[t];
      if (t == BRIDGE) byte |= CROSS_F | WATER_F;
      else if (t == WATER || t == SHORE) byte |= WATER_F;
      else if (t == FOREST) byte |= FOREST_F;
      else if (t == HILLS) byte |= HILLS_F;
      else if (t == CITY) byte = CITY_F;          // nobody's city is player 14's
      if (m.crossing(x, y)) byte |= CROSS_F;
      grd[y * W + x] = byte;
    }
  }
  int to = m.terrain(dx, dy);
  Query q;
  q.W = W; q.H = H; q.sx = sx; q.sy = sy; q.dx = dx; q.dy = dy;
  q.grid = &grd;
  q.mode = LAND;
  q.woods = q.hills = false;
  q.penalty = to == WATER || to == SHORE ? WATER_PENALTY_TO : WATER_PENALTY;
  q.dist.resize(W * H);
  for (int k = 0; k < W * H; k++) q.dist[k] = grd[k] % 8 == 0 ? SHUT : UNREACHED;
  q.roads = true;
  q.isBridge = [&m](int x, int y) { return m.terrain(x, y) == BRIDGE; };
  int goal = dy * W + dx;
  if (q.dist[goal] == SHUT && !has(grd[goal], CITY_F)) return std::nullopt;
  q.dist[goal] = -1;
  if (wavefront(q, 0) != 1) return std::nullopt;
  std::vector<std::pair<int, int>> out;
  for (const Step& st : trace(q)) out.emplace_back(st.x, st.y);
  return out;
}

namespace {

// path_single_step (1555:020e): a destination one tile away is simply stepped
// to, unless a land or boat stack would cross between land and water, or the
// ground there is impassable.
bool singleStep(const Game& g, int mode, int sx, int sy, int dx, int dy) {
  auto wet = [](int t) { return t == WATER || t == SHORE; };
  int from = scn::terrainAt(*g.map, sx, sy), to = scn::terrainAt(*g.map, dx, dy);
  if (mode != FLYING) {
    if (wet(from) != wet(to)) return false;
    if (COST[to] == 0) return false;
  }
  return true;
}
}  // namespace

std::optional<Path> findPath(Game& g, const Stack& stack, int sx, int sy, int dx, int dy) {
  if (sx == dx && sy == dy) return Path{};
  int W = g.map->width, H = g.map->height;
  if (dx < 0 || dy < 0 || dx >= W || dy >= H) return std::nullopt;

  int side = stack.empty() ? NONE : stack[0]->owner;
  Mode m = stackMode(g, stack);
  std::vector<int>& grd = grid(g, side);

  std::optional<Path> path;
  std::vector<std::pair<int, int>> restore;
  if (distance(dx, dy, sx, sy) == 1 && singleStep(g, m.mode, sx, sy, dx, dy)) {
    path = Path{Step{dx, dy, 0}};
  } else {
    // a stack may always path *to* a city, just never through one it does
    // not own (path_mark_cities, 1555:0bde)
    int goal = dy * W + dx;
    int goalByte = grd[goal];
    City* city = g.map->cityTile[goal];
    if (city && goalByte % 8 == 0 && has(goalByte, CITY_F)) {
      bool port = isPort(g, *city);
      for (int ox = 0; ox <= 1; ox++) {
        for (int oy = 0; oy <= 1; oy++) {
          int x = city->x + ox, y = city->y + oy;
          if (x < W && y < H) {
            int k = y * W + x;
            restore.emplace_back(k, grd[k]);
            int byte = grd[k] - grd[k] % 8 + COST[CITY];
            if (port) byte |= CROSS_F | WATER_F;
            grd[k] = byte;
          }
        }
      }
    }
    int penalty = 0;
    if (m.mode == LAND) {
      int t = scn::terrainAt(*g.map, dx, dy);
      penalty = (t == WATER || t == SHORE) ? WATER_PENALTY_TO : WATER_PENALTY;
    }
    Query q{W, H, sx, sy, dx, dy, &grd, m.mode, m.woods, m.hills, penalty,
            prepare(g, grd, side, m.mode, dx, dy), 0, false, nullptr};
    int found = 0;
    if (q.dist[goal] != SHUT || has(grd[goal], CITY_F)) {
      q.dist[goal] = -1;
      for (int pass = 0; pass <= 1; pass++) {
        if (found == 0) found = wavefront(q, pass);
      }
    }
    if (found == 1) path = trace(q);
  }

  // what the walk spends on each step (path_step_costs, 1555:18be)
  if (path) {
    bool ashore = false;
    for (Step& step : *path) {
      int byte = grd[step.y * W + step.x];
      if (ashore) {
        step.cost = PAST_SHORE;
      } else {
        step.cost = walkCost(byte, m.mode, m.woods, m.hills);
        ashore = m.mode == LAND && crossesShore(byte, m.atSea);
      }
    }
  }
  for (auto& [k, byte] : restore) grd[k] = byte;
  if (path && path->empty()) path.reset();
  return path;
}

std::string settleSea(Game& g, const Stack& stack, int x, int y, int mode, bool wasAtSea) {
  if (mode != LAND) return "";
  int t = scn::terrainAt(*g.map, x, y);
  if (!wasAtSea) {
    if (t != WATER && t != SHORE) return "";
    bool flier = false, walker = false;
    for (Army* a : stack) {
      if (g.types.byId(a->type)->flies) flier = true;
      else if (a->type != armytype::HERO) walker = true;
    }
    if (walker) flier = false;
    std::string change;
    for (Army* a : stack) {
      if (!g.types.byId(a->type)->flies && (a->type != armytype::HERO || !flier)) {
        a->atSea = true;
        change = "to sea";
      }
    }
    return change;
  } else if (t != WATER && t != SHORE && t != BRIDGE) {
    for (Army* a : stack) a->atSea = false;
    return "ashore";
  }
  return "";
}

namespace {
// Walk `stack` along `path`, stopping where the rules say to stop.
WalkResult walkPath(Game& g, const Stack& stack, const Path& path) {
  int side = stack.empty() ? NONE : stack[0]->owner;
  int left = stackMoves(stack);
  WalkResult result;
  result.stopped = "arrived";
  int lastOk = 0, cumulative = 0;
  std::map<int, int> costTo;

  for (int i = 1; i <= (int)path.size(); i++) {
    const Step& step = path[i - 1];
    if (left < MIN_MOVE_LEFT || step.cost > left) {
      result.stopped = "out of moves";
      break;
    }
    City* city = g.map->cityTile[step.y * g.map->width + step.x];
    auto here = game::armiesAt(g, step.x, step.y);
    if (city && !city->razed && city->ownerIndex != side) {
      if (!diplomacy::mayAttack(g, side, city->ownerIndex)) {
        result.stopped = "at peace";
        break;
      }
      result.stopped = "attack";
      result.attack = Attack{step.x, step.y, city};
      break;
    }
    bool theirs = !here.empty() && here[0]->owner != side;
    if (theirs && diplomacy::mayAttack(g, side, here[0]->owner)) {
      result.stopped = "attack";
      result.attack = Attack{step.x, step.y, nullptr};
      break;
    }
    left -= step.cost;
    cumulative += step.cost;
    // the stack may only stop on a tile of its own where it fits; other
    // steps -- a side at peace's stack among them -- are passed over
    // (1a8b:07f9)
    if (!theirs && (int)(here.size() + stack.size()) <= rules::MAX_STACK) {
      lastOk = i;
      costTo[i] = cumulative;
    }
  }

  if (lastOk == 0) {
    if (result.stopped == "arrived") result.stopped = "blocked";
    return result;
  }

  int cost = costTo[lastOk];
  const Step& dest = path[lastOk - 1];
  Mode m = stackMode(g, stack);
  for (Army* a : stack) {
    a->x = dest.x;
    a->y = dest.y;
    a->moves = std::max(0, a->moves - cost);
  }
  std::string change = settleSea(g, stack, dest.x, dest.y, m.mode, m.atSea);
  if (!change.empty()) {
    for (Army* a : stack) {
      if (a->atSea == (change == "to sea")) a->moves = 0;
    }
    result.stopped = "out of moves";
    result.attack.reset();
  }

  // walking uncovers the map as it goes
  if (g.map->options.hiddenMap != 0 && side != NONE) {
    bool flying = modeOf(g, stack) == FLYING;
    int found = 0;
    for (int i = 0; i < (int)path.size(); i++) {
      if (i == lastOk - 1) break;
      found += game::reveal(g, side, path[i].x, path[i].y, flying);
    }
    found += game::reveal(g, side, dest.x, dest.y, flying);
    if (found > 0) invalidate(g);
  }

  result.steps = lastOk;
  result.spent = cost;
  for (int i = 0; i < lastOk; i++) result.walked.emplace_back(path[i].x, path[i].y);
  if (lastOk < (int)path.size() && result.stopped == "arrived") result.stopped = "blocked";
  return result;
}
}  // namespace

WalkResult walk(Game& g, const Stack& stack, const Path& path) {
  game::tidyTowers(g);
  WalkResult result = walkPath(g, stack, path);
  game::tidyTowers(g);
  return result;
}

WalkResult moveTo(Game& g, const Stack& stack, int x, int y) {
  WalkResult r;
  if (stack.empty()) { r.stopped = "blocked"; return r; }
  auto path = findPath(g, stack, stack[0]->x, stack[0]->y, x, y);
  if (!path) { r.stopped = "no route"; return r; }
  if (path->empty()) { r.stopped = "arrived"; return r; }
  return walk(g, stack, *path);
}

}  // namespace move
}  // namespace w2
