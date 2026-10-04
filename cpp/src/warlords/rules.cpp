#include "warlords/rules.hpp"

#include <algorithm>
#include <cstdlib>

#include "util/util.hpp"

namespace w2::rules {

Bugs bugs;

const Purpose& purpose(int p) {
  static const Purpose table[7] = {
      {0, 0, 0},
      {10, 4, 1},    // quick and cheap
      {10, 10, 1},   // balanced
      {5, 10, 1},    // strongest
      {5, 10, 1},    // flying types only
      {5, 10, 1},
      {10, 1, 10},   // fastest
  };
  return table[std::clamp(p, 0, 6)];
}

std::vector<Slot> citySlots(const std::vector<int>& produceIds, const Types& types, Rng& rng) {
  std::vector<Slot> slots;
  for (int id : produceIds) {
    const ArmyType* a = types.byId(id);
    int strength = a->strength, time = a->time, cost = a->cost, move = a->move;
    if (rng.chance(10)) {
      if (rng.chance(60)) strength = std::min(9, strength + 1);
      else strength = std::max(1, strength - 1);
    }
    if (rng.chance(20)) {
      int r = rng.dice(1, 100, 0);
      move += r < 10 ? 4 : r < 60 ? 2 : r < 95 ? -2 : -4;
      move = std::max(2, move);
    }
    move = std::max(6, move);
    if (rng.chance(10)) {
      int quarter = cost / 4;
      cost += rng.chance(60) ? -quarter : quarter;
    }
    if (rng.chance(10)) {
      time = rng.chance(60) ? std::max(1, time - 1) : time + 1;
    }
    slots.push_back(Slot{id, a->name, strength, time, cost, move, std::abs(a->price)});
  }
  luaSort(slots, [](const Slot& p, const Slot& q) { return p.price < q.price; });
  return slots;
}

int bestSlot(const std::vector<Slot>& slots, int purpose, const Types& types, bool sideBonus) {
  const Purpose& w = rules::purpose(purpose);
  int best = NONE, bestScore = 0;
  for (int i = (int)slots.size() - 1; i >= 0; i--) {
    const Slot& slot = slots[i];
    const ArmyType* a = types.byId(slot.type);
    if (purpose != 4 || a->flies) {
      int str = std::min(9, slot.strength + (sideBonus ? 2 : 0));
      if (a->siege) str += 2;
      int time = slot.time + ((str < 3 && purpose != 6) ? 1 : 0);
      int score = (10 - std::min(10, time)) * w.time + str * w.str + (slot.move * w.move) / 2;
      if (score > bestScore) { best = i; bestScore = score; }
    }
  }
  return best;
}

std::optional<int> garrisonLevel(bool owned, int neutralCities, Rng& rng) {
  if (owned) return 3;
  if (neutralCities <= 0) return std::nullopt;
  return std::min(3, rng.dice(1, 4, 0) + neutralCities - 2);
}

Army armyFromSlot(const Slot& slot, bool enhanced) {
  Army a;
  a.type = slot.type;
  a.name = slot.name;
  a.strength = enhanced ? std::min(9, slot.strength + 2) : slot.strength;
  a.maxMoves = slot.move;
  a.upkeep = slot.cost / 2;
  return a;
}

}  // namespace w2::rules
