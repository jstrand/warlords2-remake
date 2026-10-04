// The figures behind the Report menu's five reports: Army, City, Gold,
// Production and Winning (6ef3:02fb). The drawing is ui/reports.
#pragma once

#include <array>

#include "warlords/types.hpp"

namespace w2::report {

constexpr int ARMY = 0, CITY = 1, GOLD = 2, PRODUCTION = 3, WINNING = 4;

/** The Winning report's score for a side: (gold + 5 income + upkeep + each
 *  city's income times defence) / 30, within 1..500. */
int score(const Game& g, const Side& side);

struct Figures {
  std::array<int, 8> value{};
  std::array<bool, 8> out{};
  int max = 0;
  int result = 0;
};
/** One report's figures. */
Figures figures(const Game& g, const Side& side, int n);

}  // namespace w2::report
