// Quests: taking one at a temple, checking it off, and the reward.
//
// docs/rules.md > Quests. quest_assign is 4976:0d7a, quest_check 4976:1ded
// and quest_choose_reward 4976:1909. One quest per side at a time.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "warlords/types.hpp"

namespace w2 {

/** What a finished quest paid (quest_choose_reward). */
struct Reward {
  std::string kind;   // "allies", "gold", "item", "revealed"
  std::vector<Army*> armies;
  const ArmyType* type = nullptr;
  int gold = 0;
  Item* item = nullptr;
  Site* site = nullptr;
};

/** A quest's end: done with a reward, or failed for a reason. */
struct QuestResult {
  std::shared_ptr<Quest> quest;
  std::shared_ptr<Reward> reward;
  std::string failed;     // "" when it was done
  int why = 0;            // the STRING.DAT group quest_check tells a human it with
};

namespace quest {

constexpr int SLAY_HERO = 0, RETRIEVE_ITEM = 1, SLAY_TYPE = 2;
constexpr int SLAUGHTER = 3, OCCUPY = 4, RAZE = 5, PILLAGE_GOLD = 6;

// DS:00a0, rolled with 1d10: types 4, 5 and 6 come up twice as often.
extern const std::vector<int> TYPE_TABLE;
extern const char* const DESCRIPTIONS[7];
constexpr int ITEM_RANGE = 50, CITY_RANGE = 60;
// what a quest's target *is*, so it can be saved and restored by reference
extern const char* const TARGET_KIND[7];
constexpr int EXPERIENCE = 10;

/** Take a quest at a temple, or none (quest_assign, 4976:0d7a). */
std::shared_ptr<Quest> assign(Game& g, Side& side, Army* h);
/** A line of prose for a quest. */
std::string describe(const Quest& q);

/** What just happened, for event(). */
struct Event {
  std::vector<Army*> stack;
  std::vector<Army*> killed;
  City* city = nullptr;
  int gold = 0;
  Army* hero = nullptr;
};
/** Tell the quest what just happened: "battle", "item", "pillage", "occupy",
 *  "raze" or "turn" (quest_check, 4976:1ded). */
std::shared_ptr<QuestResult> event(Game& g, Side& side, const std::string& ev, const Event& data = Event());
/** Choose and give the reward (quest_choose_reward, 4976:1909). */
std::shared_ptr<Reward> reward(Game& g, Side& side, const Quest* q);

}  // namespace quest
}  // namespace w2
