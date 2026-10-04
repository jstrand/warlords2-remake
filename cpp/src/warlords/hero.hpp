// Heroes: offers, recruitment, experience, promotion and death.
//
// docs/rules.md > Heroes. This is where the first of the original's bugs
// shows up: see battleExperience and rules::bugs.
#pragma once

#include <string>
#include <utility>
#include <vector>

#include "warlords/types.hpp"

namespace w2 {

struct Battle;

/** A hero offering itself to a side (hero_offer_check). */
struct HeroOffer {
  int price = 0;
  City* city = nullptr;
  bool first = false;
  std::string name;
  bool female = false;
};

namespace hero {

constexpr int MAX_IN_GAME = 40;          // heroes in the whole game
constexpr int MAX_PER_SIDE = 5;          // 6 once the side owns this many cities
constexpr int MAX_PER_BIG_SIDE = 6, BIG_SIDE_CITIES = 40;
constexpr int START_STRENGTH = 5, START_MOVES = 14;
constexpr int MAX_STRENGTH = 9;
constexpr int PROMOTION_MOVES = 2;

/** What the levels are called, by level, 1-based as the game numbers them. */
const char* levelName(int level);
/** Experience needed to leave a level, or NONE at the top. */
int promotionAt(int level);

// Each side has its own hundred candidate heroes in TERRAIN0/HERONAM<side>.DAT,
// "#0 Sir Nick" a line. load_hero_name (6563:0c67) rolls dice(1, 100, 0); the
// count is hard-coded, so HERONAM4.DAT's 101st hero is never drawn.
constexpr int NAME_ROLL = 100;

/** The candidate list for a side, as {name, female}. Cached. */
const std::vector<std::pair<std::string, bool>>& names(Game& g, const Side& side);
/** Roll the name and sex of a hero offering itself to `side`. */
std::pair<std::string, bool> rollName(Game& g, const Side& side);
/** Does a hero offer itself to `side` this turn, and at what price? None when
 *  none does (hero_offer_check, 7563:0000). */
std::shared_ptr<HeroOffer> offer(Game& g, Side& side);
/** The allies a hired hero brings: 1-3 of one random magical type. */
std::pair<const ArmyType*, int> allies(Game& g);
/** One ally army, as create_ally_army (6536:12e6) builds it: moves full, no
 *  upkeep. */
Army newAlly(const ArmyType* type, int x, int y, int owner, int homeCity);
/** Hire the offered hero: the hero and its allies (hero_recruit, 7563:031b). */
std::pair<Army*, std::vector<Army*>> recruit(Game& g, Side& side, const HeroOffer& off);
/** Give a hero experience, capped at 60. */
void addExperience(Game& g, Army* h, int n);
/** Promote a side's heroes, one step each (hero_check_promotions, 7563:0579). */
std::vector<Army*> checkPromotions(Game& g, const Side& side);
/** Award experience after a battle. **Original bug**: a surviving defending
 *  hero is credited only when the attacker at the same place in the line is
 *  a hero (67cc:0c8e). */
void battleExperience(Game& g, const std::vector<Army*>& attackers, const std::vector<Army*>& defenders,
                      const Battle& result, bool wasCity);
/** What the hero info dialog lists for a hero (796c:03d9): the items it
 *  carries, then those lying on its tile, each in item order. */
std::vector<Item*> itemsHere(Game& g, const Army* h);
/** Put an item down on the hero's tile (7563:0943) -- or lose it at sea. */
void dropItem(Game& g, Army* h, Item* it);
/** Pick an item up off the hero's tile (7563:08c7). */
void takeItem(Game& g, Army* h, Item* it);
/** A dead hero drops everything it carried where it fell -- lost on water
 *  (hero_drop_items, 67cc:16cd). */
std::vector<Item*> dropItems(Game& g, Army* h, int x, int y);

}  // namespace hero
}  // namespace w2
