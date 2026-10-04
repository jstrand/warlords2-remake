// Ruins, temples and sages: what they hold, and what searching them does.
//
// docs/rules.md > Ruins, temples and sages. The contents are rolled once at
// game start (setup_random_sites, 66d4:0000); searching is site_search,
// 6536:0000.
#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "warlords/types.hpp"

namespace w2 {

struct QuestResult;

/** What searching a site found (site.search). */
struct SearchResult {
  Site* site = nullptr;
  std::string kind;   // "temple", "no hero", "sage", "killed", "item", "gold", "allies", "empty"
  Army* hero = nullptr;
  int blessed = 0;
  std::shared_ptr<Quest> assigned;          // a temple's new quest
  std::shared_ptr<QuestResult> quest;       // what finding an item did for one
  const Monster* monster = nullptr;         // what killed the hero
  const Monster* guardian = nullptr;        // what the hero beat
  Item* item = nullptr;
  int gold = 0;
  std::vector<Army*> armies;
  const ArmyType* type = nullptr;
};

namespace site {

// what a site holds
constexpr int EMPTY = 0, TEMPLE = 1, ITEM = 2, SAGE = 3, GOLD = 4, ALLIES = 5;
extern const char* const CONTENT_NAMES[6];

constexpr int CAPITAL_RANGE = 15;       // "a capital within 15 tiles"
constexpr int RICH_SHARE = 3;           // sites * 3 / 10 are rich
constexpr int BLESSING_TEMPLES = 4;     // only the first four temples can bless
constexpr int SAGE_RANGE = 35;

/** Items that may only be hidden in a rich ruin (item_reserved, 66d4:08f4). */
bool itemReserved(int type, int value);
inline bool itemReserved(const Item& it) { return itemReserved(it.type, it.value); }
inline bool itemReserved(const PoolItem& it) { return itemReserved(it.type, it.value); }

/** Refill item records 8..21 from the scenario's .ITM pool (66d4:04ef). */
void fillItemPool(Game& g, int reserved);
/** Mark `sites * 3 / 10` non-temple sites rich (mark_rich_sites, 66d4:091e). */
void markRich(Game& g);
/** Roll what every site holds (setup_random_sites, 66d4:0000). */
void setup(Game& g);
/** Does the hero survive the ruin's guardian? */
bool survivesGuardian(Game& g, const Army& h, const std::vector<Army*>& stack, int monsterStrength);
/** The strength of a site's guardian, from the scenario's monster table. */
int guardianStrength(const Game& g, const Site& s);
/** Bless a stack at a temple: +1 strength (max 9), once per temple per army;
 *  only the first four temples can bless. */
int bless(Game& g, const Site& s, const std::vector<Army*>& stack);
/** Search the site under a stack (site_search, 6536:0000). A human chooses at
 *  a temple; a found item is left on the ground for Take to pick up. None
 *  when there is nothing to search. */
std::shared_ptr<SearchResult> search(Game& g, const std::vector<Army*>& stack, int x, int y, bool human = false);

// What a sage can tell a side, for its hero at (hx, hy) (6536:1610).
bool shownTo(const Site& s, const Side& side);
struct SageEntry {
  std::string kind;   // "gold", "allies", "item"
  std::string name;
  Item* item = nullptr;
};
std::vector<SageEntry> sageList(Game& g, const Side& side, int hx, int hy);
/** Show the side where an entry of sageList lies (6536:0e85). */
Site* sageShow(Game& g, const Side& side, const SageEntry& entry, int hx, int hy);
/** The sage's gem (6536:0b1a): 3d500 + 500 gold. */
int sageGem(Game& g, Side& side);
/** Uncover a patch of the map round the tile pointed at (6536:0cd6):
 *  {x, y, w, h} in tiles. */
std::array<int, 4> sageMap(Game& g, const Side& side, int cx, int cy);

}  // namespace site
}  // namespace w2
