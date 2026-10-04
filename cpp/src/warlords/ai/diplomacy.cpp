#include "warlords/ai/diplomacy.hpp"

#include <algorithm>

#include "warlords/ai/core.hpp"
#include "warlords/ai/groups.hpp"

namespace w2::ai::diplo {

using namespace core;

bool canAttack(Game& g, Side& side, const City& c) {
  int own = owner(c);
  if (own == NEUTRAL) return true;
  AIData& d = data(g, side);
  int wars = 0, last = NONE;
  for (int s = 7; s >= 0; s--) {
    if (inPlay(g, s) && state(g, side.index, s) == 2) { wars++; last = s; }
  }
  int st = state(g, side.index, own);
  if (st == 2) return true;
  if (st == 0 && wars != 0 && (wars != 1 || last != own)) return false;
  if (st > 2) return false;
  return d.bold;
}

bool spared(Game& g, Side& side, int other) {
  AIData& d = data(g, side);
  if (g.map->options.diplomacy == 0 || d.solidarity == 0 || !side.computer) return false;
  for (int h = 7; h >= 0; h--) {
    const Side& hs = g.map->sides[h];
    if (inPlay(g, h) && !hs.computer && state(g, other, h) == 2 && hs.aiSolidarity != 0) return true;
  }
  return false;
}

// Claim one more city as the side's own ground (558d:0917).
static void claim(Game& g, Side& side) {
  City* cap = side.capital;
  if (!cap) return;
  City* best = nullptr;
  int bestD = NONE;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    int cl = c.claim != NONE ? c.claim : NEUTRAL;
    if (standing(c) && cl != side.index && (cl == NEUTRAL || !inPlay(g, cl))) {
      int dd = dist(c.x, c.y, cap->x, cap->y);
      if (dd < 40 && (bestD == NONE || dd < bestD)) { best = &c; bestD = dd; }
    }
  }
  if (best) best->claim = side.index;
}

void phase(Game& g, Side& side) {
  AIData& d = data(g, side);
  int me = side.index;
  bool human = !side.computer;
  claim(g, side);
  if (g.map->options.diplomacy == 0) return;
  int enemy = groups::pickEnemy(g, side);

  int swapped[16] = {0}, total[16] = {0}, seen[16] = {0}, theyWar[8] = {0}, theyPeace[8] = {0};
  for (int b = 7; b >= 0; b--) {
    propose(g, me, b, 0);
    int p = proposal(g, b, me);
    if (p == 2) theyWar[b] = 1;
    else if (p == 0) theyPeace[b] = 1;
  }
  int tiles = 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (standing(c)) tiles++;
    int o = c.ownerIndex;
    if (o != NONE) {
      total[o]++;
      if (!cflag(d, c, CF_UNSEEN)) seen[o]++;
      int cl = c.claim != NONE ? c.claim : NEUTRAL;
      if (o == me) {
        if (cl != NEUTRAL && cl >= 0 && cl < 16) swapped[cl]++;
      } else if (cl == me) {
        swapped[o]++;
      }
    }
  }
  // the grudge tests; each has a war branch that can never run (docs/re/ai.md)
  for (int b = 7; b >= 0; b--) {
    if (b != me && inPlay(g, b)) {
      int grudge = (theyPeace[b] - theyWar[b]) * 2;
      if (swapped[b] >= grudge + 4) propose(g, me, b, 1);
    }
  }
  for (int b = 7; b >= 0; b--) {
    if (b != me && inPlay(g, b)) {
      int grudge = (theyPeace[b] - theyWar[b]) * 2;
      int threat = d.lost[b] + d.citiesLost[b] * 4 + d.heroesKilled[b] * 2;
      if (threat >= grudge + 5) propose(g, me, b, std::max(1, proposal(g, me, b)));
    }
  }
  if (enemy != NONE) propose(g, me, enemy, 2);
  for (int gi = MAX_GROUPS; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && grp->target != NONE) propose(g, me, grp->target, 2);
  }
  // whoever holds too much of the world
  int leader = NONE, most = 0;
  for (int s = 7; s >= 0; s--) {
    if (inPlay(g, s)) {
      int share = total[s] * 100 / std::max(1, tiles);
      int limit = isComputer(g, s) ? 50 : d.humanShare;
      if (most < total[s] && limit < share) { leader = s; most = total[s]; }
    }
  }
  if (leader != NONE && leader != me) {
    for (int s = 7; s >= 0; s--) {
      if (inPlay(g, s)) propose(g, me, s, (s == leader || s == enemy) ? 2 : 0);
    }
  }
  if (!human && d.solidarity != 0) {
    for (int s = 7; s >= 0; s--) {
      if (inPlay(g, s) && isComputer(g, s) && s != me && proposal(g, me, s) != 0 && spared(g, side, s)) {
        propose(g, me, s, 0);
      }
    }
    bool atWarWithHuman = false;
    for (int s = 7; s >= 0; s--) {
      if (inPlay(g, s) && !isComputer(g, s) && state(g, me, s) == 2) atWarWithHuman = true;
    }
    if (atWarWithHuman) {
      for (int s = 7; s >= 0; s--) {
        if (inPlay(g, s) && isComputer(g, s) && g.map->sides[s].aiSolidarity != 0) propose(g, me, s, 0);
      }
    }
  }
  if (!human && g.greatest) {
    auto fights = groups::fightingHumans(g);
    for (int s = 7; s >= 0; s--) {
      if (inPlay(g, s) && isComputer(g, s) && (fights[me] || fights[s])) propose(g, me, s, 0);
    }
  }
  for (int b = 7; b >= 0; b--) if (seen[b] == 0) propose(g, me, b, 0);
  for (int gi = MAX_GROUPS; gi >= 1; gi--) {
    auto grp = d.groups[gi];
    if (grp->active != 0 && grp->target != NONE && proposal(g, me, grp->target) == 0) groups::cancel(g, side, gi);
  }
}

}  // namespace w2::ai::diplo
