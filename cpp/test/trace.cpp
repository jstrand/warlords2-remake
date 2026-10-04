// An all-computer game, one line per side's turn: the C++ half of the
// Lua-vs-C++ comparison (web/test/trace.lua prints the same lines).
//
//     cpp/build/w2trace ERYTHEA 1 10 [hidden] [save]
//
// Run from the repository root, so that original/ is found.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "warlords/ai.hpp"
#include "warlords/game.hpp"
#include "warlords/save.hpp"

using namespace w2;

static void trace(const Game& g, const Side& side) {
  long long h = 0;
  for (Army* a : g.armies) {
    h = (h * 31 + (a->x == NONE ? 999 : a->x) * 7 + (a->y == NONE ? 999 : a->y) * 13 + a->type * 17 +
         (a->owner == NONE ? 15 : a->owner) * 19 + a->strength + a->moves * 3) % 1000000007;
  }
  std::string owners;
  for (auto& c : g.map->cities) owners += c.razed ? 'x' : c.ownerIndex != NONE ? (char)('0' + c.ownerIndex) : '.';
  printf("%d %d gold=%d armies=%d rng=%u h=%lld %s\n", g.turn, side.index, side.gold, (int)g.armies.size(),
         g.rng.state, h, owners.c_str());
}

int main(int argc, char** argv) {
  std::string scen = argc > 1 ? argv[1] : "ERYTHEA";
  double seed = argc > 2 ? atof(argv[2]) : 1;
  int turns = argc > 3 ? atoi(argv[3]) : 10;
  bool hidden = false, roundTrip = false;
  for (int i = 4; i < argc; i++) {
    if (!strcmp(argv[i], "hidden")) hidden = true;
    // "save": write the game out and read it back after every turn, which
    // must change nothing
    if (!strcmp(argv[i], "save")) roundTrip = true;
  }
  game::NewGameOptions opts;
  opts.seed = seed;
  if (hidden) opts.options.emplace_back("hiddenMap", 1);
  auto g = game::newGame("original", scen, opts);
  for (Side* s : g->sides) s->computer = true;
  Side* side = game::begin(*g);
  while (side && g->turn <= turns) {
    ai::playTurn(*g, *side);
    trace(*g, *side);
    if (getenv("DUMP")) {
      int i = 1;
      for (Army* a : g->armies) {
        auto v = [](int n) { return n == NONE ? std::string("nil") : std::to_string(n); };
        printf("%d\t%s\t%s\t%d\t%s\t%d\t%d\n", i++, v(a->x).c_str(), v(a->y).c_str(), a->type, v(a->owner).c_str(), a->strength, a->moves);
      }
    }
    side = game::endTurn(*g);
    if (roundTrip && side) {
      g = save::decode(save::encode(*g), "original");
      side = g->side;
    }
  }
  return 0;
}
