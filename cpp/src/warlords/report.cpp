#include "warlords/report.hpp"

#include <algorithm>

#include "warlords/game.hpp"

namespace w2::report {

int score(const Game& g, const Side& side) {
  int cities = 0;
  for (City* c : game::sideCities(g, side)) cities += c->defence * c->income;
  int s = (side.gold + game::income(g, side) * 5 + game::upkeep(g, side) + cities) / 30;
  return std::max(1, std::min(500, s));
}

Figures figures(const Game& g, const Side& side, int n) {
  Figures r;
  for (int i = 0; i < 8; i++) {
    const Side& s = g.map->sides[i];
    r.out[i] = !s.alive;
    r.value[i] = 0;
  }
  if (n == PRODUCTION) {
    r.result = (int)side.produced.size();
    return r;
  }
  if (n == ARMY) {
    for (Army* a : g.armies) {
      if (a->owner >= 0 && a->owner < 8) r.value[a->owner]++;
    }
  } else if (n == CITY) {
    for (int i = 0; i < 8; i++) r.value[i] = (int)game::sideCities(g, g.map->sides[i]).size();
  } else if (n == GOLD) {
    for (int i = 0; i < 8; i++) r.value[i] = g.map->sides[i].gold;
  } else if (n == WINNING) {
    int scores[8];
    for (int i = 0; i < 8; i++) {
      const Side& s = g.map->sides[i];
      scores[i] = s.inUse ? score(g, s) : 1;
    }
    int rank = 0;
    for (int i = 0; i < 8; i++) if (scores[side.index] < scores[i]) rank++;
    for (int i = 0; i < 8; i++) r.value[i] = scores[i] / 5;
    r.max = 100;
    r.result = rank;
    return r;
  }
  for (int i = 0; i < 8; i++) r.max = std::max(r.max, r.value[i]);
  // the top of the scale is the largest figure made even
  if (r.max > 0 && r.max / 2 == (r.max - 1) / 2) r.max++;
  r.result = r.value[side.index];
  return r;
}

}  // namespace w2::report
