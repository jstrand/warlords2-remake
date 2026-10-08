// The random map generator: see randommap.hpp and docs/re/random_map.md.
//
// RANDOM\RANDOM.DAT is both the generator's parameters and its memory: the
// original reads the whole 0xa560 bytes into one buffer and works in it, the
// 112x156 grid of terrain types at +0x6120 included. This does the same, so
// the offsets below are RANDOM.DAT's own, and the few reads the original makes
// off the edge of the grid land where they did. Writes off the grid are
// dropped; the original's would corrupt its own tables.
//
// Every roll is its own statement: C++ leaves the order of a call's arguments
// and of an operator's operands open, and the dice must fall in the order the
// web port's do.
#include "warlords/randommap.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

#include "util/util.hpp"
#include "warlords/armytype.hpp"
#include "warlords/bytes.hpp"
#include "warlords/move.hpp"
#include "warlords/types.hpp"

namespace w2::randommap {

const char* const FILES[6] = {"SCN", "MAP", "RD", "SGN", "CTY", "SPC"};
const int SLIDER_SHOWS[4][7] = {{5, 7, 9, 11, 13, 15, 17}, {5, 7, 9, 11, 13, 15, 17},
                                {70, 75, 80, 85, 90, 95, 100}, {9, 11, 13, 15, 17, 19, 21}};
const char* const SLIDER_FORMATS[4] = {"(%d%%)", "(%d%%)", "(%d)", "(%d%%)"};

namespace {

constexpr int W = 112, H = 156;

// terrain type ids, as the grid and the scenario's tile table number them
constexpr int ROAD = 0, BRIDGE = 1, WATER = 2, SHORE = 3, FOREST = 4, HILLS = 5;
constexpr int MOUNTAINS = 6, PLAIN = 7, MARSH = 8, TOWER = 9, CITY = 10, SITE = 11;

// RANDOM.DAT: the parameters (the sliders add to these)
constexpr int P_CITIES = 0x2a, P_EDGE_POINTS = 0x2c;
constexpr int P_MOUNTAINS = 0x34, P_HILLS = 0x36, P_WIDE_RIVERS = 0x38, P_RIVERS = 0x3a;
constexpr int P_FOREST = 0x3c, P_PASSES = 0x3e, P_EROSION = 0x40;
constexpr int SLIDER_HILLS = 0x44, SLIDER_WATER = 0x52, SLIDER_FOREST = 0x60, SLIDER_CITIES = 0x6e;
constexpr int CORNERS = 0x7c, EDGES = 0x8c, DIAGONALS = 0xac;
constexpr int NB = 0xbc;          // the 8 neighbours, S first and anticlockwise
constexpr int NB_ROAD = 0xdc;     // the 8 again, N first and clockwise: road shapes
// RANDOM.DAT: the generator's working space and its tables
constexpr int EDGE_COUNT = 0xfc, EDGE_PTS = 0x104, EDGE_START = 0x244, EDGE_END = 0x254;
constexpr int SHAPE = 0x264;      // neighbour mask -> tile variant, -1 for none
constexpr int ROAD_SHAPE = 0x364; // neighbour mask -> road id - 1, -1 for none
constexpr int TILES = 0x464;      // per terrain type, 16 variants x 2 checkerboard tiles
constexpr int SIDE_NAMES = 0x764, SITE_FORMATS = 0xed2;
constexpr int SIGN_CITY = 0x1888, SIGN_LEAGUES = 0x189c, SIGNS = 0x18b0;
constexpr int SIDE_CLASS = 0x1d62, REGION_CLASS = 0x1d72, PRODUCTION = 0x1d82;
constexpr int FOREST_DONE = 0x1f52, FOREST_WANTED = 0x1f54;
constexpr int NODES = 0x5dd6, NODE_COUNT = 0x611e, GRID = 0x6120;
constexpr int DAT_SIZE = 0xa560;

// .SCN offsets (docs/formats/scenario.md)
constexpr int S_SIDE_REC = 387, S_RANDOM = 0x120, S_TERRAIN_SET = 0x161;
constexpr int S_TERRAIN = 0x710, S_SITE_COUNT = 0x80f, S_SITES = 0x811, SITE_STRIDE = 31;
constexpr int S_CITY_COUNT = 5499, S_CITIES = 5501, CITY_STRIDE = 65;
constexpr int N_SITES = 40;

// directions the signposts give (4125:2b7c)
const char* const COMPASS[8] = {"north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"};
// the twelve tiles round a city's 2x2 footprint, where its roads start (DS:0382)
const int ROUND_CITY[12][2] = {{-1, -1}, {0, -1}, {1, -1}, {2, -1}, {2, 0}, {2, 1}, {2, 2}, {1, 2},
                               {0, 2}, {-1, 2}, {-1, 1}, {-1, 0}};
// the road builder's terrain costs, pseudo-player 14 (DS:01e0)
const int ROAD_COST[12] = {1, 1, 3, 3, 4, 5, 7, 2, 5, 2, 1, 7};
const int FOOTPRINT[4][2] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};   // DS:02c4, 02cc
constexpr int SAFETY = 200000;    // a loop the original might never leave

struct P { int x = 0, y = 0; };

/** Chebyshev distance (4c49:0fed). */
int reach(int x1, int y1, int x2, int y2) { return std::max(std::abs(x1 - x2), std::abs(y1 - y2)); }

/** map_distance (1a8b:0acc): the straight line, rounded down. */
int distance(int x1, int y1, int x2, int y2) {
  double dx = x1 - x2, dy = y1 - y2;
  return (int)std::floor(std::sqrt(dx * dx + dy * dy));
}

/** 4c49:0dd0: the step from one tile towards another, one of 8 compass
 *  directions by the slope, cut at tan 22.5 and tan 67.5 (4125:0292). */
P step(int x1, int y1, int x2, int y2) {
  if (x2 == x1) return P{0, y2 == y1 ? 0 : (y2 < y1 ? -1 : 1)};
  if (y2 == y1) return P{x2 < x1 ? -1 : 1, 0};
  double s = (double)(y2 - y1) / (double)(x2 - x1);
  if (x2 < x1) {
    if (s >= 2.414) return P{0, -1};
    if (s >= 0.414) return P{-1, -1};
    if (s >= -0.414) return P{-1, 0};
    if (s >= -2.414) return P{-1, 1};
    return P{0, 1};
  }
  if (s >= 2.414) return P{0, 1};
  if (s >= 0.414) return P{1, 1};
  if (s >= -0.414) return P{1, 0};
  if (s >= -2.414) return P{1, -1};
  return P{0, -1};
}

/** The direction from one tile to another for a signpost (828e:0b51). */
int compass(int x1, int y1, int x2, int y2) {
  if (x1 == x2) return y1 < y2 ? 4 : 0;
  if (y1 == y2) return x1 < x2 ? 2 : 6;
  if (x2 < x1 && y2 < y1) return 7;
  if (x2 < x1 && y1 < y2) return 5;
  if (x1 < x2 && y2 < y1) return 1;
  return 3;
}

/** The checkerboard: 1 where x + y is even (C's (n)/2 != (n-1)/2). */
int parity(int x, int y) {
  int n = x + y;
  return n == 0 || n / 2 != (n - 1) / 2 ? 1 : 0;
}

void clamp(P& p) {                      // 4bed:0470
  if (p.x < 1) p.x = 0;
  if (p.y < 1) p.y = 0;
  if (p.x > W - 1) p.x = W - 1;
  if (p.y > H - 1) p.y = H - 1;
}

int i8(int b) { return b > 127 ? b - 256 : b; }

int getI16(const std::vector<uint8_t>& t, int o) { return (int16_t)(t[o] | (t[o + 1] << 8)); }
void setI16(std::vector<uint8_t>& t, int o, int v) {
  t[o] = (uint8_t)(v & 0xff);
  t[o + 1] = (uint8_t)((v >> 8) & 0xff);
}
std::string cstrOf(const std::vector<uint8_t>& t, int o, int n) {
  std::string s;
  for (int i = o; i < o + n && i < (int)t.size(); i++) {
    if (t[i] == 0) break;
    s += (char)t[i];
  }
  return s;
}

struct SpotList {
  int x[50], y[50], ew[50], cityDist[50], chosen[50], chosenDist[50];
};

struct Surroundings { bool shore, forest, road, hills, marsh; };

}  // namespace

class Generator {
 public:
  std::vector<uint8_t> m, scn;
  Types types;
  Rng* rng;
  bool allies;
  std::vector<uint16_t> tiles;
  std::vector<uint8_t> rd;
  std::string cty, spc, sgn;
  int siteCell = -1;               // 4125:02d4, where the sites' round begins

  // the phases, as the original's progress bar moves through them
  int stage = 0;
  int siteI = 0, tenth = 0;
  std::vector<uint8_t> served;
  int shown = 4;
  std::optional<std::pair<int, int>> pending;

  // --- memory -------------------------------------------------------------

  int r16(int o) const { return getI16(m, o); }
  void w16(int o, int v) { setI16(m, o, v); }
  std::string str(int o, int n) const { return cstrOf(m, o, n); }
  int s16(int o) const { return getI16(scn, o); }
  void ws16(int o, int v) { setI16(scn, o, v); }
  void wstr(int o, const std::string& s) {     // strcpy into the scenario image
    for (size_t i = 0; i < s.size(); i++) scn[o + i] = (uint8_t)s[i];
    scn[o + s.size()] = 0;
  }

  /** The terrain grid. Off the grid a read gets whatever RANDOM.DAT holds
   *  there, as the original's did. */
  int at(int x, int y) const {
    int o = GRID + y * W + x;
    return o >= 0 && o < DAT_SIZE ? m[o] : 0;
  }
  void put(int x, int y, int t) {
    int i = y * W + x;
    if (i >= 0 && i < W * H) m[GRID + i] = (uint8_t)t;
  }
  int nx(int i) const { return r16(NB + 4 * i); }
  int ny(int i) const { return r16(NB + 4 * i + 2); }
  /** The index of a step among the 8 neighbours, or -1. */
  int nbIndex(int dx, int dy) const {
    int k = -1;
    for (int i = 0; i < 8; i++) if (nx(i) == dx && ny(i) == dy) k = i;
    return k;
  }

  // the tile map: the low byte is the tile, bit 15 a crossing
  int tile(int x, int y) const { return x >= 0 && y >= 0 && x < W && y < H ? tiles[y * W + x] & 0xff : 0; }
  void setTile(int x, int y, int t) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    uint16_t& v = tiles[y * W + x];
    v = (uint16_t)((v & 0xff00) | (t & 0xff));
  }
  int road(int x, int y) const { return x >= 0 && y >= 0 && x < W && y < H ? rd[y * W + x] & 0x1f : 0; }
  void setRoad(int x, int y, int r) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    uint8_t& v = rd[y * W + x];
    v = (uint8_t)((v & 0xe0) | (r & 0x1f));
  }
  int terrainOf(int t) const { return scn[S_TERRAIN + t]; }
  /** Variant `v` of terrain type `t` from RANDOM.DAT's tile table, the
   *  checkerboard picking one of its pair. */
  int tileFor(int t, int v, int x, int y) const { return r16(TILES + 64 * t + 4 * v + 2 * parity(x, y)) & 0xff; }

  // the cities, as the scenario holds them
  int cityCount() const { return s16(S_CITY_COUNT); }
  void setCityCount(int n) { ws16(S_CITY_COUNT, n); }
  int crec(int i) const { return S_CITIES + CITY_STRIDE * i; }
  int cx(int i) const { return s16(crec(i)); }
  int cy(int i) const { return s16(crec(i) + 2); }

  /** dice(n, sides, bonus) (6ecb:02bf): no sides gives the bonus, and the
   *  clamp makes fewer than none give n + bonus. */
  int dice(int n, int sides, int bonus = 0) {
    if (sides == 0) return bonus;
    if (sides < 0) return n + bonus;
    return rng->dice(n, sides, bonus);
  }

  /** terrain_near (4c49:11f5): is type `t` on any of the 8 tiles round. */
  bool near(int x, int y, int t) const {
    for (int i = 0; i < 8; i++) {
      int ax = x + nx(i), ay = y + ny(i);
      if (ax >= 0 && ay >= 0 && ax < W && ay < H && at(ax, ay) == t) return true;
    }
    return false;
  }
  bool nearPlain(int x, int y) const { return near(x, y, PLAIN); }

  /** 4bed:04ae: push a point by (1d3-2) x 1dn each way; with `toEdge`, 30%
   *  of the time it then goes to the nearest edge. Clamped to the map. */
  void jitter(P& p, int n, bool toEdge) {
    int s1 = dice(1, 3, -2);
    p.x += s1 * dice(1, n, 0);
    int s2 = dice(1, 3, -2);
    p.y += s2 * dice(1, n, 0);
    if (dice(1, 100, 0) < 30 && toEdge) {
      int l = p.x, t = p.y, r = W - 1 - p.x, b = H - 1 - p.y;
      if (l <= t && l <= r && l <= b) p.x = 0;
      if (t <= l && t <= r && t <= b) p.y = 0;
      if (r <= t && r <= l && r <= b) p.x = W - 1;
      if (b <= t && b <= l && b <= r) p.y = H - 1;
    }
    clamp(p);
  }

  // --- 4c49: the coastline --------------------------------------------------

  P pt(int o) const { return P{r16(o), r16(o + 2)}; }
  void setPt(int o, P p) { w16(o, p.x); w16(o + 2, p.y); }

  /** 4c49:0087: where edge `i`'s stretch of coast begins and ends -- the
   *  map's corners drawn in 10 along the diagonal, then pushed about. */
  void coastEnds(int i) {
    int j = (i + 1) % 4;
    if (r16(P_EDGE_POINTS + 2 * i) < 3) {
      setPt(EDGE_START + 4 * i, pt(EDGES + 8 * i));
      setPt(EDGE_END + 4 * i, pt(EDGES + 8 * i + 4));
      return;
    }
    P a = pt(EDGES + 8 * i);
    a.x -= 10 * r16(DIAGONALS + 4 * i); a.y -= 10 * r16(DIAGONALS + 4 * i + 2);
    jitter(a, 16, true);
    put(a.x, a.y, PLAIN);
    setPt(EDGE_START + 4 * i, a);
    P b = pt(EDGES + 8 * i + 4);
    b.x -= 10 * r16(DIAGONALS + 4 * j); b.y -= 10 * r16(DIAGONALS + 4 * j + 2);
    jitter(b, 16, true);
    put(b.x, b.y, PLAIN);
    setPt(EDGE_END + 4 * i, b);
  }

  /** 4c49:0206: swap an edge's end with the next one's start where that
   *  keeps the outline turning round the middle. */
  void coastOrder() {
    for (int i = 0; i < 4; i++) {
      int j = (i + 1) % 4;
      P e = pt(EDGE_END + 4 * i), s = pt(EDGE_START + 4 * j);
      if ((double)(56 - e.x) / (double)(78 - e.y) < (double)(56 - s.x) / (double)(78 - s.y)) {
        setPt(EDGE_END + 4 * i, s);
        setPt(EDGE_START + 4 * j, e);
      }
    }
  }

  static bool onEdge(P p) { return p.x == 0 || p.y == 0 || p.x == W - 1 || p.y == H - 1; }

  /** 4c49:02f9: the points along edge `i`, each a step on from the last
   *  towards the end and pushed about by up to half a step. */
  void coastPoints(int i) {
    int base = EDGE_PTS + 0x50 * i, want = r16(P_EDGE_POINTS + 2 * i);
    P s = pt(EDGE_START + 4 * i), e = pt(EDGE_END + 4 * i);
    setPt(base, s);
    int n = 1;
    if (want < 3 || (onEdge(s) && onEdge(e))) {
      setPt(base + 4, e);
      w16(EDGE_COUNT + 2 * i, 2);
      return;
    }
    int len = reach(e.x, e.y, s.x, s.y) / (want - 1);
    int k = 0;
    for (;;) {
      P p = pt(base + 4 * k);
      P d = step(p.x, p.y, e.x, e.y);
      P q{p.x + d.x * len, p.y + d.y * len};
      jitter(q, len / 2, false);
      k++;
      put(q.x, q.y, PLAIN);
      setPt(base + 4 * k, q);
      n++;
      if (reach(e.x, e.y, q.x, q.y) < len || n >= want - 1) break;
    }
    setPt(base + 4 * (k + 1), e);
    w16(EDGE_COUNT + 2 * i, n + 1);
  }

  /** 4c49:104a: turn a step by -1 to +3 eighths. */
  P turn(P d) {
    int k = nbIndex(d.x, d.y);
    int i = dice(1, 5, (k < 0 ? 8 : k) - 2) % 8;
    return P{nx(i), ny(i)};
  }

  /** 4c49:0a0e: a wandering line of plain from a to b. */
  void coastLine(P a, P b) {
    P cur = a;
    P first = step(a.x, a.y, b.x, b.y);
    for (int n = 0; n < SAFETY; n++) {
      P d = step(cur.x, cur.y, b.x, b.y);
      int r = reach(cur.x, cur.y, b.x, b.y) < 3 ? 1 : dice(1, 3, 0);
      if (r == 2) d = first;
      else if (r == 3) { d = turn(d); first = d; }
      cur.x += d.x; cur.y += d.y;
      clamp(cur);
      put(cur.x, cur.y, PLAIN);
      if (cur.x == b.x && cur.y == b.y) return;
    }
  }

  /** 4c49:1016: which edge a point lies on, -1 for none. */
  static int edgeOf(P p) {
    if (p.x == 0) return 3;
    if (p.y == 0) return 0;
    if (p.x == W - 1) return 1;
    if (p.y == H - 1) return 2;
    return -1;
  }

  /** A straight run of plain from a to b, one step each. */
  void straight(P a, P b) {
    P d = step(a.x, a.y, b.x, b.y);
    P cur = a;
    for (int n = 0; n < SAFETY && !(cur.x == b.x && cur.y == b.y); n++) {
      cur.x += d.x; cur.y += d.y;
      clamp(cur);
      put(cur.x, cur.y, PLAIN);
    }
  }

  /** 4c49:0b0e: two points on the map's edge are joined along it, round a
   *  corner if need be; across the map they wander as any other. */
  void coastAlongEdge(P a, P b) {
    int ea = edgeOf(a), eb = edgeOf(b);
    if (ea == eb) return straight(a, b);
    int apart = std::abs(ea - eb);
    if (apart == 2) return coastLine(a, b);
    P corner = pt(CORNERS + 4 * (apart == 3 ? 0 : std::max(ea, eb)));
    straight(a, corner);
    straight(b, corner);
  }

  void coastSegment(P a, P b) {
    if (onEdge(a) && onEdge(b)) coastAlongEdge(a, b);
    else coastLine(a, b);
  }

  /** 4c49:05c7: join every point of the outline to the next. */
  void coastJoin() {
    for (int i = 0; i < 4; i++) {
      int base = EDGE_PTS + 0x50 * i, n = r16(EDGE_COUNT + 2 * i);
      int k = 0;
      for (; k < n - 1; k++) coastSegment(pt(base + 4 * k), pt(base + 4 * (k + 1)));
      int j = (i + 1) % 4;
      coastSegment(pt(base + 4 * k), pt(EDGE_PTS + 0x50 * j));
    }
  }

  /** 4c49:10bd: may the sea flood into this tile? */
  bool floods(int x, int y) const {
    int t = at(x, y);
    if (t == SHORE || t == BRIDGE) return false;
    bool inside = x != 0 && y != 0 && x != W - 1 && y != H - 1;
    if (!inside && t == 0) return true;
    return at(x + 1, y) == WATER || at(x - 1, y) == WATER || at(x, y + 1) == WATER || at(x, y - 1) == WATER;
  }

  /** 4c49:0867: the sea floods in from the edges up to the outline; what it
   *  does not reach is land. */
  void coastFill() {
    for (int i = 0; i < W * H; i++) m[GRID + i] = m[GRID + i] == PLAIN ? SHORE : 0;
    auto flood = [&](int x, int y) { if (floods(x, y)) put(x, y, WATER); };
    for (int pass = 0; pass < 2; pass++) {
      for (int x = 0; x < W; x++) for (int y = 0; y < H; y++) flood(x, y);
      for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) flood(x, y);
      for (int x = W - 1; x >= 0; x--) for (int y = H - 1; y >= 0; y--) flood(x, y);
      for (int y = H - 1; y >= 0; y--) for (int x = W - 1; x >= 0; x--) flood(x, y);
    }
    for (int i = 0; i < W * H; i++) m[GRID + i] = m[GRID + i] == WATER ? WATER : PLAIN;
  }

  /** 4c49:0ce7: water beside land is shore. */
  void shores() {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        if (at(x, y) != WATER) continue;
        if (nearPlain(x, y)) put(x, y, SHORE);
        if (near(x, y, HILLS)) put(x, y, SHORE);
        if (near(x, y, MOUNTAINS)) put(x, y, SHORE);
      }
    }
  }

  /** 4c49:0000: the land. */
  void coast() {
    for (int i = 0; i < 4; i++) coastEnds(i);
    coastOrder();
    for (int i = 0; i < 4; i++) coastPoints(i);
    coastJoin();
    coastFill();
    shores();
    if (dice(1, 100, 0) < 50) {
      // 4c49:0daa: 0-2 channels straight across, west to east
      int n = dice(1, 3, -1);
      for (int i = 0; i < n; i++) river(true, false, true);
      waterTidy();
    }
    shores();
  }

  // --- 4d71: mountains and hills -----------------------------------------

  int node(int i) const { return NODES + 14 * i; }

  /** 4d71:0495 / 03af: seeds on random plain tiles, each a node. */
  void seeds(int n, int t, int kind) {
    for (int k = 0; k < n; k++) {
      int x = 0, y = 0;
      for (int tries = 0; tries < SAFETY; tries++) {
        x = dice(1, W, -1);
        y = dice(1, H, -1);
        if (at(x, y) == PLAIN) break;
      }
      put(x, y, t);
      int c = r16(NODE_COUNT), o = node(c);
      w16(o, x); w16(o + 2, y); w16(o + 4, kind); w16(o + 6, 0);
      w16(NODE_COUNT, c + 1);
    }
  }

  /** 4d71:057b: each node is linked to its nearest others -- a hill to 0-1,
   *  a mountain to 0-3. */
  void link() {
    int count = r16(NODE_COUNT);
    if (count < 2) return;
    for (int i = 0; i < count; i++) {
      int o = node(i);
      int n = r16(o + 4) == 1 ? dice(1, 2, -1) : dice(1, 4, -1);
      if (n >= count - 1) n = count - 1;
      w16(o + 6, n);
      for (int l = 0; l < n; l++) {
        int best = 10000, bestJ = -1;
        for (int j = 0; j < count; j++) {
          if (j == i) continue;
          int k = 0;
          while (k < l && r16(o + 8 + 2 * k) != j) k++;
          if (k != l) continue;
          int p = node(j);
          int d = reach(r16(p), r16(p + 2), r16(o), r16(o + 2));
          if (d < best) { best = d; bestJ = j; }
        }
        w16(o + 8 + 2 * l, bestJ);
      }
    }
  }

  /** 4d71:09a7: a ridge from a to b, wandering as the coast does; `width` 2
   *  adds a tile to either side, 3 one two out. Only plain and hills give
   *  way to it. */
  void ridge(P a, P b, int width, int t) {
    P cur = a;
    P first = step(a.x, a.y, b.x, b.y);
    int k = 0;
    auto lay = [&](int x, int y) {
      int v = at(x, y);
      if (v == PLAIN || v == HILLS) put(x, y, t);
    };
    for (int n = 0; n < SAFETY; n++) {
      P d = step(cur.x, cur.y, b.x, b.y);
      int r = reach(cur.x, cur.y, b.x, b.y) < 3 ? 1 : dice(1, 3, 0);
      if (r == 2) d = first;
      else if (r == 3) { d = turn(d); first = d; }
      cur.x += d.x; cur.y += d.y;
      int ki = nbIndex(d.x, d.y);
      if (ki >= 0) k = ki;
      int l = (k + 2) % 8, rr = (k + 6) % 8;
      P p1{cur.x + nx(l), cur.y + ny(l)};
      P p2{cur.x + nx(rr), cur.y + ny(rr)};
      P p3{cur.x + 2 * nx(l), cur.y + 2 * ny(l)};
      P p4{cur.x + 2 * nx(rr), cur.y + 2 * ny(rr)};
      clamp(cur);
      lay(cur.x, cur.y);
      bool done = cur.x == b.x && cur.y == b.y;
      if (width == 2) {
        clamp(p1);
        lay(p1.x, p1.y);
        lay(p2.x, p2.y);             // the original clamps only the one
      }
      if (width == 3) {
        clamp(p3);
        lay(p3.x, p3.y);
        lay(p4.x, p4.y);
      }
      if (done) return;
    }
  }

  /** 4d71:0884 / 08f3: a node with no links gets a short ridge of its own,
   *  up and to the left. */
  void blob(P p, int t) {
    P q;
    q.x = p.x + dice(1, 5, -10);
    q.y = p.y + dice(1, 5, -10);
    clamp(q);
    int width = dice(1, 2, 1);
    ridge(p, q, width, t);
  }

  /** 4d71:06d2: the ridges along the links. */
  void ridges() {
    int count = r16(NODE_COUNT);
    if (count < 2) return;
    for (int i = 0; i < count; i++) {
      int o = node(i);
      P p{r16(o), r16(o + 2)};
      bool hill = r16(o + 4) == 1;
      int links = r16(o + 6);
      if (links == 0) { blob(p, hill ? HILLS : MOUNTAINS); continue; }
      for (int l = 0; l < links; l++) {
        int q = node(r16(o + 8 + 2 * l));
        P to{r16(q), r16(q + 2)};
        if (hill && r16(q + 4) == 1) ridge(p, to, 3, HILLS);
        else {
          int width = dice(1, 2, 1);
          ridge(p, to, width, MOUNTAINS);
        }
      }
    }
  }

  /** 4d71:002e: plain hemmed in by mountains (or hills) on all four sides
   *  joins them, mountains on the shore come down, and a diagonal run of
   *  three mountains is thickened on one side. */
  void mountainTidy() {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        if (x != 0 && y != 0 && x != W - 1 && y != H - 1 && at(x, y) == PLAIN) {
          for (int t : {MOUNTAINS, HILLS}) {
            if (at(x, y + 1) == t && at(x + 1, y) == t && at(x - 1, y) == t && at(x, y - 1) == t) put(x, y, t);
          }
        }
        if (at(x, y) == MOUNTAINS && near(x, y, SHORE)) put(x, y, PLAIN);
      }
    }
    for (int x = 1; x < W - 1; x++) {
      for (int y = 1; y < H - 1; y++) {
        if (at(x, y) != MOUNTAINS) continue;
        auto mm = [&](int dx, int dy) { return at(x + dx, y + dy) == MOUNTAINS; };
        if (mm(1, 1) && mm(-1, -1) && !mm(1, -1) && !mm(-1, 1)) {
          if (dice(1, 10, 0) > 5) put(x - 1, y + 1, MOUNTAINS);
          else put(x + 1, y - 1, MOUNTAINS);
        } else if (mm(-1, 1) && mm(1, -1) && !mm(-1, -1) && !mm(1, 1)) {
          if (dice(1, 10, 0) > 5) put(x + 1, y + 1, MOUNTAINS);
          else put(x - 1, y - 1, MOUNTAINS);
        }
      }
    }
  }

  /** 4d71:033a: mountains are ringed with hills. */
  void foothills() {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        int t = at(x, y);
        if (t != MOUNTAINS && t != HILLS && near(x, y, MOUNTAINS)) put(x, y, HILLS);
      }
    }
  }

  /** 4d71:0000. */
  void highlands() {
    w16(NODE_COUNT, 0);
    seeds(r16(P_MOUNTAINS), MOUNTAINS, 2);
    seeds(r16(P_HILLS), HILLS, 1);
    link();
    ridges();
    mountainTidy();
    foothills();
  }

  // --- 4f5f: erosion and passes --------------------------------------------

  /** 4f5f:0044: a walk between two random tiles; erosion wears its
   *  mountains to hills, a pass cuts mountains and hills to plain, five tiles
   *  across. */
  void wear(bool pass) {
    P a, b;
    a.x = dice(1, W, -1);
    a.y = dice(1, H, -1);
    b.x = dice(1, W, -1);
    b.y = dice(1, H, -1);
    P cur = a;
    while (!(cur.x == b.x && cur.y == b.y)) {
      P d = step(cur.x, cur.y, b.x, b.y);
      int k = nbIndex(d.x, d.y);
      int l = (k + 1) % 8, r = (k + 7) % 8;
      int t = at(cur.x, cur.y);
      bool hit = pass ? t == MOUNTAINS || t == HILLS : t == MOUNTAINS;
      if (hit) {
        int to = pass ? PLAIN : HILLS;
        const int offs[5][2] = {{0, 0}, {nx(l), ny(l)}, {nx(r), ny(r)}, {2 * nx(l), 2 * ny(l)}, {2 * nx(r), 2 * ny(r)}};
        for (auto& o : offs) {
          P p{cur.x + o[0], cur.y + o[1]};
          clamp(p);
          put(p.x, p.y, to);
        }
      }
      cur.x += d.x; cur.y += d.y;
    }
  }

  /** Hills with three mountains on their four sides become mountain
   *  (4f5f:04d6); plain with three hills, hills (05ae). */
  void surrounded(int t, int by) {
    for (int x = 1; x < W - 1; x++) {
      for (int y = 1; y < H - 1; y++) {
        if (at(x, y) != t) continue;
        int n = (at(x, y + 1) == by) + (at(x, y - 1) == by) + (at(x + 1, y) == by) + (at(x - 1, y) == by);
        if (n > 2) put(x, y, by);
      }
    }
  }

  /** 4f5f:0000. */
  void erosion() {
    for (int i = 0; i < r16(P_EROSION); i++) wear(false);
    for (int i = 0; i < r16(P_PASSES); i++) wear(true);
    surrounded(HILLS, MOUNTAINS);
    surrounded(PLAIN, HILLS);
    foothills();
  }

  // --- 4eb7: rivers ----------------------------------------------------------

  /** 4eb7:005d: a river. Without `channel` it runs from a random hill or
   *  mountain to a random water or shore tile, 200 tries each; a channel
   *  runs from the west edge to the east. Each step lays water three tiles
   *  across (`wide`: five, a channel seven), turns aside a quarter of the
   *  time each way, and with `stops` the river ends on meeting water the
   *  second time. */
  void river(bool wide, bool stops, bool channel) {
    P a, b;
    if (channel) {
      a.x = 0;
      a.y = dice(1, H, -1);
      b.x = W - 1;
      b.y = dice(1, H, -1);
    } else {
      for (int i = 0; i < 200; i++) {
        a.x = dice(1, W, -1);
        a.y = dice(1, H, -1);
        int t = at(a.x, a.y);
        if (t == HILLS || t == MOUNTAINS) break;
      }
      for (int i = 0; i < 200; i++) {
        b.x = dice(1, W, -1);
        b.y = dice(1, H, -1);
        int t = at(b.x, b.y);
        if (t == WATER || t == SHORE) break;
      }
    }
    P cur = a;
    int met = 0;
    bool done = cur.x == b.x && cur.y == b.y;
    for (int n = 0; !done && n < SAFETY; n++) {
      P d = step(cur.x, cur.y, b.x, b.y);
      int k = nbIndex(d.x, d.y);
      int l = (k + 1) % 8, r = (k + 7) % 8, aside1 = (k + 2) % 8, aside2 = (k + 6) % 8;
      if (stops) {
        // nb(-1) reads the word before the table, as the original's did
        int ahead = at(cur.x + nx(k), cur.y + ny(k));
        if (ahead == WATER && met == 1) done = true;
        if (ahead == WATER && met == 0) met++;
      }
      auto lay = [&](int ox, int oy) {
        P p{cur.x + ox, cur.y + oy};
        clamp(p);
        put(p.x, p.y, WATER);
      };
      lay(0, 0);
      lay(nx(l), ny(l));
      lay(nx(r), ny(r));
      if (wide) {
        lay(2 * nx(l), 2 * ny(l));
        lay(2 * nx(r), 2 * ny(r));
        if (channel) {
          lay(3 * nx(l), 3 * ny(l));
          lay(3 * nx(r), 3 * ny(r));
        }
      }
      cur.x += d.x; cur.y += d.y;
      if (cur.x == b.x && cur.y == b.y) done = true;
      int roll = dice(1, 4, 0);
      if (roll == 2) { cur.x += nx(aside1); cur.y += ny(aside1); }
      else if (roll == 3) { cur.x += nx(aside2); cur.y += ny(aside2); }
    }
  }

  static bool wet(int t) { return t == SHORE || t == WATER; }

  /** 4eb7:060c: land with water on three of its four sides goes under, shore
   *  out of sight of land becomes open water, and a diagonal of water is
   *  given a shore tile beside it. */
  void waterTidy() {
    auto land = [](int t) { return t == PLAIN || t == HILLS || t == MOUNTAINS; };
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        if (!land(at(x, y))) continue;
        int n = wet(at(x, y + 1)) + wet(at(x, y - 1)) + wet(at(x + 1, y)) + wet(at(x - 1, y));
        if (n > 2) put(x, y, WATER);
      }
    }
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        if (at(x, y) == SHORE && !near(x, y, PLAIN) && !near(x, y, HILLS) && !near(x, y, MOUNTAINS)) put(x, y, WATER);
      }
    }
    auto dry = [](int t) { return t == HILLS || t == FOREST || t == PLAIN; };
    for (int x = 1; x < W - 1; x++) {
      for (int y = 1; y < H - 1; y++) {
        if (!wet(at(x, y))) continue;
        auto a = [&](int dx, int dy) { return at(x + dx, y + dy); };
        if (wet(a(1, 1)) && wet(a(-1, -1)) && dry(a(1, -1)) && dry(a(-1, 1))) {
          if (dice(1, 10, 0) > 4) put(x - 1, y + 1, SHORE);
          else put(x + 1, y - 1, SHORE);
        } else if (wet(a(-1, 1)) && wet(a(1, -1)) && dry(a(-1, -1)) && dry(a(1, 1))) {
          if (dice(1, 10, 0) > 4) put(x + 1, y + 1, SHORE);
          else put(x - 1, y - 1, SHORE);
        }
      }
    }
  }

  /** 4eb7:0000. */
  void rivers() {
    for (int i = 0; i < r16(P_RIVERS); i++) river(false, true, false);
    for (int i = 0; i < r16(P_WIDE_RIVERS); i++) river(true, true, false);
    waterTidy();
    shores();
    mountainTidy();
    foothills();
    waterTidy();
    shores();
  }

  // --- 4e47: forests --------------------------------------------------------

  void addForest() { w16(FOREST_DONE, r16(FOREST_DONE) + 1); }

  /** 4e47:00b5: a wood round a random plain tile: eight arms, and off each
   *  step of an arm two side shoots that shorten as it goes. */
  void wood() {
    int x = 0, y = 0;
    for (int tries = 0; tries < SAFETY; tries++) {
      x = dice(1, W, -1);
      y = dice(1, H, -1);
      if (at(x, y) == PLAIN) break;
    }
    int arms[8];
    int span, base;
    if (dice(1, 100, 0) < 65) {
      for (int i = 0; i < 8; i++) arms[i] = dice(1, 8, 2);
      span = 4; base = 2;
    } else {
      for (int i = 0; i < 8; i++) arms[i] = dice(1, 10, 5);
      span = 6; base = 4;
    }
    auto grow = [&](P& p) {
      clamp(p);
      if (at(p.x, p.y) == PLAIN) {
        put(p.x, p.y, FOREST);
        addForest();
      }
    };
    put(x, y, FOREST);
    addForest();
    for (int i = 0; i < 8; i++) {
      P cur{x, y};
      int k = 0;
      for (int s = 0; s < arms[i]; s++) {
        cur.x += nx(i); cur.y += ny(i);
        grow(cur);
        int l = (i + 2) % 8, r = (i + 6) % 8;
        k++;
        int nl = dice(1, span - k / 2, base - k / 4);
        int nr = dice(1, span - k / 2, base - k / 3);
        P p = cur;
        for (int j = 0; j < nl; j++) { p.x += nx(l); p.y += ny(l); grow(p); }
        P q = cur;
        for (int j = 0; j < nr; j++) { q.x += nx(r); q.y += ny(r); grow(q); }
      }
    }
  }

  /** 4e47:03d9: plain with forest on three sides is forest, and a diagonal
   *  of forest is given a tile beside it. */
  void forestTidy() {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        if (at(x, y) != PLAIN) continue;
        int n = (at(x, y + 1) == FOREST) + (at(x, y - 1) == FOREST) + (at(x + 1, y) == FOREST) + (at(x - 1, y) == FOREST);
        if (n > 2) {
          put(x, y, FOREST);
          addForest();
        }
      }
    }
    auto open = [](int t) { return t == HILLS || t == SHORE || t == PLAIN; };
    for (int x = 1; x < W - 1; x++) {
      for (int y = 1; y < H - 1; y++) {
        if (at(x, y) != FOREST) continue;
        auto a = [&](int dx, int dy) { return at(x + dx, y + dy); };
        if (a(1, 1) == FOREST && a(-1, -1) == FOREST && open(a(1, -1)) && open(a(-1, 1))) {
          if (dice(1, 10, 0) < 5) put(x + 1, y - 1, FOREST);
          else put(x - 1, y + 1, FOREST);
        } else if (a(-1, 1) == FOREST && a(1, -1) == FOREST && open(a(-1, -1)) && open(a(1, 1))) {
          if (dice(1, 10, 0) < 5) put(x - 1, y - 1, FOREST);
          else put(x + 1, y + 1, FOREST);
        }
      }
    }
  }

  /** 4e47:0000: woods until they cover land / 100 x the Forest parameter. */
  void forests() {
    int land = 0;
    for (int i = 0; i < W * H; i++) {
      int t = m[GRID + i];
      if (t == PLAIN || t == HILLS || t == MOUNTAINS) land++;
    }
    w16(FOREST_WANTED, land / 100 * r16(P_FOREST));
    w16(FOREST_DONE, 0);
    for (int n = 0; n < 10000 && r16(FOREST_DONE) < r16(FOREST_WANTED); n++) {
      wood();
      forestTidy();
    }
  }

  // --- 4fc9: marshes ----------------------------------------------------------

  /** 4fc9:0029: 1d5+3 tiles of marsh scattered round (x, y). */
  void marshAround(int x, int y) {
    int n = dice(1, 5, 3);
    for (int i = 0; i < n; i++) {
      P p;
      int r1 = dice(1, 3, 0);
      int k1 = dice(1, 8, -1);
      p.x = x + r1 * nx(k1);
      int r2 = dice(1, 3, 0);
      int k2 = dice(1, 8, -1);
      p.y = y + r2 * ny(k2);
      clamp(p);
      if (at(p.x, p.y) == PLAIN) put(p.x, p.y, MARSH);
    }
  }

  /** 4fc9:010a: a marsh, near the shore if one of four tries finds it. */
  void marsh() {
    int x = 0, y = 0, tries = 0;
    for (int n = 0; n < SAFETY; n++) {
      x = dice(1, 102, 5);
      y = dice(1, 146, 5);
      if (at(x, y) != PLAIN) continue;
      if (tries >= 4) break;
      tries++;
      if (near(x, y, SHORE) || near(x + 1, y, SHORE) || near(x, y + 1, SHORE) || near(x + 1, y + 1, SHORE)) break;
    }
    put(x, y, MARSH);
    marshAround(x, y);
    marshAround(x + 1, y + 1);
    marshAround(x - 1, y - 1);
    marshAround(x - 1, y + 1);
    marshAround(x + 1, y - 1);
  }

  /** 4f5f:06a0: 1d3 marshes. */
  void marshes() {
    int n = dice(1, 3, 0);
    for (int i = 0; i < n; i++) marsh();
  }

  // --- 513d: cities and sites ---------------------------------------------

  /** 513d:0331: a city's 2x2 footprint, clear of water, hills and other
   *  cities by a tile; on the coast if one of four tries finds it. */
  void placeCity() {
    int x = 0, y = 0, tries = 0;
    for (int n = 0; n < SAFETY; n++) {
      x = dice(1, 102, 5);
      y = dice(1, 146, 5);
      bool ok = true;
      for (auto& f : FOOTPRINT) {
        int dx = f[0], dy = f[1];
        int t = at(x + dx, y + dy);
        if (t == SHORE || t == WATER || t == CITY || t == MOUNTAINS || t == HILLS) ok = false;
        if (near(x + dx, y + dy, CITY) || near(x + dx + 1, y + dy + 1, CITY) || near(x + dx - 1, y + dy - 1, CITY)) ok = false;
      }
      if (!ok) continue;
      if (tries >= 4) break;
      tries++;
      if (near(x, y, SHORE) || near(x + 1, y, SHORE) || near(x, y + 1, SHORE) || near(x + 1, y + 1, SHORE)) break;
    }
    int c = cityCount();
    ws16(crec(c), x);
    ws16(crec(c) + 2, y);
    setCityCount(c + 1);
    for (auto& f : FOOTPRINT) put(x + f[0], y + f[1], CITY);
  }

  /** 513d:0000. */
  void cities() {
    setCityCount(0);
    for (int i = r16(0xa84); i < r16(P_CITIES); i++) placeCity();
  }

  /** 513d:0a77: somewhere for a site. The map is taken a sixteenth at a time,
   *  round in turn: 50 tries there for plain clear of cities and of other
   *  sites by three, then 50 anywhere, then 50 for anything not a city. */
  P siteSpot() {
    if (siteCell == -1) siteCell = dice(1, 16, -1);
    int cxx = siteCell % 4, cyy = siteCell / 4;
    int x = 0, y = 0;
    bool found = false;
    for (int i = 0; i < 50 && !found; i++) {
      x = dice(1, 20, cxx * W / 4 + 3);
      y = dice(1, 31, cyy * H / 4 + 3);
      found = at(x, y) == PLAIN && !near(x, y, CITY) && !near(x, y, SITE) && !near(x - 3, y, SITE) &&
              !near(x + 3, y, SITE) && !near(x, y - 3, SITE) && !near(x, y + 3, SITE);
    }
    for (int i = 0; i < 50 && !found; i++) {
      x = dice(1, 102, 5);
      y = dice(1, 146, 5);
      found = at(x, y) == PLAIN && !near(x, y, CITY) && !near(x, y, SITE);
    }
    if (!found) {
      x = dice(1, 102, 5);
      y = dice(1, 146, 5);
      for (int i = 0; i < 50 && !found; i++) {
        x = dice(1, 102, 5);
        y = dice(1, 146, 5);
        found = at(x, y) != CITY && at(x, y) != SITE;
      }
    }
    siteCell = (siteCell + 1) % 16;
    return P{x, y};
  }

  /** 513d:003a, its start: forty sites, every one unplaced. */
  void sitesBegin() {
    ws16(S_SITE_COUNT, N_SITES);
    for (int i = 0; i < N_SITES; i++) {
      ws16(S_SITES + SITE_STRIDE * i + 2, -1);
      ws16(S_SITES + SITE_STRIDE * i, -1);
    }
  }

  /** 513d:003a: site `i`, keeping the template's temples and ruins, with
   *  names and descriptions from RANDOM.DAT's word lists. */
  void site(int i) {
    P s = siteSpot();
    int o = S_SITES + SITE_STRIDE * i;
    ws16(o, s.x);
    ws16(o + 2, s.y);
    put(s.x, s.y, SITE);
    std::string name, text;
    if (scn[o + 24] == 1) {
      int k = dice(1, 10, -1);
      name = str(0x1000 + 10 * k, 10) + " Temple";
      text = "#%03d|The %s can bless your|armies or give you|quests|\n";
    } else {
      int f = dice(1, r16(SITE_FORMATS), -1);
      int k1 = dice(1, 10, -1);
      std::string end = str(0xf9c + 10 * k1, 10);
      int k2 = dice(1, 10, -1);
      std::string word = str(0xf38 + 10 * k2, 10) + end;
      name = format(str(SITE_FORMATS + 2 + 20 * f, 20), word);
      text = "#%03d|%s is|inhabited by monsters and|full of treasure!|\n";
    }
    wstr(o + 4, name);
    spc += format(text, i, name);
  }

  // --- 4fef: terrain to tiles -----------------------------------------------

  /** 4fef:005c: every terrain type its first tile, a city its castle and the
   *  first eight sites their own. */
  void toTiles() {
    for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        int t = at(x, y);
        if (t == CITY && at(x, y - 1) != CITY && at(x - 1, y) != CITY && x > 0 && y > 0) {
          int c = r16(TILES + 64 * CITY) & 0xff;
          setTile(x, y, c); setTile(x + 1, y, c + 1);
          setTile(x, y + 1, c + 16); setTile(x + 1, y + 1, c + 17);
        } else if (t == SITE) {
          setTile(x, y, tileFor(SITE, 0, x, y));
        } else if (t != CITY) {
          setTile(x, y, tileFor(t, 0, x, y));
        }
      }
    }
    for (int i = 0; i < 8; i++) {
      int o = S_SITES + SITE_STRIDE * i;
      setTile(s16(o), s16(o + 2), r16(TILES + 64 * SITE + 4) & 0xff);
    }
  }

  /** The 8 neighbours as a mask (bit i for neighbour i), counting the map's
   *  edge in: 4fef:0c9d (water, shore, bridge), 0d4f (forest), 0dcb
   *  (mountains), 0e47 (hills or mountains). */
  int mask(int x, int y, const std::function<bool(int)>& test) const {
    int mk = 0;
    for (int i = 0; i < 8; i++) {
      int ax = x + nx(i), ay = y + ny(i);
      bool off = ax < 0 || ay < 0 || ax >= W || ay >= H;
      if (off || test(at(ax, ay))) mk |= 1 << i;
    }
    return mk;
  }

  int shape(int mk) const { return i8(m[SHAPE + mk]); }

  template <class F>
  void each(int t, F f) {
    for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) if (at(x, y) == t) f(x, y);
  }

  /** 4fef:0464: the tiles that join each kind of ground to its neighbours. A
   *  shape the tile set has no tile for gives way: water to marsh, forest
   *  and hills often to plain, mountains to hills. */
  void shapes() {
    for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        int t = at(x, y);
        if (t != WATER && t != SHORE) continue;
        int v = shape(mask(x, y, [](int n) { return n == WATER || n == SHORE || n == BRIDGE; }));
        if (v < 0) {
          put(x, y, MARSH);
          setTile(x, y, tileFor(MARSH, 0, x, y));
          marshAround(x, y);
          tiles[y * W + x] &= 0x7fff;
        } else {
          put(x, y, WATER);
          setTile(x, y, tileFor(WATER, v, x, y));
        }
      }
    }
    each(FOREST, [&](int x, int y) {
      int v = shape(mask(x, y, [](int n) { return n == FOREST; }));
      if (v >= 0) {
        setTile(x, y, tileFor(FOREST, v, x, y));
        put(x, y, FOREST);
      } else if (dice(1, 10, 0) < 6) {
        setTile(x, y, tileFor(PLAIN, 0, x, y));
        put(x, y, PLAIN);
      } else {
        setTile(x, y, tileFor(FOREST, 13, x, y));
      }
    });
    each(MOUNTAINS, [&](int x, int y) {
      int v = shape(mask(x, y, [](int n) { return n == MOUNTAINS; }));
      if (v >= 0) {
        setTile(x, y, tileFor(MOUNTAINS, v, x, y));
        put(x, y, MOUNTAINS);
      } else {
        setTile(x, y, tileFor(HILLS, 0, x, y));
        put(x, y, HILLS);
      }
    });
    each(HILLS, [&](int x, int y) {
      int v = shape(mask(x, y, [](int n) { return n == MOUNTAINS || n == HILLS; }));
      if (v >= 0) {
        setTile(x, y, tileFor(HILLS, v, x, y));
        put(x, y, HILLS);
      } else if (dice(1, 10, 0) < 6) {
        setTile(x, y, tileFor(PLAIN, 0, x, y));
        put(x, y, PLAIN);
      } else {
        put(x, y, HILLS);
        setTile(x, y, tileFor(HILLS, 13, x, y));
      }
    });
    each(MARSH, [&](int x, int y) {
      // mostly the plain marsh; three in ten one of four others
      int o = dice(1, 10, 0) < 7 ? 2 * parity(x, y) : 4 * dice(1, 4, 0);
      setTile(x, y, r16(TILES + 64 * MARSH + o) & 0xff);
      put(x, y, MARSH);
    });
  }

  // --- 5311: bridges, crossings and roads -----------------------------------

  /** 5311:0a7f: ground a bridge may land on. */
  bool firm(int x, int y) const {
    int t = at(x, y);
    return t == PLAIN || t == FOREST || t == HILLS;
  }

  /** The nearest city, by map distance, to each listed spot (shared by
   *  5311:03c3 and 08f6; a spot's figure is worked out once). */
  void nearestCities(SpotList& list, int n) {
    if (list.cityDist[0] != -1) return;
    for (int i = n - 1; i >= 0; i--) {
      if (list.x[i] == -1) continue;
      int best = 10000;
      for (int c = cityCount() - 1; c >= 0; c--) best = std::min(best, distance(list.x[i], list.y[i], cx(c), cy(c)));
      list.cityDist[i] = best;
    }
  }

  /** 5311:08f6 (bridges) and 03c3 (crossings): the best spot still free --
   *  a die roll, plus more the nearer a city is. Bridges keep 10 apart. */
  int pickSpot(SpotList& list, int n, bool apart) {
    nearestCities(list, n);
    for (int i = n - 1; i >= 0; i--) {
      if (list.chosen[i] != 0 || list.x[i] == -1) continue;
      int best = 10000;
      for (int j = n - 1; j >= 0; j--) {
        if (list.chosen[j] != 0) best = std::min(best, distance(list.x[i], list.y[i], list.x[j], list.y[j]));
      }
      list.chosenDist[i] = best;
    }
    int bestScore = -1, pick = -1;
    for (int i = n - 1; i >= 0; i--) {
      if (list.chosen[i] != 0 || list.x[i] == -1) continue;
      if (apart && !(list.chosenDist[i] > 9)) continue;
      int roll = dice(1, 15, 1);
      int nearBy = list.cityDist[i] < 31 ? 30 - list.cityDist[i] : 0;
      if (bestScore < nearBy + roll) { bestScore = nearBy + roll; pick = i; }
    }
    return pick;
  }

  /** 5311:05b4: bridges over two-tile rivers. As the original has it, only
   *  a river running north and south (tiles 0x26 and 0x28) is ever bridged. */
  void bridges(SpotList& list) {
    int n = 0;
    for (int y = 1; y < H - 2; y++) {
      for (int x = 1; x < W - 2; x++) {
        int t = tile(x, y);
        bool across = (t == 0x26 || t == 0x16) && (tile(x + 1, y) == 0x28 || tile(x + 1, y) == 0x18);
        bool along = (t == 0x21 || t == 0x11) && (tile(x, y + 1) == 0x24 || tile(x, y + 1) == 0x14);
        if (!across && !along) continue;
        int ew = t == 0x26 ? 1 : 0;
        bool ok = ew && ((firm(x - 1, y) && firm(x + 2, y)) || (firm(x, y - 1) && firm(x, y + 2)));
        if (ok && n < 50) {
          list.x[n] = x; list.y[n] = y; list.ew[n] = ew;
          n++;
        }
      }
    }
    if (n == 0) return;
    for (int i; (i = pickSpot(list, n, true)) != -1;) {
      list.chosen[i] = 1;
      int x = list.x[i], y = list.y[i];
      if (list.ew[i] == 0) {
        setTile(x, y, 0x84); setTile(x, y + 1, 0x94);
        put(x, y + 1, BRIDGE); put(x, y, BRIDGE);
        setRoad(x, y + 2, 1); setRoad(x, y - 1, 1);
      } else {
        setTile(x, y, 0x85); setTile(x + 1, y, 0x86);
        put(x + 1, y, BRIDGE); put(x, y, BRIDGE);
        setRoad(x + 2, y, 1); setRoad(x - 1, y, 1);
      }
    }
  }

  /** 5311:01f6: up to ten crossings -- shore that looks out on three tiles of
   *  open water, at least 15 from the others. */
  void crossings(SpotList& list) {
    for (int y = 1; y < H - 2; y++) {
      for (int x = 1; x < W - 2; x++) {
        if (dice(1, 2, -1) != 0) continue;
        int dx = 0, dy = 0;
        switch (tile(x, y)) {
          case 0x20: case 0x23: dx = -1; break;
          case 0x21: dy = -1; break;
          case 0x22: case 0x25: dx = 1; break;
          case 0x24: dy = 1; break;
          default: continue;
        }
        bool open = true;
        for (int k = 1; k <= 3; k++) if (at(x + k * dx, y + k * dy) != WATER) open = false;
        if (!open) continue;
        int nearBy = 10000;
        for (int i = 49; i >= 0; i--) {
          if (list.x[i] != -1) nearBy = std::min(nearBy, distance(x, y, list.x[i], list.y[i]));
        }
        if (nearBy <= 14) continue;
        for (int i = 0; i < 50; i++) {
          if (list.x[i] == -1) { list.x[i] = x; list.y[i] = y; break; }
        }
      }
    }
    list.cityDist[0] = -1;
    for (int n = 0, i; n < 10 && (i = pickSpot(list, 50, false)) != -1; n++) {
      list.chosen[i] = 1;
      tiles[list.y[i] * W + list.x[i]] |= 0x8000;
    }
  }

  bool isServed(int c) const { return c >= 0 && c < (int)served.size() ? served[c] != 0 : false; }
  void serve(int c) { if (c >= 0 && c < (int)served.size()) served[c] = 1; }

  /** 5311:0e22: a pair of cities to join -- a random one not yet served, and
   *  of the unserved 30 to 70 away, the one that rolls highest. */
  std::optional<std::pair<int, int>> roadPair() {
    for (;;) {
      int a = -1;
      for (int n = 80; n > 0; n--) {
        a = dice(1, cityCount(), -1);
        if (!isServed(a)) break;
        a = -1;
      }
      if (a == -1) return std::nullopt;
      serve(a);
      int b = -1, best = -1;
      for (int c = cityCount() - 1; c >= 0; c--) {
        if (isServed(c)) continue;
        int d = distance(cx(a), cy(a), cx(c), cy(c));
        if (d > 29 && d < 71) {
          int roll = dice(1, 1000, 0);
          if (best < roll) { best = roll; b = c; }
        }
      }
      if (b != -1) { serve(b); return std::make_pair(a, b); }
    }
  }

  /** 5311:0ad0: a tile round a city's footprint to start a road from. */
  std::optional<P> roadEnd(int c) {
    for (int n = 8; n > 0; n--) {
      const int* r = ROUND_CITY[dice(1, 12, -1)];
      int x = cx(c) + r[0], y = cy(c) + r[1];
      int t = at(x, y);
      if (t == PLAIN || t == FOREST || t == HILLS) return P{x, y};
    }
    return std::nullopt;
  }

  /** 5311:0c1c: lay a road along the path the game's own pathfinder finds,
   *  as pseudo-player 14 -- not over water, shore or bridges -- and every six
   *  steps count the cities within 10 as served. */
  void layRoad(P from, P to) {
    move::RoadGround ground;
    ground.terrain = [this](int x, int y) { return terrainOf(tile(x, y)); };
    ground.road = [this](int x, int y) { return road(x, y) != 0; };
    ground.crossing = [this](int x, int y) {
      return x >= 0 && y >= 0 && x < W && y < H && (tiles[y * W + x] & 0x8000) != 0;
    };
    auto route = move::roadRoute(ground, ROAD_COST, W, H, from.x, from.y, to.x, to.y);
    if (!route) return;
    int x = from.x, y = from.y, since = 0;
    for (size_t i = 0; i < route->size() && i < 200; i++) {
      since++;
      if (x == to.x && y == to.y) break;
      x = (*route)[i].first; y = (*route)[i].second;
      int t = at(x, y);
      if (t != SHORE && t != WATER && t != BRIDGE) setRoad(x, y, 1);
      if (since > 5) {
        since = 0;
        for (int c = cityCount() - 1; c >= 0; c--) {
          if (distance(x, y, cx(c), cy(c)) < 10) serve(c);
        }
      }
    }
  }

  /** 5311:0000, its tiles: then bridges and crossings. */
  void roadTiles() {
    toTiles();
    shapes();
    shapes();
    for (int i = 0; i < W * H; i++) { tiles[i] &= 0x7fff; rd[i] &= 0xe0; }
  }

  void roadCrossings() {
    SpotList list;
    for (int i = 0; i < 50; i++) {
      list.x[i] = list.y[i] = list.ew[i] = list.cityDist[i] = list.chosenDist[i] = -1;
      list.chosen[i] = 0;
    }
    bridges(list);
    for (int i = 49; i >= 0; i--) {
      if (list.chosen[i] == 0) list.x[i] = list.y[i] = list.ew[i] = list.cityDist[i] = list.chosenDist[i] = -1;
    }
    crossings(list);
    served.assign(100, 0);
    shown = 4;
  }

  /** The cities joined by roads, a pair at a time; a progress figure when
   *  the original's bar moves on, nothing once every city is served. */
  std::optional<int> roadsUntilShown() {
    for (;;) {
      std::pair<int, int> pair;
      if (pending) {
        pair = *pending;
        pending.reset();
      } else {
        auto p = roadPair();
        if (!p) break;
        pair = *p;
        if (shown < 10 && dice(1, 3, -1) == 0) {
          pending = pair;
          return 80 + shown++;
        }
      }
      auto a = roadEnd(pair.first);
      std::optional<P> b;
      if (a) b = roadEnd(pair.second);
      if (a && b && !(a->x == b->x && a->y == b->y)) layRoad(*a, *b);
    }
    for (int i = 0; i < W * H; i++) if (m[GRID + i] == SITE) rd[i] &= 0xe0;
    return std::nullopt;
  }

  /** 4fef:0a4b: each road its shape from the roads and bridges round it; a
   *  shape there is no piece for leaves plain or hills. Then a dozen or two
   *  straight pieces take their other look. */
  void roadShapes() {
    for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        if (road(x, y) == 0) continue;
        int mk = 0;
        for (int i = 0; i < 8; i++) {
          int ax = x + r16(NB_ROAD + 4 * i), ay = y + r16(NB_ROAD + 4 * i + 2);
          if (ax >= 0 && ay >= 0 && ax < W && ay < H && (road(ax, ay) != 0 || terrainOf(tile(ax, ay)) == BRIDGE)) mk |= 1 << i;
        }
        int v = i8(m[ROAD_SHAPE + mk]);
        if (v >= 0) setRoad(x, y, v + 1);
        else if (dice(1, 10, 0) < 6) {
          setTile(x, y, tileFor(PLAIN, 0, x, y));
          put(x, y, PLAIN);
        } else {
          setTile(x, y, tileFor(HILLS, 13, x, y));
        }
      }
    }
    const int swaps[2][2] = {{2, 17}, {1, 16}};
    for (auto& sw : swaps) {
      int n = dice(1, 10, 10);
      for (int k = 0; k < n; k++) {
        for (int tries = 0; tries < 10000; tries++) {
          int x = dice(1, W, -1);
          int y = dice(1, H, -1);
          if (road(x, y) == sw[0]) { setRoad(x, y, sw[1]); break; }
        }
      }
    }
  }

  // --- 513d / 5132: the sides and the cities' details ---------------------

  /** 513d:05e3: the cities, read back off the tile map in rows. */
  void cityList() {
    int n = 0;
    for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        if (tile(x, y) != 0x60) continue;
        int o = crec(n);
        ws16(o, x); ws16(o + 2, y);
        scn[o + 21] = 15;
        scn[o + 20] = (uint8_t)dice(1, 3, 2);
        n++;
      }
    }
    setCityCount(n);
  }

  /** 513d:08f0: a random city in one of the map's eight regions, four
   *  across and two down; after 202 tries, wherever the last one was. */
  int cityIn(int region) {
    int x0 = (region % 4) * 28, y0 = region / 4 * 78;
    int c = 0;
    for (int tries = 0; tries < 202; tries++) {
      c = dice(1, cityCount(), -1);
      int x = cx(c), y = cy(c);
      if (x >= x0 && x < x0 + 28 && y >= y0 && y < y0 + 78) break;
    }
    return c;
  }

  static int sideRec(int s) { return S_SIDE_REC + 20 * s; }

  /** 513d:0689: each side a capital in a region of its own, 12 from the
   *  edge and 20 from the others both ways; ten rounds of 100 tries a side,
   *  the last round taking what it gets. A side's armies favour its region. */
  void capitals() {
    for (int round = 1;; round++) {
      bool used[8] = {false};
      for (int s = 0; s < 8; s++) { ws16(sideRec(s) + 6, -100); ws16(sideRec(s) + 8, -100); }
      bool again = false;
      for (int s = 0; s < 8 && !again; s++) {
        int region;
        do region = dice(1, 8, -1); while (used[region]);
        used[region] = true;
        int c = 0, tries = 0;
        bool ok = false;
        for (; !ok && tries < 100; tries++) {
          c = cityIn(region);
          int x = cx(c), y = cy(c);
          if (x < 12 || x > 100 || y < 12 || y > 144) continue;
          ok = true;
          for (int o = 0; o < 8; o++) {
            if (std::abs(x - s16(sideRec(o) + 6)) < 20 || std::abs(y - s16(sideRec(o) + 8)) < 20) { ok = false; break; }
          }
        }
        // a find on the hundredth try counts as none, as the original has it
        if (tries > 99 && round < 10) { again = true; break; }
        w16(REGION_CLASS + 2 * region, r16(SIDE_CLASS + 2 * s));
        ws16(sideRec(s) + 6, cx(c));
        ws16(sideRec(s) + 8, cy(c));
      }
      if (!again) break;
    }
    for (int s = 0; s < 8; s++) {
      int x = s16(sideRec(s) + 6), y = s16(sideRec(s) + 8);
      for (int c = 0; c < cityCount(); c++) {
        if (cx(c) == x && cy(c) == y) { scn[crec(c) + 21] = (uint8_t)s; break; }
      }
      // 513d:09b3: the castle in the side's colours
      int t = s < 6 ? 0x62 + 2 * s : 0x80 + 2 * (s - 6);
      setTile(x, y, t); setTile(x + 1, y, t + 1);
      setTile(x, y + 1, t + 16); setTile(x + 1, y + 1, t + 17);
    }
  }

  /** 5132:0014 / 006b: each side a name from five, and 3d50+20 gold. */
  void sides() {
    for (int s = 0; s < 8; s++) {
      int k = dice(1, 5, -1);
      std::string name = str(SIDE_NAMES + 100 * s + 20 * k, 20);
      for (int i = 0; i < 20; i++) scn[20 * s + i] = 0;
      wstr(20 * s, name);
    }
    for (int s = 0; s < 8; s++) ws16(sideRec(s) + 2, dice(3, 50, 20));
  }

  /** What touches a city's footprint (513d:0ce0, into 4125:02d6-02de). */
  Surroundings surroundings(int c) const {
    int x = cx(c), y = cy(c);
    auto by = [&](int t) { return near(x, y, t) || near(x + 1, y, t) || near(x, y + 1, t) || near(x + 1, y + 1, t); };
    return Surroundings{by(SHORE), by(FOREST), by(ROAD), by(HILLS), by(MARSH)};
  }

  /** random_city_value (513d:1104): a capital 9; else 1d4-1, +4 by the
   *  shore, +2 by a road, -1 by forest, -2 by marsh, within 0..9. */
  int value(int c, const Surroundings& f) {
    if (scn[crec(c) + 21] != 15) return 9;
    int v = dice(1, 4, -1);
    if (f.shore) v += 4;
    if (f.road) v += 2;
    if (f.forest) v -= 1;
    if (f.marsh) v -= 2;
    if (v > 8) v = 9;
    if (v < 1) v = 0;
    return v;
  }

  /** 513d:1468: a name of two or three syllables, ending most often in a
   *  word for what lies round it. */
  std::string cityName(int c, const Surroundings& f) {
    bool three = dice(1, 10, 0) < 7;
    int k = dice(1, 20, -1);
    std::string name = str(0xa86 + 10 * k, 10);
    if (three) {
      int k2 = dice(1, 20, -1);
      name += str(0xb4e + 10 * k2, 10);
    }
    int end = -1;
    if (dice(1, 10, 0) < 7) {
      if (f.marsh) end = 0xda6;
      else if (f.forest) end = 0xcde;
      else if (f.shore) end = 0xd42;
      else if (f.hills) end = 0xe0a;
      else if (dice(1, 10, 0) < 5) end = 0xe6e;
    }
    if (end != -1) {
      int k3 = dice(1, 10, -1);
      name += str(end + 10 * k3, 10);
    } else {
      int k3 = dice(1, 20, -1);
      name += str(0xc16 + 10 * k3, 10);
    }
    name = name.substr(0, 15);
    int o = crec(c) + 4;
    for (int i = 0; i < 16; i++) scn[o + i] = 0;
    wstr(o, name);
    return name;
  }

  /** 513d:1171: the city's name, income -- value x 2 + 1d8 + 14 -- and its
   *  three lines of description, grander as the city is richer. */
  void cityText(int c, int v, const Surroundings& f) {
    std::string name = cityName(c, f);
    ws16(crec(c) + 42, v * 2 + dice(1, 8, 0) + 14);
    auto grade = [&]() { return std::min(9, std::max(0, dice(1, 3, v - 2))); };
    int g1 = grade();
    int g2 = grade();
    auto w = [&](int o, int i) { return str(o + 16 * i, 16); };
    std::string a, b, cc;
    bool plain = !f.forest && !f.marsh && !f.hills;
    if (plain || dice(1, 100, 0) > 79) {
      if (dice(1, 100, 0) < 50) {
        a = w(0x16a8, dice(1, 10, -1));
        b = w(0x1748, dice(1, 10, -1));
        cc = w(0x17e8, dice(1, 10, -1));
      } else {
        a = w(0x14c8, dice(1, 10, -1));
        b = w(0x1568, dice(1, 10, -1));
        cc = w(0x1608, dice(1, 10, -1));
      }
    } else {
      int k;
      if (f.marsh) k = dice(1, 2, 7);
      else if (f.hills) k = dice(1, 4, 3);
      else k = dice(1, 4, -1);
      a = w(0x12e8, dice(1, 10, -1));
      b = w(0x1388, dice(1, 10, -1));
      cc = w(0x1428, k);
    }
    std::string kind = w(0x1248, g2);
    int k = dice(1, 3, -1);
    std::string adj = str(0x1068 + 0x30 * g1 + 16 * k, 16);
    cty += format("#%03d|%s is a %s|%s, %s|%s %s|\n", c, name, adj, kind, a, b, cc);
  }

  /** The army types' flags the generator asks after (build_army_move_flags,
   *  6715:0000): ARMYTYPE +54 flies, +48 magical -- an ally. */
  bool flies(int t) const { const ArmyType* a = types.byId(t); return a && a->bonus[54] != 0; }
  bool magical(int t) const { const ArmyType* a = types.byId(t); return a && a->bonus[48] != 0; }

  void setSlot(int c, int k, int e) {
    int o = crec(c);
    const ArmyType* a = types.byId(e);
    scn[o + 22 + k] = (uint8_t)e;
    scn[o + 26 + k] = (uint8_t)(a ? a->time : 0);
    scn[o + 30 + k] = (uint8_t)(a ? a->strength : 0);
    scn[o + 34 + k] = (uint8_t)(a ? a->move : 0);
    scn[o + 38 + k] = (uint8_t)(a ? a->cost : 0);
  }

  /** 513d:161d: what a city can make. value / 2 + 1d4 - 1 slots, one more
   *  each by forest and hills, at most 4, from RANDOM.DAT's list in order:
   *  each type rolls its chance, and must suit the city's region, or its
   *  forest, hills or shore, or be at home anywhere. A rich city sometimes
   *  adds a flier. Allies only where the option lets cities make them. */
  void production(int c, int v, const Surroundings& f) {
    int x = cx(c), y = cy(c);
    int region = x * 4 / W + y * 2 / H * 4;
    int slots = v / 2;
    slots += dice(1, 4, -1);
    if (f.forest) slots++;
    if (f.hills) slots++;
    if (slots > 3) slots = 4;
    if (slots < 1) slots = 0;
    auto rec = [](int e) { return PRODUCTION + 16 * e; };
    auto suits = [&](int e) {
      int terr = r16(rec(e) + 6);
      return terr == 7 || (terr == 4 && f.forest) || (terr == 5 && f.hills) ||
             r16(rec(e) + 4) == r16(REGION_CLASS + 2 * region);
    };
    int k = 0;
    for (int e = 0; e < 29 && k < slots; e++) {
      int t = r16(rec(e));
      if (!allies && magical(t)) continue;
      if (dice(1, 10, -1) >= r16(rec(e) + 2)) continue;
      bool ok = suits(e) || (r16(rec(e) + 6) == 3 && f.shore);
      if (t == 5 && !f.shore) ok = false;
      if (ok) setSlot(c, k++, t);
    }
    if (k == 0) setSlot(c, k++, r16(rec(0)));
    if (v > 6 && dice(1, 10, -1) < 5 && k < 4) {
      bool flier = false;
      for (int j = 0; j < k; j++) if (flies(scn[crec(c) + 22 + j])) flier = true;
      if (!flier && k < slots) {
        for (int e = 0; e < 29; e++) {
          int t = r16(rec(e));
          if (flies(t) && !magical(t) && suits(e)) { setSlot(c, k++, t); break; }
        }
      }
    }
    for (; k < 4; k++) scn[crec(c) + 22 + k] = 255;
  }

  /** random_magical_type (6563:1a9b). */
  int magicalType() {
    int n = 0;
    for (int t = 0; t < 28; t++) if (magical(t)) n++;
    int pick = dice(1, n, -1);
    int i = 0, t = 0;
    for (; t < 28; t++) {
      if (!magical(t)) continue;
      if (i == pick) break;
      i++;
    }
    return t > 27 || !magical(t) ? 25 : t;
  }

  /** random_add_production (513d:1b3f): with allies on, 2d3 cities may also
   *  make one. */
  void allyProduction() {
    int n = dice(2, 3, 0);
    for (int tries = 0; n > 0 && tries < 100; tries++) {
      int c = dice(1, cityCount(), -1), o = crec(c);
      int k = 0;
      for (int j = 0; j < 4; j++) if (scn[o + 22 + j] < 128) k++;
      if (k < 4) { setSlot(c, k, magicalType()); n--; }
    }
  }

  /** 513d:1c1d: a capital adds a strong army it cannot make yet -- strength
   *  5 or more, not a navy or an ally -- in its last empty slot. */
  void capitalArmies() {
    for (int s = 0; s < 8; s++) {
      int x = s16(sideRec(s) + 6), y = s16(sideRec(s) + 8);
      int c = -1;
      for (int i = 0; i < cityCount() && c < 0; i++) if (cx(i) == x && cy(i) == y) c = i;
      if (c < 0) continue;
      int o = crec(c);
      // the highest empty slot, else the last
      int k = 3;
      while (k >= 0 && scn[o + 22 + k] < 128) k--;
      if (k < 0) k = 3;
      int draws = 0, pick = -1;
      while (draws <= 39) {
        int t = dice(1, 28, -1);
        if (t == 5 || magical(t)) continue;
        draws++;
        bool has = false;
        for (int j = 0; j < 4; j++) if (scn[o + 22 + j] == t) has = true;
        const ArmyType* a = types.byId(t);
        if (!has && a && a->strength >= 5) { pick = t; break; }
      }
      // one found on the fortieth draw is dropped, as the original has it
      if (pick >= 0 && draws < 40) scn[o + 22 + k] = (uint8_t)pick;
    }
  }

  /** random_map_cities (513d:0ce0). */
  void cityDetails() {
    for (int c = 0; c < cityCount(); c++) {
      Surroundings f = surroundings(c);
      int v = value(c, f);
      cityText(c, v, f);
      production(c, v, f);
    }
    if (allies) allyProduction();
    capitalArmies();
  }

  /** auto_file_random_sgn (4fef:113d): 41-70 signposts on open plain off
   *  the roads, most pointing the way to the nearest city, a few with
   *  RANDOM.DAT's own words on them. */
  std::string signs() {
    int n = dice(1, 30, 40);
    std::vector<uint8_t> out(2 + 104 * n, 0);
    setI16(out, 0, n);
    auto putStr = [&](int o, const std::string& s) {
      for (size_t i = 0; i < s.size() && i < 49; i++) out[o + i] = (uint8_t)s[i];
    };
    int fixed = 0;
    for (int i = 0; i < n; i++) {
      int x = 0, y = 0;
      for (int tries = 0; tries < SAFETY; tries++) {
        x = dice(1, W, -1);
        y = dice(1, H, -1);
        if (terrainOf(tile(x, y)) == PLAIN && !near(x, y, CITY) && !near(x, y, TOWER) && road(x, y) == 0) break;
      }
      setTile(x, y, 0);
      int o = 2 + 104 * i;
      setI16(out, o, x); setI16(out, o + 2, y);
      if (dice(1, 10, 0) < 4 && fixed < r16(SIGNS)) {
        putStr(o + 4, str(SIGNS + 2 + 60 * fixed, 30));
        putStr(o + 54, str(SIGNS + 2 + 60 * fixed + 30, 30));
        fixed++;
      } else {
        int c = -1, best = 1000;
        for (int j = 0; j < cityCount(); j++) {
          int d = distance(x, y, cx(j), cy(j));
          if (d < best) { best = d; c = j; }
        }
        if (c < 0) continue;
        int cxx = cx(c), cyy = cy(c);
        putStr(o + 4, format(str(SIGN_CITY, 20), cstrOf(scn, crec(c) + 4, 16)));
        putStr(o + 54, format(str(SIGN_LEAGUES, 20), distance(x, y, cxx, cyy) * 2,
                              std::string(COMPASS[compass(x, y, cxx, cyy)])));
      }
    }
    return std::string(out.begin(), out.end());
  }

  // --- the whole -------------------------------------------------------------

  /** random_map_generate (4bed:011c), a phase a call: the progress figure
   *  the original's bar shows after it, or nothing once the world is made. */
  std::optional<int> next(Files& files) {
    switch (stage++) {
      case 0: return 0;
      case 1:
        // 4bed:036b: the map starts as sea
        for (int i = 0; i < W * H; i++) { m[GRID + i] = WATER; rd[i] &= 0xe0; }
        coast();
        return 10;
      case 2: highlands(); return 20;
      case 3: erosion(); return 30;
      case 4: rivers(); return 40;
      case 5: forests(); return 50;
      case 6: marshes(); return 60;
      case 7: cities(); return 70;
      case 8:
        sitesBegin();
        [[fallthrough]];
      case 9:
        // the sites, the bar moving on a tenth of the way through them each
        while (siteI < N_SITES) {
          int now = siteI * 10 / N_SITES;
          if (now != tenth) { tenth = now; stage = 9; return 70 + now; }
          site(siteI++);
        }
        return 80;
      case 10: return 81;
      case 11: roadTiles(); return 82;
      case 12: roadCrossings(); return 83;
      case 13:
        if (auto pct = roadsUntilShown()) { stage = 13; return *pct; }
        return 90;
      case 14:
        // 4fef:0014
        roadShapes();
        return 92;
      case 15: cityList(); capitals(); sides(); return 94;
      case 16: cityDetails(); return 96;
      case 17: sgn = signs(); return 98;
      case 18: {
        scn[S_RANDOM] = 1; scn[S_RANDOM + 1] = 0;
        std::string map(2 * W * H, '\0');
        for (int i = 0; i < W * H; i++) {
          map[2 * i] = (char)(tiles[i] & 0xff);
          map[2 * i + 1] = (char)(tiles[i] >> 8);
        }
        std::string road(W * H, '\0');
        for (int i = 0; i < W * H; i++) road[i] = (char)(rd[i] & 0x1f);
        files["SCN"] = std::string(scn.begin(), scn.end());
        files["MAP"] = map;
        files["RD"] = road;
        files["SGN"] = sgn;
        files["CTY"] = cty;
        files["SPC"] = spc;
        return 100;
      }
      default: return std::nullopt;
    }
  }
};

std::array<int, 4> settle(std::array<int, 4> sliders, Rng& rng) {
  for (int& v : sliders) {
    if (v == RANDOM_SLIDER) v = rng.dice(1, 7, -1);
  }
  return sliders;
}

namespace {
/** random_map_load_params (4bed:0000): RANDOM.DAT with the sliders added. */
std::vector<uint8_t> params(const std::string& dataDir, const std::array<int, 4>& sliders) {
  auto s = readFile(dataDir + "/RANDOM/RANDOM.DAT");
  if (!s) throw std::runtime_error("Random data not found");
  std::vector<uint8_t> m(s->begin(), s->end());
  if (m.size() < (size_t)DAT_SIZE) m.resize(DAT_SIZE, 0);
  auto add = [&](int o, int t, int i) { setI16(m, o, getI16(m, o) + getI16(m, t + 2 * i)); };
  int water = sliders[0], hills = sliders[1], cities = sliders[2], forest = sliders[3];
  add(P_CITIES, SLIDER_CITIES, cities);
  add(P_FOREST, SLIDER_FOREST, forest);
  add(P_HILLS, SLIDER_HILLS, hills);
  add(P_MOUNTAINS, SLIDER_HILLS, hills);
  add(P_WIDE_RIVERS, SLIDER_WATER, water);
  add(P_RIVERS, SLIDER_WATER, water);
  return m;
}
}  // namespace

Run::Run(const Options& opts) {
  if (!opts.rng) throw std::runtime_error("random map: no dice");
  auto sliders = settle(opts.sliders, *opts.rng);
  gen_ = std::make_unique<Generator>();
  Generator& g = *gen_;
  g.m = params(opts.dataDir, sliders);
  // auto_file_x_scn (4fef:1001): the scenario of the chosen terrain set,
  // Erythea for the only one there is, is the template
  auto t = readFile(opts.dataDir + "/ERYTHEA/ERYTHEA.SCN");
  if (!t) throw std::runtime_error("cannot open: ERYTHEA.SCN");
  g.scn.assign(t->begin(), t->end());
  armytype::load(opts.dataDir + "/TERRAIN0/ARMYTYPE.DAT", g.types);
  g.rng = opts.rng;
  g.allies = opts.allies;
  g.tiles.assign(W * H, 0);
  g.rd.assign(W * H, 0);
  g.scn[S_TERRAIN_SET] = (uint8_t)opts.terrainSet;
}

Run::~Run() = default;

bool Run::step() {
  auto pct = gen_->next(files_);
  if (!pct) return false;
  pct_ = *pct;
  return true;
}

Files generateNow(const Options& opts) {
  Run run(opts);
  while (run.step()) {}
  return run.files();
}

void install(const std::string& dataDir, const Files& files) {
  for (auto& [ext, bytes] : files) installFile(dataDir + "/" + DIR + "/" + DIR + "." + ext, bytes);
}

Files installed(const std::string& dataDir) {
  Files out;
  for (const char* ext : FILES) {
    if (auto b = readFile(dataDir + "/" + DIR + "/" + DIR + "." + ext)) out[ext] = *b;
  }
  return out;
}

std::string terrainSetName(const std::string& dataDir, int set) {
  auto s = readFile(dataDir + "/DATA/TERRAIN.DAT");
  if (!s || set >= u16(*s, 0)) return "";
  return cstr(*s, 2 + 52 * set + 2, 50);
}

int terrainSets(const std::string& dataDir) {
  auto s = readFile(dataDir + "/DATA/TERRAIN.DAT");
  return s ? std::max(1, u16(*s, 0)) : 1;
}

}  // namespace w2::randommap
