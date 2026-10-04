// Diplomacy: the state between every pair of sides, and the proposals that
// change it.
//
// docs/rules.md > Diplomacy. A byte per ordered pair: the current state and
// this side's proposal. Proposals are applied at the start of the proposing
// side's turn (diplomacy_apply, 484e:0db3).
#pragma once

#include <map>
#include <string>
#include <vector>

#include "warlords/types.hpp"

namespace w2::diplomacy {

constexpr int PEACE = 0, INTERMEDIATE = 1, WAR = 2;
extern const char* const STATE_NAMES[3];

// STRING.DAT group 106, best first.
extern const char* const TITLES[8];
/** Which titles are used, by how many sides are in play (1-based ranks). */
const std::vector<int>& ratingRanks(int n);

/** War everywhere if Diplomacy is off, peace if it is on (484e:11bd). */
void init(Game& g);
/** The state between two sides. */
int state(const Game& g, int a, int b);
inline bool atWar(const Game& g, int a, int b) { return state(g, a, b) != PEACE; }
/** May `a` attack `b`? A side at peace may not. */
bool mayAttack(const Game& g, int a, int b);
/** Propose a state to another side. */
void propose(Game& g, int a, int b, int st);
/** The state a side proposes to another. */
int proposal(const Game& g, int a, int b);
/** Apply one side's proposals. Returns a list of messages. */
std::vector<std::string> apply(Game& g, Side& side);
/** At the end of a side's turn, its peace overtures count (484e:1063). */
void scoreUpdate(Game& g, Side& side);
/** What a peaceful move adds to the diplomatic score (484e:1063); NONE for nothing. */
int addScore(Game& g, Side& side, int from, int to);
/** Every side's diplomatic title, by side index (diplomatic_rating, 484e:0aed). */
std::map<int, std::string> ratings(const Game& g);

}  // namespace w2::diplomacy
