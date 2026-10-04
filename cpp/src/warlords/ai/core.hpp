// The computer players' shared machinery: their per-side data, the city
// neighbour table, the stack they have "selected", the flood fill they
// measure distances with, the battles they simulate, and the walk that moves
// a stack and fights what it meets.
//
// A port of WARLORD2.EXE; the addresses are Ghidra's (docs/re/ai.md).
#pragma once

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <tuple>
#include <vector>

#include "warlords/combat.hpp"
#include "warlords/move.hpp"
#include "warlords/rules.hpp"
#include "warlords/types.hpp"

namespace w2 {

/** An assault group (5f19:08ba). Its lists are keyed 1-6 (members and staged
 *  1-4) with holes, as the original's fixed slots are. */
struct Group {
  int active = 0;
  int target = NONE;           // the side it goes for
  int rally = NONE;            // the city it gathers at
  std::map<int, int> members, cities, dist, bonus, taken;   // city indices, distances
  std::map<int, Army*> staged;
  int size = 0, flags = 0;
};

/** A side's block of AI data (ai_init_side, 59bf:084d). */
struct AIData {
  int turns = 0;
  int own = 0, enemy = 0, neutral = 0, unseen = 0;
  int rebuildLimit = 10;
  bool bold = false;
  int heroes = 0;
  int rebuildType = 0, rebuildTypeRich = 0;
  int bought = 0;
  int dieHuman = 0, dieLord = 0, dieKnight = 0, dieWarlord = 0;
  int sims = 10;
  int searchers = 0, explorers = 0;
  int raze = 0, sack = 0, pillage = 0, perCity = 0;
  int bonusHuman = 0, bonusWarlord = 0, bonusLord = 0, bonusKnight = 0;
  int poor = 0;
  int solidarity = 0;
  int minStrength = 0, flyCities = 0, strongCities = 0, fastCities = 0;
  int early = 0;
  int humanShare = 80;
  int questCity = NONE;
  int cautious = 0;
  int questsDone = 0, itemsPassed = 0;
  int maxGroups = 1;
  std::map<int, int> roles, held, flags, garrison, keep;   // by city index
  // 1-based, as the original numbers them: an army's aiGroup is the number,
  // and 0 means none. Held by pointer as the Lua holds tables: cancelling a
  // group puts a fresh one in its slot, and whoever still holds the old one
  // goes on with it, as the original's code does.
  std::array<std::shared_ptr<Group>, 5> groups;
  std::array<int, 8> heroesKilled{}, armiesKilled{}, battles{}, lost{}, cityBattles{}, citiesLost{};
  std::optional<std::pair<int, int>> cursor;
};

/** Each city's six nearest cities by land path, by city index. */
struct Neighbours {
  struct Entry {
    std::vector<int> cities;
    std::vector<int> dist;
  };
  std::vector<Entry> byCity;
};

namespace ai::core {

constexpr int NEUTRAL = rules::NEUTRAL;

// city roles, AI data +0x56 + city (docs/re/ai.md > City roles)
constexpr int JUST_TAKEN = 1, TAKING_NEUTRAL = 2, NEAR_NEUTRAL = 3;
constexpr int WEAK = 4, BUILDING = 5, MEMBER = 6, RALLY = 7;
constexpr int STOP = 8, EXPLORER2 = 11, EXPLORER = 13, NOWHERE = 14;

// an army's standing order, the low nibble of record +14
constexpr int ORDER_NONE = 0, ORDER_CITY = 1, ORDER_ITEM = 2;
constexpr int ORDER_SITE = 3, ORDER_ROAM = 4;

// per-city AI flags, +0x11e + city
constexpr int CF_UNSEEN = 0x01, CF_FLIER = 0x02, CF_CLEANED = 0x04;
constexpr int CF_MOVE12 = 0x08, CF_MAGIC = 0x10, CF_HERO = 0x20;
constexpr int CF_FLYGROUP = 0x40, CF_MOVE16 = 0x80;

// an assault group's flags, entry +0x5a
constexpr int GF_RAZE = 0x01, GF_SACK = 0x02, GF_PILLAGE = 0x04;
constexpr int GF_MOVE16 = 0x08, GF_MOVE12 = 0x10, GF_FLY = 0x20;

constexpr int MAX_GROUPS = 4;
constexpr int UNREACHED = 30001;

inline bool has(int v, int bit) { return (v & bit) != 0; }
inline int set(int v, int bit) { return v | bit; }
inline int clear(int v, int bit) { return v & ~bit; }
inline int get(const std::map<int, int>& m, int k) {
  auto it = m.find(k);
  return it == m.end() ? 0 : it->second;
}
inline bool hasKey(const std::map<int, int>& m, int k) { return m.count(k) > 0; }

/** map_distance (2012:1199): the straight-line distance, truncated. */
int dist(int x1, int y1, int x2, int y2);
inline int owner(const City& c) { return c.ownerIndex == NONE ? NEUTRAL : c.ownerIndex; }
/** Is the city still a city (not ruins)? */
inline bool standing(const City& c) { return !c.razed; }
/** Is a side in the game? */
bool inPlay(const Game& g, int index);
bool isComputer(const Game& g, int index);
/** The diplomatic state between two sides. */
int state(const Game& g, int a, int b);
/** A side's proposal to another. */
int proposal(const Game& g, int a, int b);
void propose(Game& g, int a, int b, int v);
/** The city whose footprint covers (x, y). */
City* cityAt(const Game& g, int x, int y);
/** The site on (x, y) (828e:04b5). */
Site* siteAt(const Game& g, int x, int y);
int terrain(const Game& g, int x, int y);
/** A temple, or a ruin nobody has searched. */
bool siteOpen(const Site& s);
/** Has the side seen (x, y), or any tile next to it (623c:16ae)? */
bool explored(const Game& g, int sd, int x, int y);
bool flies(const Game& g, const Army* a);
bool magical(const Game& g, const Army* a);
int ability(const Game& g, const Army* a);
bool woods(const Game& g, const Army* a);
bool hills(const Game& g, const Army* a);
inline bool isHero(const Army* a) { return a->hero(); }
/** The side's armies, last record first. */
std::vector<Army*> armies(const Game& g, int sideIndex);
/** Every placed army of a side on (x, y), last record first. */
std::vector<Army*> onTile(const Game& g, int sideIndex, int x, int y);
/** Is an army still in the game? */
inline bool alive(const Game& g, const Army* a) { return g.alive(a); }

/** A fresh block of AI data (ai_init_side, 59bf:084d). */
AIData newData();
/** An empty assault group (5f19:08ba). */
inline std::shared_ptr<Group> emptyGroup() { return std::make_shared<Group>(); }
/** The AI data of a side, made on first use. */
AIData& data(Game& g, Side& sd);
AIData& data(Game& g, int sideIndex);
inline int role(const AIData& d, const City& c) { return get(d.roles, c.index); }
inline void setRole(AIData& d, const City& c, int r) { d.roles[c.index] = r; }
inline bool cflag(const AIData& d, const City& c, int bit) { return has(get(d.flags, c.index), bit); }
inline void setCflag(AIData& d, const City& c, int bit) { d.flags[c.index] = set(get(d.flags, c.index), bit); }
inline void clearCflag(AIData& d, const City& c, int bit) { d.flags[c.index] = clear(get(d.flags, c.index), bit); }

/** A city's neighbours: {cities, dists}. */
std::pair<std::vector<City*>, std::vector<int>> neighbours(Game& g, const City& c);
/** For every city: flood by land from it (radius 45, then 60) and keep six
 *  neighbours, spread over the four directions (623c:0749). */
std::shared_ptr<Neighbours> buildNeighbours(Game& g);

/** The stack a computer has "selected". */
struct Sel {
  std::vector<Army*> armies;
  Army* leader = nullptr;
  int minMoves = 0;
  int mode = move::LAND;
  Army* hero = nullptr;
};
/** The movement points a selection shares, and how it moves. */
Sel& refresh(Game& g, Sel& sel);
/** Select a list of armies as one stack (623c:14d3). The leader is the first. */
Sel select(Game& g, const std::vector<Army*>& list);
/** The lowest group number from 2 up that none of the side's armies uses
 *  (1b62:0cde). */
int newGroup(const Game& g, int sideIndex);
/** Select a list and give it an order (623c:0ae7); none for an empty one. */
std::optional<Sel> order(Game& g, const std::vector<Army*>& list, int ord, int dest, int flags);
/** Where an order points an army (623c:0c7e). */
void setTarget(Game& g, Army* a, int ord, int dest);
/** Forget an army's order. */
inline void clearOrder(Army* a) { a->aiOrder = ORDER_NONE; a->aiDest = NONE; }
/** Select the stack an army moves with (8c07:06eb); they are marked moved. */
Sel selectStackOf(Game& g, Army* a);
/** The side's armies on a tile with at least `minMoves` moves left, up to
 *  eight (623c:12c9). */
std::vector<Army*> collect(const Game& g, int sideIndex, int x, int y, int minMoves);
/** The side's armies on a tile in group nibble `grp` with standing order
 *  `ord`, and a maximum move of at least `minMax` (623c:13b2). */
std::vector<Army*> collectOrdered(const Game& g, int sideIndex, int x, int y, int grp, int ord, int minMax);

/** A flood's path costs from its start. */
struct Flood {
  std::vector<int> dist;   // -1 where it did not reach
  int W = 0, H = 0;
};
/** Flood the map from (x, y) the way the selection moves (1555:1ab9). */
Flood floodFrom(Game& g, int sideIndex, int x, int y, int range, const Sel* sel);
Flood floodFrom(Game& g, int sideIndex, int x, int y, int range, int mode);
int floodAt(const Flood& flood, int x, int y);
/** The flood's distance to a city: {best, bx, by}, the least over the twelve
 *  tiles round it, or 100 (1000 with `far`) when none is closer, bx NONE
 *  then (59bf:0a85). */
std::tuple<int, int, int> cityDistance(const Game& g, const Flood& flood, const City& c, bool far = false);
/** The chance, in percent, that the selection takes (x, y) (623c:15c5). */
int odds(Game& g, const Sel& sel, int x, int y);
/** A hero picks up whatever lies where it stands (6087:0427). */
void pickUp(Game& g, const Sel& sel);
/** Fight for (x, y) with the selection, for real. */
Battle fight(Game& g, Sel& sel, int x, int y);
/** Mark the selection done for the turn (8c07:08fb). */
void done(const Sel& sel);
/** move_stack_to (1a8b:0001) for a computer: walk to (tx, ty), fighting what
 *  the walk runs into, until the walk stops. Returns the last walk code. */
int moveTo(Game& g, Sel& sel, int tx, int ty, bool keep = false);
/** Attack a city the walk has run into (1a8b:0254): {again, x, y}. */
std::tuple<bool, int, int> attackCity(Game& g, Sel& sel, City* dest, int ax, int ay);
/** Is the city worth attacking with this stack, or is there better nearby
 *  (623c:1771)? */
City* retarget(Game& g, Sel& sel, City* c, int x, int y);
/** The best city for the selection to go for, by the flood (623c:1885). */
City* bestCity(Game& g, const Sel& sel, const Flood& flood, bool skipOwn);

}  // namespace ai::core
}  // namespace w2
