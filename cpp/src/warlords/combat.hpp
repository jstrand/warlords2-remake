// Combat: building the two lines, the modifiers, and the fight itself.
//
// docs/rules.md > Combat. combat_setup (6a89:008b) builds the lines,
// combat_terrain_class (6a89:0000) classifies the tile and combat_resolve
// (67cc:08a6) fights.
#pragma once

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "warlords/types.hpp"

namespace w2 {

struct QuestResult;

/** A battle, decided and then applied (combat.resolve, game.applyAttack). */
struct Battle {
  bool won = false;
  std::set<const Army*> deadByArmy;
  std::vector<int> log;        // 0 = a defender died, 1 = an attacker
  int attackMod = 0, defendMod = 0, cls = 0;
  std::vector<Army*> attackers, defenders;          // the survivors
  std::vector<Army*> deadAttackers, deadDefenders;
  // set by game::decideAttack
  struct Lines {
    std::vector<Army*> attackers, defenders;
    City* city = nullptr;
  } lines;
  int loot = 0;
  struct Fought {
    std::vector<Army*> stack;
    int x = 0, y = 0, defOwner = NONE;
  };
  std::optional<Fought> fought;   // until applied
  std::shared_ptr<QuestResult> quest;
  City* captured = nullptr;
};

namespace combat {

// terrain classes, in the order the ARMYTYPE bonus fields are stored
constexpr int CITY = 0, OPEN = 1, WOODS = 2, HILLS = 3;
extern const char* const CLASS_NAMES[4];

constexpr int SIEGE = 1, NEGATE_HERO = 2, NEGATE_OTHER = 3;

// hero strength -> command bonus, indexed 0..9
constexpr int HERO_TABLE[10] = {0, 0, 0, 0, 1, 1, 1, 2, 2, 3};

constexpr int HIT_POINTS = 2;
constexpr int MAX_STRENGTH = 15;
constexpr int DIE = 20, DIE_INTENSE = 24;
constexpr int STALEMATE = 10000;              // throws before the defender wins

/** The battle tile's terrain class (combat_terrain_class, 6a89:0000). */
int terrainClass(const Game& g, int x, int y);

struct Lines {
  std::vector<Army*> attackers, defenders;
  int defOwner = NONE;
  City* city = nullptr;
};
/** Build both battle lines for an attack on (x, y). */
Lines lines(const Game& g, const std::vector<Army*>& stack, int x, int y);

/** Battle items (+n in battle) carried by one army. */
int battleItems(const Army& army);
/** Command items carried by one army, a standard counting 1 (6a89:0d71). */
int commandItems(const Army& army);
/** The side's hero bonus: the table lookup on its strongest hero, plus every
 *  command item it carries. */
int heroBonus(const Game& g, const std::vector<Army*>& line);
/** The city's fortification bonus for the defender, or 0. */
int fortify(const Game& g, const std::vector<Army*>& attackers, int x, int y, int cls);
/** Both sides' modifiers: {attackMod, defendMod}. */
std::pair<int, int> modifiers(const Game& g, const std::vector<Army*>& attackers,
                              const std::vector<Army*>& defenders, int x, int y, int cls);
/** One army's fighting strength. A boat at sea on water or shore is 4. */
int strength(const Game& g, const Army& army, int mod, int cls, int terrain);
/** What View > Stack shows beside each army of the group that moves: its
 *  strength in a fight on (x, y) (89e0:1b9c). */
std::map<const Army*, int> stackStrengths(const Game& g, const std::vector<Army*>& armies, int x, int y);
/** Fight a battle. The armies are not removed; the caller applies the
 *  outcome (game::applyAttack). */
Battle resolve(Game& g, const std::vector<Army*>& attackers, const std::vector<Army*>& defenders, int x, int y);

/** The Military Advisor's verdict: 19 simulated battles, wins / 2 into
 *  STRING.DAT group 126 (military_advisor, 67cc:1f19). */
extern const char* const ADVICE[10];
constexpr int ADVICE_BATTLES = 19;
std::pair<std::string, int> advise(Game& g, const std::vector<Army*>& attackers,
                                   const std::vector<Army*>& defenders, int x, int y);

}  // namespace combat
}  // namespace w2
