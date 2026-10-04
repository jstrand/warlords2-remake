// TERRAIN0/ARMYTYPE.DAT -- the 29 army types.
// See docs/formats/armytype.md. Records are keyed by their type id (+0), not
// by position in the file: the file is in display order.
#pragma once

#include <string>

#include "warlords/types.hpp"

namespace w2::armytype {

// bonus field offsets worth naming (docs/formats/armytype.md, docs/rules.md)
constexpr int SIEGE = 52;   // value 1 = siege ability (+2 strength vs cities)
constexpr int FLIES = 54;
constexpr int WOODS_MOVE = 56;
constexpr int HILLS_MOVE = 58;
constexpr int BOAT = 60;

constexpr int NAVY = 5;     // removed from every city's production at game start
constexpr int HERO = 28;
constexpr int SCOUTS = 11;  // placeholder garrison when Neutral Cities is off

void load(const std::string& path, Types& out);

}  // namespace w2::armytype
