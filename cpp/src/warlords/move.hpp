// Movement: the cost grid, stack modes, pathfinding and walking a path.
//
// This follows WARLORD2.EXE: the original builds a one-byte-per-tile cost grid
// for the moving side (path_build_cost_grid, 1555:0d9e), runs a wavefront
// over it (path_wavefront, 1555:0373) and walks the resulting path
// (walk_path, 1a8b:07f9). See docs/rules.md > Movement and > Moving a stack.
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "warlords/types.hpp"

namespace w2::move {

// terrain type ids, as the scenario's own tile -> type table numbers them
constexpr int ROAD = 0, BRIDGE = 1, WATER = 2, SHORE = 3;
constexpr int FOREST = 4, HILLS = 5, MOUNTAINS = 6, PLAIN = 7;
constexpr int MARSH = 8, TOWER = 9, CITY = 10, SITE = 11;

extern const char* const TERRAIN_NAMES[12];

// DS:1274 in WARLORD2.EXE. 0 = impassable.
constexpr int COST[12] = {1, 1, 1, 2, 4, 6, 0, 2, 5, 2, 1, 2};

// cost-grid flags
constexpr int WATER_F = 0x08, CROSS_F = 0x10;
constexpr int HILLS_F = 0x20, FOREST_F = 0x40, CITY_F = 0x80;

// stack modes, as the original numbers them
constexpr int BOAT = 0, LAND = 1, FLYING = 2;

inline bool has(int byte, int flag) { return (byte & flag) != 0; }

constexpr int WATER_PENALTY = 10;       // a land stack's route leaving open water
constexpr int WATER_PENALTY_TO = 20;    // ... when the whole move is aimed at water
constexpr int PAST_SHORE = 0x80;        // a step after going to sea or ashore
constexpr int MIN_MOVE_LEFT = 2;        // the walk stops below this
constexpr int MAX_PATH = 200;           // a path is at most 200 compass steps

// compass directions, 0 = north, clockwise (the order the game stores paths in)
constexpr int DIRS[8][2] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};

constexpr int SHUT = 30001;

using Stack = std::vector<Army*>;

/** How a stack moves, and what bonuses it carries. */
struct Mode {
  int mode = LAND;
  bool woods = false, hills = false, atSea = false;
};
Mode stackMode(const Game& g, const Stack& stack);
/** Just the mode, for the many callers that want only it. */
inline int modeOf(const Game& g, const Stack& stack) { return stackMode(g, stack).mode; }

/** Is a city a port -- does any tile bordering its footprint hold a bridge,
 *  water or shore? Gives up the moment its walk would leave the map. */
bool isPort(const Game& g, const City& city);

/** The cost grid for one side (path_build_cost_grid, 1555:0d9e), cached on
 *  the game state; invalidate drops it. */
std::vector<int>& grid(Game& g, int sideIndex);
/** Drop the cached grids. Call after a city changes hands. */
void invalidate(Game& g);

/** The wavefront's weight for spreading from one tile to another, or none if
 *  it cannot spread there. This picks the route only. */
std::optional<int> stepCost(int fromByte, int toByte, int mode, bool woods, bool hills, int penalty);
/** The moves a walk spends stepping onto a tile (1555:19e4). */
int walkCost(int byte, int mode, bool woods, bool hills);
/** Does a land stack stepping onto this tile go to sea (or come ashore)? */
bool crossesShore(int byte, bool atSea);
/** The movement points a stack shares: the lowest of its armies'. */
int stackMoves(const Stack& stack);

struct Step {
  int x = 0, y = 0, cost = 0;
};
using Path = std::vector<Step>;

/** The route a stack would take to (x, y) and how much of it it can walk now. */
struct Preview {
  Path path;
  int reach = 0;
};
std::optional<Preview> preview(Game& g, const Stack& stack, int x, int y);

/** The compass direction from one tile towards another (1a8b:0ae4), or -1. */
int direction(int x1, int y1, int x2, int y2);
/** map_distance (1a8b:0acc): the straight-line distance, rounded down. */
int distance(int x1, int y1, int x2, int y2);

/** path_prepare_grid (1555:08bf): nothing reached, and what the stack can
 *  never enter shut. */
std::vector<int> prepare(Game& g, const std::vector<int>& grid, int sideIndex, int mode, int dx, int dy);

/** The path from (sx,sy) to (dx,dy) for a stack, or none if there is no
 *  route: 1555:000a, step for step. An empty path means "already there". */
std::optional<Path> findPath(Game& g, const Stack& stack, int sx, int sy, int dx, int dy);

struct Attack {
  int x = 0, y = 0;
  City* city = nullptr;
};
struct WalkResult {
  int steps = 0, spent = 0;
  std::string stopped;         // "arrived", "out of moves", "attack", "at peace", "blocked", "no route"
  std::optional<Attack> attack;
  std::vector<std::pair<int, int>> walked;
};

/** Going to sea and coming ashore, at the end of a walk (1a8b:04c8).
 *  Returns "to sea" or "ashore" when that happened, "" otherwise. */
std::string settleSea(Game& g, const Stack& stack, int x, int y, int mode, bool wasAtSea);
/** Walk `stack` along `path`; a tile left empty loses its encampment. */
WalkResult walk(Game& g, const Stack& stack, const Path& path);
/** Move a stack towards a tile: pathfind, then walk (move_stack_to, 1a8b:0001). */
WalkResult moveTo(Game& g, const Stack& stack, int x, int y);

}  // namespace w2::move
