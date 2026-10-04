// Saving and loading a game in progress.
//
// The C++ port's own format, JSON. Only what cannot be recomputed is written:
// everything derived from the data files -- the map, the army types, the
// terrain -- is reloaded from them. Armies are written with an id so the
// references between them (a quest's hero, a staged stack) survive.
#pragma once

#include <memory>
#include <string>

#include "warlords/types.hpp"

namespace w2::save {

constexpr int VERSION = 1;

/** A game as a string. */
std::string encode(const Game& g);
/** Rebuild a game from a saved string; throws std::runtime_error. */
std::unique_ptr<Game> decode(const std::string& text, const std::string& dataDir);

}  // namespace w2::save
