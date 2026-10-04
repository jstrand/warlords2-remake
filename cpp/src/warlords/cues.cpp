#include "warlords/cues.hpp"

#include <algorithm>
#include <map>

#include "warlords/armytype.hpp"

namespace w2::cues {

namespace {
// FILE.DAT group and index of each cue's song; index -1 is a random entry.
const std::map<int, std::pair<int, int>> SONG = {
    {0, {8, 0}}, {1, {10, -1}}, {3, {9, -1}}, {4, {12, 0}}, {5, {13, -1}}, {6, {14, -1}},
    {7, {15, 0}}, {8, {16, 0}}, {9, {17, 0}}, {10, {18, 0}}, {11, {19, 0}},
};
const std::map<int, int> LOSING = {{5, 40}, {10, 41}, {15, 42}, {20, 43}, {25, 44}, {30, 45}};
const std::map<int, int> WINNING = {{10, 48}, {15, 49}, {20, 50}, {25, 51}, {30, 52}, {35, 53}};

std::string entry(const uidata::Strings& files, int group, int index, const Roll& roll) {
  static const std::vector<std::string> none;
  const auto& g = group >= 0 && group < (int)files.size() ? files[group] : none;
  if (index < 0) index = roll((int)g.size()) - 1;
  if (index > (int)g.size() - 1) index = (int)g.size() - 1;
  if (index < 0 || index >= (int)g.size()) return "";
  return g[index];
}
}  // namespace

std::pair<std::string, bool> song(const uidata::Strings& files, int cue, const std::vector<Side*>& sides, const Roll& roll) {
  if (cue == COMPUTER) {
    // with a human still in play, group 11; in a game of computers alone, 5%
    // group 12, 47% group 11 and 48% group 10
    bool human = false;
    for (Side* s : sides) {
      if (s->alive && !s->computer) { human = true; break; }
    }
    int group = 11;
    if (!human) {
      int r = roll(100);
      if (r < 6) group = 12;
      else if (r >= 53) group = 10;
    }
    return {entry(files, group, -1, roll), false};
  }
  auto s = SONG.find(cue);
  if (s == SONG.end()) return {"", false};
  return {entry(files, s->second.first, s->second.second, roll), true};
}

int advisor(const Game& g, Side& side, const Roll& roll) {
  int cities = 0, heroes = 0;
  for (auto& c : g.map->cities) if (c.ownerIndex == side.index && !c.razed) cities++;
  if (cities > 39) return NONE;
  for (Army* a : g.armies) if (a->owner == side.index && a->type == armytype::HERO) heroes++;
  Advisor& adv = side.advisor;
  int group;
  if (cities < adv.mark) {
    int m = (cities / 5) * 5;
    adv.dir = 2;
    adv.mark = m;
    auto it = LOSING.find(m + 5);
    group = it != LOSING.end() ? it->second : 46;
  } else if (adv.mark + 5 <= cities) {
    int m = (cities / 5) * 5;
    adv.dir = 1;
    adv.mark = m;
    auto it = WINNING.find(m);
    group = it != WINNING.end() ? it->second : 47;
  } else {
    if (g.turn % 7 != 0) return NONE;
    if (side.gold < 100) group = 54;
    else if (side.gold > 2800) group = 55;
    else if (heroes < 1) group = 56;
    else if (heroes > 4) group = 57;
    else if (roll(5) == 1) group = 58;
    else return NONE;
  }
  if (g.turn == 1) return NONE;
  return group;
}

std::pair<std::string, std::string> clip(const uidata::Strings& files, int group, const Roll& roll) {
  if (group >= GREET) return {entry(files, group, 0, roll), entry(files, group, 1, roll)};
  static const std::vector<std::string> none;
  const auto& g = group >= 0 && group < (int)files.size() ? files[group] : none;
  int i = roll((int)g.size());
  const auto& t = group + 30 < (int)files.size() ? files[group + 30] : none;
  std::string sample = i >= 1 && i <= (int)g.size() ? g[i - 1] : "";
  int ti = std::min(i, (int)t.size()) - 1;
  std::string text = ti >= 0 && ti < (int)t.size() ? t[ti] : "";
  return {sample, text};
}

}  // namespace w2::cues
