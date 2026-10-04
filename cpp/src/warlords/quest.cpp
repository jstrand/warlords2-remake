#include "warlords/quest.hpp"

#include <algorithm>
#include <cstdlib>

#include "util/util.hpp"
#include "warlords/armytype.hpp"
#include "warlords/game.hpp"
#include "warlords/hero.hpp"
#include "warlords/history.hpp"
#include "warlords/site.hpp"

namespace w2::quest {

const std::vector<int> TYPE_TABLE = {0, 1, 2, 3, 4, 5, 6, 4, 5, 6};
const char* const DESCRIPTIONS[7] = {
    "slay the enemy hero",
    "retrieve a magic item",
    "slay a unit of an enemy army type",
    "slaughter %d armies of one side",
    "force a city into submission and occupy it",
    "conquer a city and raze it",
    "sack and pillage %d gold",
};
const char* const TARGET_KIND[7] = {"army", "item", "armytype", "side", "city", "city", "none"};

namespace {
std::vector<Side*> otherSides(Game& g, const Side& side) {
  std::vector<Side*> out;
  for (Side* s : g.sides) if (s->alive && s->index != side.index) out.push_back(s);
  return out;
}

// Pick a target for a quest of this type into q; false if there is none.
bool pickTarget(Game& g, const Side& side, int type, Army* h, Quest& q) {
  if (type == SLAY_HERO) {
    std::vector<Army*> heroes;
    for (Army* a : g.armies)
      if (a->type == armytype::HERO && a->owner != NONE && a->owner != side.index) heroes.push_back(a);
    q.army = g.rng.pick(heroes, (Army*)nullptr);
    return q.army != nullptr;
  } else if (type == RETRIEVE_ITEM) {
    std::vector<std::pair<Item*, Site*>> choices;
    for (auto& s : g.map->sites) {
      if (s.content == site::ITEM && !s.searched) {
        Item* item = nullptr;
        for (auto& it : g.map->items) if (it.index == s.item) item = &it;
        if (item && !site::itemReserved(*item) && std::abs(s.x - h->x) <= ITEM_RANGE &&
            std::abs(s.y - h->y) <= ITEM_RANGE) {
          choices.emplace_back(item, &s);
        }
      }
    }
    int k = g.rng.pickIndex(choices.size());
    if (k < 0) return false;
    choices[k].second->revealed = 0;               // the priests show where it lies
    q.item = choices[k].first;
    return true;
  } else if (type == SLAY_TYPE) {
    for (int t = 0; t < 5; t++) {                 // up to five tries
      std::vector<const ArmyType*> magical;
      for (auto* a : g.types.list()) if (a->bonus[48] != 0) magical.push_back(a);
      const ArmyType* want = g.rng.pick(magical, (const ArmyType*)nullptr);
      if (want) {
        for (Army* a : g.armies) {
          if (a->type == want->id && a->owner != NONE && a->owner != side.index) {
            q.armyType = want;
            return true;
          }
        }
      }
    }
    return false;
  } else if (type == SLAUGHTER) {
    q.side = g.rng.pick(otherSides(g, side), (Side*)nullptr);
    return q.side != nullptr;
  } else if (type == OCCUPY || type == RAZE) {
    std::vector<City*> choices, any;
    for (auto& c : g.map->cities) {
      if (c.ownerIndex != side.index && !c.razed) {
        any.push_back(&c);
        int d = std::max(std::abs(c.x - h->x), std::abs(c.y - h->y));
        if (d <= CITY_RANGE) choices.push_back(&c);
      }
    }
    City* c = g.rng.pick(choices, (City*)nullptr);
    if (!c) c = g.rng.pick(any, (City*)nullptr);
    q.city = c;
    return c != nullptr;
  } else if (type == PILLAGE_GOLD) {
    return true;                                  // no target, just a count
  }
  return false;
}

bool heroInStack(const Quest& q, const std::vector<Army*>& stack) {
  return std::find(stack.begin(), stack.end(), q.hero) != stack.end();
}

// The quest is over. A failure carries the STRING.DAT group quest_check tells
// a human it with; a human side keeps the outcome as `questNews`.
std::shared_ptr<QuestResult> finish(Game& g, Side& side, const std::string& reason, int why = 0) {
  auto q = side.quest;
  side.quest.reset();
  if (!q) return nullptr;
  auto out = std::make_shared<QuestResult>();
  out->quest = q;
  if (reason == "done") {
    history::deed(g, &side, history::QUEST_DONE, 0, 0, q->hero ? q->hero->name : "");   // 4976:1da8
    hero::addExperience(g, q->hero, EXPERIENCE);
    out->reward = reward(g, side, q.get());
  } else {
    out->failed = reason;
    out->why = why;
  }
  if (!side.computer) side.questNews = out;
  return out;
}
}  // namespace

std::shared_ptr<Quest> assign(Game& g, Side& side, Army* h) {
  if (side.quest) return nullptr;                    // one at a time
  if (g.map->options.quests == 0) return nullptr;
  for (int t = 0; t < 20; t++) {
    int type = TYPE_TABLE[g.rng.dice(1, 10, 0) - 1];
    // types 3, 4 and 5 are skipped once the game has been won
    if (!(g.won && (type == SLAUGHTER || type == OCCUPY || type == RAZE))) {
      Quest q;
      if (pickTarget(g, side, type, h, q)) {
        q.type = type;
        q.hero = h;
        q.done = 0;
        q.targetKind = TARGET_KIND[type];
        if (type == SLAUGHTER) q.required = g.rng.dice(1, 12, 10);
        else if (type == PILLAGE_GOLD) q.required = g.rng.dice(3, 300, 500);
        side.quest = std::make_shared<Quest>(q);
        history::deed(g, &side, history::QUEST_GIVEN, 0, 0, h ? h->name : "");   // 4976:0dae
        return side.quest;
      }
    }
  }
  return nullptr;
}

std::string describe(const Quest& q) {
  const char* text = DESCRIPTIONS[q.type];
  if (q.required != NONE) return fmt(text, q.required);
  if (q.type == OCCUPY || q.type == RAZE) return std::string(text) + ": " + (q.city ? q.city->name : "");
  if (q.type == SLAY_TYPE) return std::string(text) + ": " + (q.armyType ? q.armyType->name : "");
  return text;
}

std::shared_ptr<QuestResult> event(Game& g, Side& side, const std::string& ev, const Event& data) {
  auto qp = side.quest;
  if (!qp) return nullptr;
  Quest& q = *qp;

  if (ev == "turn") {
    bool alive = false;
    for (Army* a : g.armies) if (a == q.hero && a->owner == side.index) alive = true;
    if (!alive) return finish(g, side, "the hero is lost", 0x20);
    if (q.type == OCCUPY || q.type == RAZE) {
      if (q.city->razed) return finish(g, side, "the city is ruins", 0x21);
      if (q.city->ownerIndex == side.index) return finish(g, side, "another took the city", 0x2a);
    } else if (q.type == SLAUGHTER) {
      if (!q.side->alive) return finish(g, side, "that side is gone", 0x27);
    } else if (q.type == SLAY_HERO) {
      if (!g.alive(q.army)) return finish(g, side, "the quarry is gone", 0x29);
    } else if (q.type == RETRIEVE_ITEM) {
      if (q.item->status == 0) return finish(g, side, "the item is lost", 0x28);
    }
    return nullptr;
  }

  if (ev == "battle") {
    if (!heroInStack(q, data.stack)) return nullptr;
    if (q.type == SLAY_HERO) {
      for (Army* d : data.killed) if (d == q.army) return finish(g, side, "done");
    } else if (q.type == SLAY_TYPE) {
      for (Army* d : data.killed) if (d->type == q.armyType->id) return finish(g, side, "done");
    } else if (q.type == SLAUGHTER) {
      for (Army* d : data.killed) if (d->owner == q.side->index) q.done++;
      if (q.done >= q.required) return finish(g, side, "done");
    }
  } else if (ev == "item") {
    if (q.type == RETRIEVE_ITEM && q.hero) {
      auto& items = q.hero->items;
      auto i = std::find(items.begin(), items.end(), q.item);
      if (i != items.end()) {
        Item* it = *i;
        items.erase(i);                             // the priests take it away
        it->status = 0;
        return finish(g, side, "done");
      }
    }
  } else if (ev == "pillage") {
    if (q.type == PILLAGE_GOLD && heroInStack(q, data.stack)) {
      q.done += data.gold;
      if (q.done >= q.required) return finish(g, side, "done");
    } else if ((q.type == OCCUPY || q.type == RAZE) && data.city == q.city) {
      return finish(g, side, "that was not to pillage", 0x24);
    }
  } else if (ev == "occupy") {
    if (q.type == OCCUPY && data.city == q.city) {
      if (heroInStack(q, data.stack)) return finish(g, side, "done");
      return finish(g, side, "the hero was not there", 0x22);
    } else if (q.type == RAZE && data.city == q.city) {
      return finish(g, side, "the quest was to raze it", 0x23);
    }
  } else if (ev == "raze") {
    if (q.type == RAZE && data.city == q.city) {
      if (heroInStack(q, data.stack)) return finish(g, side, "done");
      return finish(g, side, "the hero was not there", 0x25);
    } else if (q.type == OCCUPY && data.city == q.city) {
      return finish(g, side, "the quest was to keep it", 0x26);
    }
  }
  return nullptr;
}

std::shared_ptr<Reward> reward(Game& g, Side& side, const Quest* q) {
  int cities = (int)game::sideCities(g, side).size();

  auto allies = [&](int n) {
    auto [type, count] = hero::allies(g);             // one random magical type
    (void)count;
    auto r = std::make_shared<Reward>();
    r->kind = "allies";
    r->type = type;
    Army* h = q ? q->hero : nullptr;
    City* home = g.map->city(h ? (h->homeCity == NONE ? 0 : h->homeCity) : 0);
    if (!home) home = side.capital;
    for (int i = 0; i < n; i++) {
      auto [x, y] = game::freeTileIn(g, *home, true);
      if (x != NONE) r->armies.push_back(g.add(hero::newAlly(type, x, y, side.index, home->index)));
    }
    return r;
  };

  auto gold = [&]() {
    int n = g.rng.dice(2, 1000, 1000);
    side.gold += n;
    auto r = std::make_shared<Reward>();
    r->kind = "gold";
    r->gold = n;
    return r;
  };

  if (cities < 10 && g.turn > 15) return allies(g.rng.dice(1, 3, 5));
  if (side.gold < 100) return gold();

  // an unclaimed magic item, one time in three
  std::vector<Item*> unclaimed;
  for (auto& it : g.map->items) if (it.status == 0) unclaimed.push_back(&it);
  if (!unclaimed.empty()) {
    if (g.rng.dice(1, 3, 0) == 1) {
      Item* it = g.rng.pick(unclaimed, (Item*)nullptr);
      Army* h = q ? q->hero : nullptr;
      if (h) {
        h->items.push_back(it);
        it->status = 3;
      }
      auto r = std::make_shared<Reward>();
      r->kind = "item";
      r->item = it;
      return r;
    }
  } else {
    // otherwise the priests may point at a rich site, two times in three
    std::vector<Site*> hidden;
    for (auto& s : g.map->sites) if (s.rich && !s.searched) hidden.push_back(&s);
    if (!hidden.empty() && g.rng.dice(1, 3, 0) <= 2) {
      Site* s = g.rng.pick(hidden, (Site*)nullptr);
      s->revealed = 0;
      auto r = std::make_shared<Reward>();
      r->kind = "revealed";
      r->site = s;
      return r;
    }
  }
  if (g.rng.dice(1, 2, 0) == 1) return allies(g.rng.dice(1, 3, 2));
  return gold();
}

}  // namespace w2::quest
