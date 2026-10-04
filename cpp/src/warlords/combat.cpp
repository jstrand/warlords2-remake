#include "warlords/combat.hpp"

#include <algorithm>

#include "util/util.hpp"
#include "warlords/armytype.hpp"
#include "warlords/game.hpp"
#include "warlords/move.hpp"
#include "warlords/rules.hpp"
#include "warlords/scn.hpp"

namespace w2::combat {

const char* const CLASS_NAMES[4] = {"city", "open", "woods", "hills"};
const char* const ADVICE[10] = {
    "complete and utter suicide!",
    "sheerest folly! Thou shouldst not attack!",
    "a foolish decision!",
    "a brave choice! I leave it to thee!",
    "difficult but not impossible to win!",
    "very evenly matched!",
    "a hard-fought victory! But we shall win!",
    "a comfortable victory!",
    "an easy victory! We cannot lose!",
    "as simple as butchering sleeping cattle!",
};

// ARMYTYPE offsets
static const int BONUS_SELF = 32, BONUS_STACK = 40;     // + 2*class
static const int SUBTRACT = 50, ABILITY = 52;

int terrainClass(const Game& g, int x, int y) {
  if (game::towerAt(g, x, y)) return CITY;
  int t = scn::terrainAt(*g.map, x, y);
  if (t == move::CITY || t == move::SITE) return CITY;
  if (t == move::FOREST) return WOODS;
  if (t == move::HILLS || t == move::MOUNTAINS) return HILLS;
  return OPEN;
}

namespace {
// Sort a line by its owner's fight order, lowest first, ties keeping stack
// order.
void sortLine(const Game& g, std::vector<Army*>& line, int ownerIndex) {
  const auto& row = g.map->fightOrder[ownerIndex == NONE ? 8 : ownerIndex];
  std::map<const Army*, int> order;
  for (size_t i = 0; i < line.size(); i++) order[line[i]] = (int)i;
  luaSort(line, [&](Army* p, Army* q) {
    int rp = row[p->type], rq = row[q->type];
    if (rp != rq) return rp < rq;
    return order[p] < order[q];
  });
}

// On water, shore and mountain tiles a side holding a hero sends one flier to
// the back, keeping the hero's carrier alive longest.
void protectHeroCarrier(const Game& g, std::vector<Army*>& line, int x, int y) {
  int t = scn::terrainAt(*g.map, x, y);
  if (t != move::WATER && t != move::SHORE && t != move::MOUNTAINS) return;
  if (std::none_of(line.begin(), line.end(), [](Army* a) { return a->type == armytype::HERO; })) return;
  for (size_t i = 0; i < line.size(); i++) {
    Army* a = line[i];
    if (g.types.byId(a->type)->flies) {
      line.erase(line.begin() + i);
      line.push_back(a);
      return;
    }
  }
}

int itemTotal(const Army& army, int itemType) {
  int total = 0;
  for (Item* it : army.items) if (it->type == itemType) total += std::max(1, it->value);
  return total;
}

bool lineHas(const Game& g, const std::vector<Army*>& line, int ability) {
  for (Army* a : line) if (g.types.byId(a->type)->bonus[ABILITY] == ability) return true;
  return false;
}

int stackBonus(const Game& g, const std::vector<Army*>& line, int cls) {
  int best = 0;
  for (Army* a : line) best = std::max(best, g.types.byId(a->type)->bonus[BONUS_STACK + 2 * cls]);
  return best;
}

int subtract(const Game& g, const std::vector<Army*>& line) {
  int least = 0;
  for (Army* a : line) least = std::min(least, g.types.byId(a->type)->bonus[SUBTRACT]);
  return least;
}
}  // namespace

Lines lines(const Game& g, const std::vector<Army*>& stack, int x, int y) {
  int attacker = stack.empty() ? NONE : stack[0]->owner;
  City* city = g.map->cityTile[y * g.map->width + x];
  std::vector<std::pair<int, int>> tiles = {{x, y}};
  if (city) {
    tiles.clear();
    for (int dx = 0; dx <= 1; dx++)
      for (int dy = 0; dy <= 1; dy++) tiles.emplace_back(city->x + dx, city->y + dy);
  }
  Lines out;
  bool any = false;
  for (auto& t : tiles) {
    for (Army* a : game::armiesAt(g, t.first, t.second)) {
      if (a->owner != attacker) {
        out.defenders.push_back(a);
        if (!any) { out.defOwner = a->owner; any = true; }
      }
    }
  }
  out.attackers = stack;
  sortLine(g, out.attackers, attacker);
  sortLine(g, out.defenders, out.defOwner);
  protectHeroCarrier(g, out.attackers, x, y);
  protectHeroCarrier(g, out.defenders, x, y);
  out.city = city;
  return out;
}

int battleItems(const Army& army) { return itemTotal(army, rules::ITEM_BATTLE); }

int commandItems(const Army& army) {
  int n = itemTotal(army, rules::ITEM_COMMAND);
  for (Item* it : army.items) if (it->type == rules::ITEM_STANDARD) n++;
  return n;
}

int heroBonus(const Game& g, const std::vector<Army*>& line) {
  int best = 0, command = 0;
  for (Army* a : line) {
    if (a->type == armytype::HERO) {
      int str = a->strength + battleItems(*a);
      best = std::max(best, std::min(9, str));
      command += commandItems(*a);
    }
  }
  return HERO_TABLE[std::max(0, best)] + command;
}

int fortify(const Game& g, const std::vector<Army*>& attackers, int x, int y, int cls) {
  if (cls != CITY) return 0;
  if (lineHas(g, attackers, SIEGE)) return 0;
  int value;
  City* c = g.map->cityTile[y * g.map->width + x];
  if (game::towerAt(g, x, y)) value = 1;
  else if (scn::terrainAt(*g.map, x, y) == move::SITE) value = 2;
  else value = c ? c->defence : 0;
  if (c && c->ownerIndex == NONE) value = value / 2;   // neutral
  return value;
}

std::pair<int, int> modifiers(const Game& g, const std::vector<Army*>& attackers,
                              const std::vector<Army*>& defenders, int x, int y, int cls) {
  int cap = g.map->combatCap;
  int atkHero = lineHas(g, defenders, NEGATE_HERO) ? 0 : heroBonus(g, attackers);
  int atkStack = lineHas(g, defenders, NEGATE_OTHER) ? 0 : stackBonus(g, attackers, cls);
  int defHero = lineHas(g, attackers, NEGATE_HERO) ? 0 : heroBonus(g, defenders);
  int defStack = lineHas(g, attackers, NEGATE_OTHER) ? 0 : stackBonus(g, defenders, cls);
  int attackMod = std::min(cap, atkHero + atkStack) + subtract(g, defenders);
  int defendMod = std::min(cap, defHero + defStack + fortify(g, attackers, x, y, cls)) + subtract(g, attackers);
  return {attackMod, defendMod};
}

int strength(const Game& g, const Army& army, int mod, int cls, int terrain) {
  if (army.atSea && (terrain == move::WATER || terrain == move::SHORE)) return 4;
  int s = army.strength + mod + battleItems(army) + g.types.byId(army.type)->bonus[BONUS_SELF + 2 * cls];
  return std::min(MAX_STRENGTH, s);
}

std::map<const Army*, int> stackStrengths(const Game& g, const std::vector<Army*>& armies, int x, int y) {
  int cls = terrainClass(g, x, y);
  int total = 0, best = 0;
  for (Army* a : armies) {
    if (!a->atSea) {
      if (a->type == armytype::HERO) {
        int s = std::min(9, a->strength + battleItems(*a));
        total += HERO_TABLE[std::max(0, s)] + commandItems(*a);
      } else {
        int v = g.types.byId(a->type)->bonus[BONUS_STACK + 2 * cls];
        if (v > best) { total = total + v - best; best = v; }
      }
    }
  }
  total = std::min(total, g.map->combatCap);
  std::map<const Army*, int> out;
  for (Army* a : armies) {
    int s;
    if (a->atSea) s = 4;
    else if (a->type == armytype::HERO) s = a->strength + battleItems(*a);
    else s = a->strength + g.types.byId(a->type)->bonus[BONUS_SELF + 2 * cls];
    out[a] = std::min(9, s) + total;
  }
  return out;
}

Battle resolve(Game& g, const std::vector<Army*>& attackers, const std::vector<Army*>& defenders, int x, int y) {
  int cls = terrainClass(g, x, y);
  int terrain = scn::terrainAt(*g.map, x, y);
  auto [attackMod, defendMod] = modifiers(g, attackers, defenders, x, y, cls);
  int die = g.map->options.intenseCombat != 0 ? DIE_INTENSE : DIE;

  struct E { Army* army; int strength; int hits; bool dead = false; };
  auto prepare = [&](const std::vector<Army*>& line, int mod) {
    std::vector<E> out;
    for (Army* a : line) out.push_back(E{a, std::max(1, strength(g, *a, mod, cls, terrain)), HIT_POINTS - 1});
    return out;
  };
  auto atk = prepare(attackers, attackMod), def = prepare(defenders, defendMod);
  Battle b;
  size_t ai = 0, di = 0;
  int throws = 0;
  while (ai < atk.size() && di < def.size()) {
    E& a = atk[ai];
    E& d = def[di];
    int ra = g.rng.dice(1, die, 0), rd = g.rng.dice(1, die, 0);
    bool aHits = ra <= a.strength && rd > d.strength;
    bool dHits = rd <= d.strength && ra > a.strength;
    throws++;
    if (throws > STALEMATE) { aHits = false; dHits = true; }

    // Tutorial: a human player's hero cannot die attacking a neutral defender.
    if (dHits && g.map->options.tutorial != 0 && a.army->type == armytype::HERO) {
      Side* s = g.map->side(a.army->owner == NONE ? 0 : a.army->owner);
      if (!(s && s->computer) && d.army->owner == NONE) dHits = false;
    }
    if (aHits) {
      d.hits--;
      if (d.hits < 0) { d.dead = true; b.log.push_back(0); di++; }
    } else if (dHits) {
      a.hits--;
      if (a.hits < 0) { a.dead = true; b.log.push_back(1); ai++; }
    }
  }
  for (auto& e : atk) (e.dead ? b.deadAttackers : b.attackers).push_back(e.army);
  for (auto& e : def) (e.dead ? b.deadDefenders : b.defenders).push_back(e.army);
  for (Army* a : b.deadAttackers) b.deadByArmy.insert(a);
  for (Army* a : b.deadDefenders) b.deadByArmy.insert(a);
  b.won = b.defenders.empty();
  b.attackMod = attackMod;
  b.defendMod = defendMod;
  b.cls = cls;
  return b;
}

std::pair<std::string, int> advise(Game& g, const std::vector<Army*>& attackers,
                                   const std::vector<Army*>& defenders, int x, int y) {
  int wins = 0;
  for (int i = 0; i < ADVICE_BATTLES; i++) {
    if (resolve(g, attackers, defenders, x, y).won) wins++;
  }
  return {ADVICE[wins / 2], wins};
}

}  // namespace w2::combat
