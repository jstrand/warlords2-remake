#include "warlords/hero.hpp"

#include <algorithm>

#include "util/util.hpp"
#include "warlords/ai.hpp"
#include "warlords/armytype.hpp"
#include "warlords/combat.hpp"
#include "warlords/game.hpp"
#include "warlords/history.hpp"
#include "warlords/move.hpp"
#include "warlords/rules.hpp"
#include "warlords/scn.hpp"

namespace w2::hero {

const char* levelName(int level) {
  switch (level) {
    case 1: return "Hero";
    case 2: return "Cavalier";
    case 3: return "Champion";
    case 4: return "Paladin";
  }
  return "";
}

int promotionAt(int level) {
  switch (level) {
    case 1: return 15;
    case 2: return 30;
    case 3: return 60;
  }
  return NONE;
}

const std::vector<std::pair<std::string, bool>>& names(Game& g, const Side& side) {
  auto it = g.heroNames.find(side.index);
  if (it != g.heroNames.end()) return it->second;
  std::vector<std::pair<std::string, bool>> list;
  auto text = readFile(g.dataDir + "/TERRAIN0/HERONAM" + std::to_string(side.index) + ".DAT");
  for (auto& line : splitLines(text ? *text : "")) {
    // ^#(\d)\s+(.+)$
    if (line.size() >= 3 && line[0] == '#' && line[1] >= '0' && line[1] <= '9' &&
        (line[2] == ' ' || line[2] == '\t')) {
      size_t i = 2;
      while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;
      if (i < line.size()) list.emplace_back(line.substr(i), line[1] == '1');
    }
  }
  return g.heroNames[side.index] = list;
}

std::pair<std::string, bool> rollName(Game& g, const Side& side) {
  const auto& list = names(g, side);
  int n = g.rng.dice(1, NAME_ROLL, 0);
  if (n - 1 < (int)list.size()) return list[n - 1];
  if (list.empty()) return {"Hero", false};
  return list.back();
}

static std::pair<int, int> countHeroes(const Game& g, const Side* side) {
  int all = 0, mine = 0;
  for (Army* a : g.armies) {
    if (a->type == armytype::HERO) {
      all++;
      if (side && a->owner == side->index) mine++;
    }
  }
  return {all, mine};
}

std::shared_ptr<HeroOffer> offer(Game& g, Side& side) {
  auto named = [&](HeroOffer o) {
    auto [n, f] = rollName(g, side);
    o.name = n;
    o.female = f;
    return std::make_shared<HeroOffer>(o);
  };
  // none at all once the surrender has been offered (7563:0000)
  if (g.surrenderOffered) return nullptr;
  if (g.turn == 1) return named(HeroOffer{0, side.capital, true, "", false});

  auto [all, mine] = countHeroes(g, &side);
  if (all >= MAX_IN_GAME) return nullptr;
  auto cities = game::sideCities(g, side);
  int limit = (int)cities.size() >= BIG_SIDE_CITIES ? MAX_PER_BIG_SIDE : MAX_PER_SIDE;
  if (mine >= limit) return nullptr;

  int price = mine == 0 ? g.rng.dice(1, 400, 300) : g.rng.dice(1, 600, 1000);
  if (price > side.gold) return nullptr;
  if (g.rng.dice(1, 30, 0) >= 7) return nullptr;          // a 20% chance

  City* city = g.rng.pick(cities, (City*)nullptr);
  if (!city) return nullptr;
  // a computer's hero appears where its AI wants one (ai_hero_city, 5db9:0919)
  if (side.computer) city = ai::heroCity(g, side, city);
  return named(HeroOffer{price, city, false, "", false});
}

const ArmyType* magicalType(Game& g) {
  std::vector<int> ids;
  for (int id = 0; id <= 27; id++) {
    const ArmyType* t = g.types.byId(id);
    if (t && t->bonus[48] != 0) ids.push_back(id);
  }
  int n = g.rng.dice(1, (int)ids.size(), -1);
  return g.types.byId(n >= 0 && n < (int)ids.size() ? ids[n] : 25);
}

std::pair<const ArmyType*, int> allies(Game& g) {
  const ArmyType* type = magicalType(g);
  int roll = g.rng.dice(1, 100, 0);
  int n = roll < 70 ? 1 : roll < 95 ? 2 : 3;
  return {type, n};
}

Army newAlly(const ArmyType* type, int x, int y, int owner) {
  Army a;
  a.x = x; a.y = y; a.owner = owner;
  a.type = type->id; a.name = type->name;
  a.strength = type->strength;
  a.maxMoves = type->move; a.moves = type->move;
  a.upkeep = 0;
  a.homeCity = 0;
  return a;
}

Army* placeAlly(Game& g, const ArmyType* type, int x, int y, const Side& side) {
  int px = x, py = y;
  for (int tries = 10; tries > 0; tries--) {
    if (px >= 0 && py >= 0 && px < g.map->width && py < g.map->height) {
      auto here = game::armiesAt(g, px, py);
      if (here.size() < 8 && (here.empty() || here[0]->owner == side.index)) {
        return g.add(newAlly(type, px, py, side.index));
      }
    }
    px += g.rng.dice(1, 3, -2);
    py += g.rng.dice(1, 3, -2);
  }
  return nullptr;
}

Recruited recruit(Game& g, Side& side, const HeroOffer& off) {
  City* city = off.city;
  side.gold = std::max(0, side.gold - off.price);
  std::pair<int, int> last{NONE, NONE};
  auto [hx, hy] = game::freeTileIn(g, *city, true, &last);
  if (hx == NONE) {
    hx = last.first != NONE ? last.first : city->x;
    hy = last.second != NONE ? last.second : city->y;
  }

  Army h;
  h.x = hx; h.y = hy; h.owner = side.index; h.type = armytype::HERO;
  h.name = off.name.empty() ? "Hero" : off.name;
  h.female = off.female;
  h.strength = START_STRENGTH;
  h.maxMoves = START_MOVES; h.moves = START_MOVES; h.upkeep = 0;
  h.homeCity = city->index; h.level = 1; h.experience = 0;
  if (off.first) {
    // Turn 1: the hero carries its side's standard, item number = side
    if (side.index < (int)g.map->items.size()) {
      Item* std = &g.map->items[side.index];
      std->status = 3; std->x = NONE; std->y = NONE;
      std->standardOf = side.index;
      h.items.push_back(std);
    }
  }
  Recruited out;
  out.hero = g.add(h);
  history::deed(g, &side, history::EMERGES, city->index, 0, out.hero->name);     // 7563:04e7

  if (!off.first) {
    auto [type, n] = allies(g);
    out.type = type; out.count = n;
    for (int i = 0; i < n; i++) {
      Army* a = placeAlly(g, type, hx, hy, side);
      if (a) out.allies.push_back(a);
    }
  }
  return out;
}

void addExperience(Game& g, Army* h, int n) {
  h->experience = std::min(rules::MAX_HERO_XP, h->experience + n);
}

std::vector<Promotion> checkPromotions(Game& g, const Side& side) {
  std::vector<Promotion> promoted;
  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    Army* a = g.armies[i];
    if (a->type == armytype::HERO && a->owner == side.index) {
      int level = a->level ? a->level : 1;
      int need = promotionAt(level);
      if (need != NONE && a->experience >= need) {
        int before = a->strength;
        a->level = level + 1;
        a->strength = std::min(MAX_STRENGTH, before + 1);
        a->maxMoves += PROMOTION_MOVES;
        a->title = levelName(a->level);
        promoted.push_back(Promotion{a, a->strength - before});
      }
    }
  }
  return promoted;
}

void battleExperience(Game& g, const std::vector<Army*>& attackers, const std::vector<Army*>& defenders,
                      const Battle& result, bool wasCity) {
  for (Army* a : attackers) {
    if (a->type == armytype::HERO && !result.deadByArmy.count(a)) addExperience(g, a, wasCity ? 2 : 1);
  }
  for (size_t i = 0; i < defenders.size(); i++) {
    Army* d = defenders[i];
    if (!result.deadByArmy.count(d)) {
      bool counts;
      if (rules::bugs.heroExperienceReadsAttackerTypes) {
        Army* sameSlot = i < attackers.size() ? attackers[i] : nullptr;   // the wrong line, on purpose
        counts = sameSlot && sameSlot->type == armytype::HERO;
      } else {
        counts = true;
      }
      if (counts && d->type == armytype::HERO) addExperience(g, d, 1);
    }
  }
}

std::vector<Item*> itemsHere(Game& g, const Army* h) {
  std::vector<Item*> carried = h->items;
  std::vector<Item*> ground;
  for (auto& it : g.map->items) if (it.status == 1 && it.x == h->x && it.y == h->y) ground.push_back(&it);
  auto byIndex = [](Item* a, Item* b) { return a->index < b->index; };
  luaSort(carried, byIndex);
  luaSort(ground, byIndex);
  carried.insert(carried.end(), ground.begin(), ground.end());
  return carried;
}

void dropItem(Game& g, Army* h, Item* it) {
  auto i = std::find(h->items.begin(), h->items.end(), it);
  if (i != h->items.end()) h->items.erase(i);
  if (scn::terrainAt(*g.map, h->x, h->y) == move::WATER) {
    it->status = 0; it->x = NONE; it->y = NONE;
  } else {
    it->status = 1; it->x = h->x; it->y = h->y; it->planted = false;
  }
}

void takeItem(Game& g, Army* h, Item* it) {
  it->status = 3; it->x = NONE; it->y = NONE; it->planted = false;
  h->items.push_back(it);
}

std::vector<Item*> dropItems(Game& g, Army* h, int x, int y, int cur) {
  std::vector<Item*> dropped;
  if (h->items.empty()) return dropped;
  if (cur == NONE && g.side) cur = g.side->index;
  bool drowned = scn::terrainAt(*g.map, x, y) == move::WATER;
  for (Item* it : h->items) {
    it->planted = false;
    if (drowned) {
      it->status = 0; it->x = NONE; it->y = NONE;
    } else {
      it->status = 1; it->x = x; it->y = y;
      dropped.push_back(it);
    }
    if (cur != NONE && it->index == cur) {
      for (auto& c : g.map->cities) {
        if (c.vectorTo == -2 && c.ownerIndex == cur) c.vectorTo = NONE;
      }
    }
  }
  h->items.clear();
  return dropped;
}

}  // namespace w2::hero
