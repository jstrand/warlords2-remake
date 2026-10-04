// The game's history: what the History menu plays back.
//
// docs/formats/history.md. Once a round, when the turn counter goes up
// (8065:17f6 -> 6d51:0d60), a record is taken of every side's gold, score
// and city count, every city's owner, and the round's deeds -- kept as they
// happen, two a side, a lower type counting for more (6d51:1244).
#pragma once

#include <string>

#include "warlords/types.hpp"

namespace w2::history {

// the deed types, which are also their rank: lower counts for more
constexpr int EMERGES = 0, KILLED = 1, QUEST_DONE = 2, QUEST_GIVEN = 3;
constexpr int VANQUISHED = 4, WON = 5, FINDS = 6, VICTORIOUS = 7;
constexpr int TREACHERY = 8, WAR = 9, PEACE = 10;
// first values with a meaning of their own
constexpr int IN_BATTLE = -1, SEARCHING = -2;          // KILLED
constexpr int BY_NAME = -1;                            // WON: the name won it
constexpr int ALLIES = 100, SAGE = 101, GOLD = 102;    // FINDS

constexpr int LAST_TURN = 201;
constexpr int MAX_EVENTS = 10;

/** Note a deed of a side's (6d51:1244). `name` is the hero's or the side's. */
void deed(Game& g, const Side* side, int type, int v1, int v2, const std::string& name);

// What History > Triumphs counts: a side's own row counts what it lost; its
// row for another side, what it killed of them (67cc:1b43-1e92).
constexpr int ARMIES = 0, CREATURES = 1, HEROES = 2, NAVIES = 3, STANDARDS = 4;

/** An army of its owner's killed by `killer`'s. */
void tally(Game& g, const Army* army, int killer);
/** The count for side `me` against `opp`, kind k. */
int triumph(const Game& g, int me, int opp, int k);
/** The round's record (6d51:0d60), taken as the turn counter goes up. */
void record(Game& g);

}  // namespace w2::history
