#include "warlords/diplomacy.hpp"

#include "util/util.hpp"
#include "warlords/game.hpp"
#include "warlords/history.hpp"

namespace w2::diplomacy {

const char* const STATE_NAMES[3] = {"peace", "uneasy", "war"};
const char* const TITLES[8] = {"Statesman", "Diplomat", "Pragmatist", "Politician",
                               "Deceiver", "Scoundrel", "Turncoat", "Running Dog"};

const std::vector<int>& ratingRanks(int n) {
  static const std::vector<int> ranks[9] = {
      {}, {1}, {1, 8}, {1, 4, 8}, {1, 2, 6, 8}, {1, 2, 4, 6, 8},
      {1, 2, 4, 6, 7, 8}, {1, 2, 4, 5, 6, 7, 8}, {1, 2, 3, 4, 5, 6, 7, 8}};
  if (n < 1 || n > 8) return ranks[8];
  return ranks[n];
}

static int key(int a, int b) { return a * 8 + b; }

void init(Game& g) {
  int start = g.map->options.diplomacy != 0 ? PEACE : WAR;
  for (int a = 0; a < 8; a++) {
    for (int b = 0; b < 8; b++) {
      int v = a == b ? PEACE : start;
      g.diplomacy.state[key(a, b)] = v;
      g.diplomacy.proposal[key(a, b)] = v;
    }
  }
}

int state(const Game& g, int a, int b) {
  if (a < 0 || b < 0 || a > 7 || b > 7) return WAR;
  int v = g.diplomacy.state[key(a, b)];
  return v == NONE ? WAR : v;
}

bool mayAttack(const Game& g, int a, int b) {
  if (a == NONE || b == NONE) return true;        // neutrals are fair game
  return state(g, a, b) != PEACE;
}

void propose(Game& g, int a, int b, int st) {
  if (a == b || a < 0 || b < 0 || a > 7 || b > 7) return;
  g.diplomacy.proposal[key(a, b)] = st;
}

int proposal(const Game& g, int a, int b) {
  if (a < 0 || b < 0 || a > 7 || b > 7) return state(g, a, b);
  int p = g.diplomacy.proposal[key(a, b)];
  if (p == NONE) p = state(g, a, b);
  return p;
}

std::vector<std::string> apply(Game& g, Side& side) {
  std::vector<std::string> messages;
  int a = side.index;
  for (int b = 0; b < 8; b++) {
    Side* other = g.map->side(b);
    if (b != a && other) {
      int now = state(g, a, b);
      int want = proposal(g, a, b);
      if (want != now) {
        if (want > now) {                            // escalation: at once
          g.diplomacy.state[key(a, b)] = want;
          g.diplomacy.state[key(b, a)] = want;
          if (proposal(g, b, a) < want) g.diplomacy.proposal[key(b, a)] = want;
          if (want == WAR) {
            history::deed(g, &side, history::WAR, a, b, "");          // 484e:0f15
            messages.push_back("War declared with " + other->name + "!");
          }
        } else if (proposal(g, b, a) <= want) {      // de-escalation: mutual
          g.diplomacy.state[key(a, b)] = want;
          g.diplomacy.state[key(b, a)] = want;
          if (want == PEACE) {
            history::deed(g, &side, history::PEACE, a, b, "");         // 484e:1027
            messages.push_back("Peace negotiated with " + other->name + "!");
          }
        }
      }
    }
  }
  return messages;
}

void scoreUpdate(Game& g, Side& side) {
  int a = side.index;
  for (int b = 0; b < 8; b++) {
    if (b != a) {
      int p = proposal(g, a, b), st = state(g, a, b);
      if (p < st && p < proposal(g, b, a)) addScore(g, side, st, p);
    }
  }
}

int addScore(Game& g, Side& side, int from, int to) {
  int gain = NONE;
  if (from == WAR && to == PEACE) gain = g.rng.dice(1, 10, 10);
  else if (to < from) gain = g.rng.dice(1, 2, 1);
  if (gain != NONE) game::addDiploScore(g, side, gain);
  return gain;
}

std::map<int, std::string> ratings(const Game& g) {
  struct E { Side* side; int score; int seq; };
  std::vector<E> order;
  for (size_t i = 0; i < g.sides.size(); i++) order.push_back(E{g.sides[i], g.sides[i]->diploScore, (int)i});
  luaSort(order, [](const E& p, const E& q) { return p.score != q.score ? p.score < q.score : p.seq < q.seq; });
  const std::vector<int>& ranks = ratingRanks((int)order.size());
  std::map<int, std::string> out;
  for (size_t i = 0; i < order.size(); i++) {
    int r = i < ranks.size() ? ranks[i] : 8;
    out[order[i].side->index] = TITLES[r - 1];
  }
  return out;
}

}  // namespace w2::diplomacy
