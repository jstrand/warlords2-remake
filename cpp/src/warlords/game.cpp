#include "warlords/game.hpp"

#include <algorithm>
#include <cmath>
#include <set>

#include "util/util.hpp"
#include "warlords/ai.hpp"
#include "warlords/ai/core.hpp"
#include "warlords/armytype.hpp"
#include "warlords/combat.hpp"
#include "warlords/diplomacy.hpp"
#include "warlords/hero.hpp"
#include "warlords/history.hpp"
#include "warlords/move.hpp"
#include "warlords/quest.hpp"
#include "warlords/rules.hpp"
#include "warlords/scn.hpp"
#include "warlords/site.hpp"

namespace w2 {

bool Army::hero() const { return type == armytype::HERO; }

Game::Game() = default;
Game::~Game() = default;

Army* Game::make(const Army& a) {
  pool.push_back(std::make_unique<Army>(a));
  return pool.back().get();
}

bool Game::alive(const Army* a) const {
  return a && std::find(armies.begin(), armies.end(), a) != armies.end();
}

void Game::remove(const Army* a) {
  auto it = std::find(armies.begin(), armies.end(), a);
  if (it != armies.end()) armies.erase(it);
}

namespace game {

static const int TRANSIT_TURNS = 2;       // a vectored army arrives two turns later

static int key(const Game& g, int x, int y) { return y * g.map->width + x; }

std::vector<Army*> armiesAt(const Game& g, int x, int y) {
  std::vector<Army*> out;
  for (Army* a : g.armies) if (!a->transit && a->x == x && a->y == y) out.push_back(a);
  return out;
}

City* cityAt(const Game& g, int x, int y) {
  if (x < 0 || y < 0 || x >= g.map->width || y >= g.map->height) return nullptr;
  return g.map->cityTile[key(g, x, y)];
}

void setCityOwner(Game& g, City& city, int sideIndex) {
  city.ownerIndex = sideIndex;
  move::invalidate(g);
}

std::vector<City*> sideCities(const Game& g, const Side& side) {
  std::vector<City*> out;
  for (auto& c : g.map->cities) if (c.ownerIndex == side.index) out.push_back(&c);
  return out;
}

std::vector<Army*> sideArmies(const Game& g, const Side& side) {
  std::vector<Army*> out;
  for (Army* a : g.armies) if (a->owner == side.index) out.push_back(a);
  return out;
}

int itemBonus(const Game& g, const Side& side, int itemType) {
  int total = 0;
  for (Army* a : g.armies) {
    if (a->owner == side.index) {
      for (Item* it : a->items) if (it->type == itemType) total += std::max(1, it->value);
    }
  }
  return total;
}

std::pair<int, int> freeTileIn(Game& g, const City& city, bool spill) {
  for (int dx = 0; dx <= 1; dx++) {
    for (int dy = 0; dy <= 1; dy++) {
      int x = city.x + dx, y = city.y + dy;
      if (x < g.map->width && y < g.map->height && (int)armiesAt(g, x, y).size() < rules::MAX_STACK) {
        return {x, y};
      }
    }
  }
  if (!spill) return {NONE, NONE};
  for (int i = 0; i < 20; i++) {
    int x = city.x + g.rng.dice(1, 3, -2);
    int y = city.y + g.rng.dice(1, 3, -2);
    if (x >= 0 && y >= 0 && x < g.map->width && y < g.map->height &&
        (int)armiesAt(g, x, y).size() < rules::MAX_STACK) {
      return {x, y};
    }
  }
  return {NONE, NONE};
}

int income(const Game& g, const Side& side) {
  int total = 0;
  auto cities = sideCities(g, side);
  for (City* c : cities) total += c->income;
  int perCity = itemBonus(g, side, rules::ITEM_GOLD_PER_CITY);
  return total + (int)cities.size() * perCity;
}

int upkeep(const Game& g, const Side& side) {
  int total = 0;
  for (Army* a : g.armies) {
    if (a->owner == side.index && !a->transit) {
      int u = a->upkeep;
      if (a->atSea) u = std::max(rules::SEA_MIN_UPKEEP, u);
      total += u;
    }
  }
  return total;
}

// One starting army per city (docs/rules.md > Starting garrisons).
static void setupGarrisons(Game& g) {
  for (auto& c : g.map->cities) {
    bool owned = c.ownerIndex != NONE;
    auto level = rules::garrisonLevel(owned, g.map->options.neutralCities, g.rng);
    int slot = NONE;
    if (level && !c.slots.empty()) {
      Side* side = owned ? g.map->side(c.ownerIndex) : nullptr;
      slot = rules::bestSlot(c.slots, rules::GARRISON_PURPOSE[*level], g.types, side && side->enhanced);
    }
    if (slot != NONE) {
      Army a = rules::armyFromSlot(c.slots[slot], owned && g.map->side(c.ownerIndex)->enhanced);
      a.x = c.x; a.y = c.y; a.owner = c.ownerIndex; a.moves = 0; a.homeCity = c.index;
      g.add(a);
    } else {
      // Neutral Cities off: a placeholder Scouts army of strength 1.
      const ArmyType* t = g.types.byId(armytype::SCOUTS);
      Army a;
      a.x = c.x; a.y = c.y; a.owner = NONE; a.type = armytype::SCOUTS; a.name = t->name;
      a.strength = 1; a.maxMoves = t->move; a.moves = 0; a.upkeep = 0; a.homeCity = c.index;
      g.add(a);
    }
  }
}

// Quick Start: deal every neutral city out to the sides (setup_capitals,
// 79fa:07ca).
static void dealCities(Game& g) {
  std::map<int, std::pair<int, int>> from;
  int turn = NONE;
  for (int i = 7; i >= 0; i--) {
    Side* s = g.map->side(i);
    if (s && s->inUse) {
      from[i] = {s->capX, s->capY};
      turn = i;
    }
  }
  if (turn == NONE) return;
  for (;;) {
    auto [x, y] = from[turn];
    City* pick = nullptr;
    int best = 1000;
    for (auto& c : g.map->cities) {
      if (c.ownerIndex == NONE) {
        double dx = x - c.x, dy = y - c.y;
        int d = (int)std::floor(std::sqrt(dx * dx + dy * dy));
        if (d < best) { pick = &c; best = d; }
      }
    }
    if (!pick) return;
    Side* side = g.map->side(turn);
    pick->owner = side;
    pick->ownerIndex = turn;
    if (g.rng.dice(1, 10, -1) < 5) from[turn] = {side->capX, side->capY};
    else from[turn] = {pick->x, pick->y};
    do { turn = (turn + 1) % 8; } while (!from.count(turn));
  }
}

std::unique_ptr<Game> newGame(const std::string& dataDir, const std::string& scenario, const NewGameOptions& opts) {
  auto gp = std::make_unique<Game>();
  Game& g = *gp;
  g.rng = Rng(opts.seed);
  g.dataDir = dataDir;
  armytype::load(dataDir + "/TERRAIN0/ARMYTYPE.DAT", g.types);
  g.map = scn::load(dataDir + "/" + scenario, scenario);
  g.greatest = opts.greatest;
  for (auto& [k, v] : opts.options) {
    if (int* f = g.map->options.field(k)) *f = v;
  }
  for (auto& s : g.map->sides) {
    auto it = opts.sides.find(s.index);
    if (it != opts.sides.end() && s.inUse) {
      const SideSetup& o = it->second;
      if (o.off) {
        if (s.capital) s.capital->owner = nullptr;
        s.inUse = false;
      } else {
        s.computer = o.computer;
        s.level = o.level != NONE ? o.level : s.level;
        s.card = o.card;
      }
    }
  }

  for (auto& s : g.map->sides) {
    s.alive = s.inUse;
    s.ownerIndex = s.index;
    if (s.inUse) g.sides.push_back(&s);
  }
  // 79fa:0000: every side is observed unless the map is hidden and a human plays
  int humans = 0;
  for (Side* s : g.sides) if (!s->computer) humans++;
  bool observed = !(g.map->options.hiddenMap != 0 && humans >= 1);
  for (Side* s : g.sides) s->observe = observed;

  for (auto& c : g.map->cities) c.ownerIndex = c.owner ? c.owner->index : NONE;
  if (g.map->options.quickStart != 0) dealCities(g);

  for (auto& c : g.map->cities) {
    c.slots = rules::citySlots(c.produces, g.types, g.rng);
    c.defence = rules::cityDefence((int)c.slots.size());
    c.producing = NONE;       // slot index being built (0-based)
    c.countdown = 0;
    c.vectorTo = NONE;
  }
  scn::refreshCityTiles(*g.map);

  // items 0-7 are the eight sides' standards (docs/rules.md > Heroes)
  for (size_t i = 0; i < g.map->items.size(); i++) {
    Item& it = g.map->items[i];
    if (i < 8 && it.type == rules::ITEM_STANDARD) it.standardOf = (int)i;
  }

  diplomacy::init(g);
  setupGarrisons(g);
  site::setup(g);
  for (Side* s : g.sides) revealStart(g, *s);
  ai::startGame(g);

  g.current = 0;            // index into g.sides
  g.side = g.sides.empty() ? nullptr : g.sides[0];
  return gp;
}

static void eliminate(Game& g, Side& side) {
  side.alive = false;
  history::deed(g, &side, history::VANQUISHED, side.index, 0, "");     // 8065:19f1
  g.log.push_back(side.name + " has been eliminated.");
}

static void applyIncome(Game& g, Side& side) {
  side.income = income(g, side);
  side.upkeepTotal = upkeep(g, side);
  side.gold = std::max(0, side.gold + side.income - side.upkeepTotal);
}

// Place a produced army, or send it back; false if it was disbanded.
static bool deliver(Game& g, Army* army, City& city) {
  auto [x, y] = freeTileIn(g, city, true);
  if (x == NONE || city.ownerIndex != army->owner) {
    if (army->returning) return false;      // already heading home: disbanded
    City* home = g.map->city(army->homeCity);
    army->returning = true;
    army->transit = Transit{TRANSIT_TURNS, home ? home->index : NONE};
    return true;
  }
  army->x = x; army->y = y;
  army->transit.reset();
  army->returning = false;
  army->moves = 0;
  return true;
}

// Step 5: run every producing city the side owns, logged in side.produced.
static void runProduction(Game& g, Side& side) {
  std::vector<Produced> arrived, built;
  for (City* c : sideCities(g, side)) {
    if (c->producing != NONE) {
      c->countdown--;
      if (c->countdown <= 0) {
        bool vectored = c->vectorTo != NONE && c->vectorTo != c->index;
        auto [x, y] = freeTileIn(g, *c, true);
        if (side.gold <= 0) {
          c->countdown = 0;               // no gold: it waits, built but unpaid
        } else if (!(x != NONE || vectored)) {
          c->countdown = 0;               // nowhere to stand: it waits too
        } else {
          const Slot& slot = c->slots[c->producing];
          Army a = rules::armyFromSlot(slot, side.enhanced);
          a.owner = side.index; a.homeCity = c->index; a.moves = 0;
          a.x = x; a.y = y;
          if (vectored) {
            a.transit = Transit{TRANSIT_TURNS, c->vectorTo};
            a.x = NONE; a.y = NONE;
          }
          g.add(a);
          built.push_back(Produced{vectored ? "sent" : "built", a.type, c->index, false});
          c->countdown = slot.time;       // the city starts the next one
        }
      }
    }
  }

  for (Army* a : g.armies) {
    if (a->transit && a->owner == side.index) {
      a->transit->turns--;
      if (a->transit->turns <= 0) {
        if (a->transit->dest == STANDARD) {
          auto [sx, sy] = standardAt(g, side);
          a->transit.reset();
          if (sx != NONE && (int)armiesAt(g, sx, sy).size() < rules::MAX_STACK) {
            a->x = sx; a->y = sy; a->moves = 0; a->returning = false;
            arrived.push_back(Produced{"arrived", a->type, NONE, true});
          } else {
            a->returning = true;
            a->transit = Transit{TRANSIT_TURNS, a->homeCity};
          }
        } else {
          City* dest = g.map->city(a->transit->dest);
          a->transit.reset();
          if (!dest || !deliver(g, a, *dest)) a->disbanded = true;
          else if (!a->transit) arrived.push_back(Produced{"arrived", a->type, dest->index, false});
        }
      }
    }
  }

  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    if (g.armies[i]->disbanded) g.armies.erase(g.armies.begin() + i);
  }
  side.produced = arrived;
  for (auto& e : built) side.produced.push_back(e);
}

// Step 6: movement reset.
static void resetMovement(Game& g, Side& side) {
  bool doubleMove = itemBonus(g, side, rules::ITEM_DOUBLE_MOVE) > 0;
  for (Army* a : g.armies) {
    // 8c07:0113 wipes both of the cycle's per-turn marks for the side's armies
    if (a->owner == side.index) { a->done = false; a->offered = false; }
    if (a->owner == side.index && !a->transit) {
      int carry = std::min(a->moves, rules::MOVE_CARRY);
      int base = a->atSea ? rules::SEA_MOVES : a->maxMoves;
      if (doubleMove && heroWithDoubleMoveAt(g, side, a->x, a->y)) base += a->maxMoves;
      a->moves = std::min(rules::MAX_MOVE, base + carry);
    }
  }
}

// A stack given the Defend order encamps as its side's next turn opens
// (8c07:0000): its tile gets the tower flag when it still has its full
// movement and stands on plain, forest, hills, a bridge, marsh, the tower
// terrain or a road.
static void encamp(Game& g, Side& side) {
  static const std::set<int> ENCAMP_ON = {move::PLAIN, move::FOREST, move::HILLS, move::BRIDGE, move::MARSH, move::TOWER};
  for (Army* a : g.armies) {
    if (a->owner == side.index && a->fortified && !a->transit && a->x != NONE && a->moves >= a->maxMoves) {
      bool road = scn::roadAt(*g.map, a->x, a->y) % 32 != 0;
      if (road || ENCAMP_ON.count(scn::terrainAt(*g.map, a->x, a->y))) {
        g.towers.insert(a->y * g.map->width + a->x);
      }
    }
  }
}

bool towerAt(const Game& g, int x, int y) { return g.towers.count(y * g.map->width + x) > 0; }

void tidyTowers(Game& g) {
  if (g.towers.empty()) return;
  std::set<int> held;
  for (Army* a : g.armies) if (a->x != NONE && !a->transit) held.insert(a->y * g.map->width + a->x);
  for (auto it = g.towers.begin(); it != g.towers.end();) {
    if (!held.count(*it)) it = g.towers.erase(it);
    else ++it;
  }
}

bool heroWithDoubleMoveAt(const Game& g, const Side& side, int x, int y) {
  for (Army* a : g.armies) {
    if (a->owner == side.index && !a->transit && a->x == x && a->y == y && a->type == armytype::HERO) {
      for (Item* it : a->items) if (it->type == rules::ITEM_DOUBLE_MOVE) return true;
    }
  }
  return false;
}

bool startTurn(Game& g, Side& side) {
  if (sideCities(g, side).empty()) {
    eliminate(g, side);
    return false;
  }
  tidyExplored(g, side.index);
  side.diploNews = diplomacy::apply(g, side);
  for (auto& m : side.diploNews) g.log.push_back(m);
  side.heroOffer = hero::offer(g, side);
  hero::checkPromotions(g, side);
  auto questResult = quest::event(g, side, "turn");
  if (questResult && !questResult->failed.empty()) g.log.push_back("Quest abandoned: " + questResult->failed + ".");
  applyIncome(g, side);
  runProduction(g, side);
  resetMovement(g, side);
  tidyTowers(g);
  encamp(g, side);
  return true;
}

Side* endTurn(Game& g) {
  Side* leaving = g.side;
  if (leaving && leaving->alive && (leaving->computer || !g.won)) diplomacy::scoreUpdate(g, *leaving);
  Ending ending = checkEnd(g);
  if (!ending.message.empty()) g.log.push_back(ending.message);
  g.ending = ending;
  if (ending.over) {
    g.side = nullptr;
    return nullptr;
  }
  for (size_t n = 0; n < g.sides.size(); n++) {
    g.current++;
    if (g.current >= (int)g.sides.size()) {
      g.current = 0;
      g.turn++;
      history::record(g);     // 8065:17f6 -> 6d51:0d60
    }
    Side* side = g.sides[g.current];
    if (side->alive) {
      g.side = side;
      if (startTurn(g, *side)) return side;
    }
  }
  g.side = nullptr;
  return nullptr;
}

Side* begin(Game& g) {
  g.current = 0;
  g.side = g.sides[0];
  if (!startTurn(g, *g.side)) return endTurn(g);
  return g.side;
}

static void removeArmies(Game& g, const std::vector<Army*>& dead) {
  std::set<const Army*> gone(dead.begin(), dead.end());
  for (int i = (int)g.armies.size() - 1; i >= 0; i--) {
    if (gone.count(g.armies[i])) g.armies.erase(g.armies.begin() + i);
  }
}

Sign* signAt(Game& g, int x, int y) {
  for (auto& s : g.map->signs) if (s.x == x && s.y == y) return &s;
  return nullptr;
}

void disband(Game& g, Side& side, const std::vector<Army*>& armies) {
  for (Army* a : armies) {
    if (a->type == armytype::HERO) {
      hero::dropItems(g, a, a->x, a->y);
      if (side.quest && side.quest->hero == a) side.quest.reset();
    }
  }
  removeArmies(g, armies);
}

int loot(const Game& g, const Side* loser) {
  if (!loser) return 0;
  int n = (int)sideCities(g, *loser).size();
  int share = n > 1 ? loser->gold / n : loser->gold;
  return share / 2;
}

Battle decideAttack(Game& g, const std::vector<Army*>& stack, int x, int y) {
  combat::Lines l = combat::lines(g, stack, x, y);
  Battle result = combat::resolve(g, l.attackers, l.defenders, x, y);
  result.lines.attackers = l.attackers;
  result.lines.defenders = l.defenders;
  result.lines.city = l.city;
  Side* loser = l.defOwner != NONE ? g.map->side(l.defOwner) : nullptr;
  if (result.won && l.city && loser) result.loot = loot(g, loser);
  result.fought = Battle::Fought{stack, x, y, l.defOwner};
  return result;
}

Battle resolveAttack(Game& g, const std::vector<Army*>& stack, int x, int y) {
  Battle b = decideAttack(g, stack, x, y);
  applyAttack(g, b);
  return b;
}

Battle& applyAttack(Game& g, Battle& result) {
  if (!result.fought) return result;
  Battle::Fought f = *result.fought;
  result.fought.reset();
  const auto& stack = f.stack;
  int x = f.x, y = f.y, defOwner = f.defOwner;
  const auto& attackers = result.lines.attackers;
  const auto& defenders = result.lines.defenders;
  City* city = result.lines.city;

  auto fallen = [&](Army* h) {
    history::deed(g, g.map->side(h->owner == NONE ? 8 : h->owner), history::KILLED,
                  city ? city->index : history::IN_BATTLE, 0, h->name);
  };
  int mine = stack.empty() ? NONE : stack[0]->owner;
  {
    int heroesLost = 0;
    for (Army* dd : result.deadDefenders) if (dd->type == armytype::HERO) heroesLost++;
    ai::recordBattle(g, defOwner, mine, x, y, heroesLost, (int)result.deadDefenders.size(),
                     result.won && !defenders.empty(), city != nullptr);
  }
  for (Army* a : result.deadAttackers) {
    history::tally(g, a, defOwner);
    if (a->type == armytype::HERO) { hero::dropItems(g, a, a->x, a->y); fallen(a); }
  }
  for (Army* d : result.deadDefenders) {
    history::tally(g, d, mine);
    if (d->type == armytype::HERO) { hero::dropItems(g, d, x, y); fallen(d); }
  }
  hero::battleExperience(g, attackers, defenders, result, city != nullptr);
  Side* mySide = g.map->side(stack.empty() ? 0 : (stack[0]->owner == NONE ? 0 : stack[0]->owner));
  {
    quest::Event ev;
    ev.stack = attackers;
    ev.killed = result.deadDefenders;
    result.quest = quest::event(g, *mySide, "battle", ev);
  }

  removeArmies(g, result.deadAttackers);
  removeArmies(g, result.deadDefenders);
  tidyTowers(g);

  if (result.won && city) {
    Side* winner = g.map->side(stack[0]->owner);
    Side* loser = defOwner != NONE ? g.map->side(defOwner) : nullptr;
    if (loser) {
      int l = result.loot;
      winner->gold += l;
      loser->gold = std::max(0, loser->gold - 2 * l);
    }
    city->producing = NONE; city->countdown = 0; city->vectorTo = NONE;
    city->ownerIndex = winner->index;
    scn::setCityTiles(*g.map, *city);
    move::invalidate(g);
    result.captured = city;
    history::deed(g, winner, history::WON, winner->index, city->index, winner->name);   // 67cc:0af5
    // a computer's quest hero taking its quest city (5e97:038d) razes it
    bool handled = winner->computer && ai::questCapture(g, *winner, *city, result.attackers);
    if (!handled && winner->computer) {
      quest::Event ev;
      ev.city = city;
      ev.stack = result.attackers;
      auto q = quest::event(g, *winner, "occupy", ev);
      if (q) result.quest = q;
    }
  }

  // The survivors walk into the tile they just cleared -- as many as fit.
  if (result.won) {
    g.towers.erase(y * g.map->width + x);
    int room = rules::MAX_STACK - (int)armiesAt(g, x, y).size();
    for (Army* a : result.attackers) {
      if (room <= 0) break;
      a->x = x; a->y = y;
      room--;
    }
  }
  return result;
}

// A production type's "value" is half its purchase price (ARMYTYPE +30).
static int slotValue(const Game& g, const Slot& slot) { return std::abs(g.types.byId(slot.type)->price) / 2; }

static void recompute(Game& g, City& city) {
  city.defence = rules::cityDefence((int)city.slots.size());
  if (city.producing != NONE && city.producing >= (int)city.slots.size()) {
    city.producing = NONE;
    city.countdown = 0;
  }
}

void addDiploScore(Game& g, Side& side, int n) { side.diploScore += n; }

std::shared_ptr<QuestResult> occupy(Game& g, Side& side, City& city, const std::vector<Army*>& stack) {
  quest::Event ev;
  ev.city = &city;
  ev.stack = stack;
  return quest::event(g, side, "occupy", ev);
}

Spoils pillage(Game& g, Side& side, City& city, const std::vector<Army*>& stack) {
  Spoils out;
  if (city.slots.empty()) return out;
  Slot slot = city.slots.back();          // slots are sorted cheapest first
  city.slots.pop_back();
  int gold = slotValue(g, slot);
  out.gold = gold;
  out.lost.emplace_back(slot.type, gold);
  side.gold += gold;
  addDiploScore(g, side, g.rng.dice(1, 5, 0));
  recompute(g, city);
  quest::Event ev;
  ev.city = &city;
  ev.gold = gold;
  ev.stack = stack;
  quest::event(g, side, "pillage", ev);
  return out;
}

Spoils sack(Game& g, Side& side, City& city, const std::vector<Army*>& stack) {
  Spoils out;
  if (city.slots.size() < 2) return out;
  int gold = 0;
  for (size_t i = 1; i < city.slots.size(); i++) {
    int v = slotValue(g, city.slots[i]);
    out.lost.emplace_back(city.slots[i].type, v);
    gold += v;
  }
  city.slots.resize(1);
  out.gold = gold;
  side.gold += gold;
  addDiploScore(g, side, g.rng.dice(1, 10, 5));
  recompute(g, city);
  quest::Event ev;
  ev.city = &city;
  ev.gold = gold;
  ev.stack = stack;
  quest::event(g, side, "pillage", ev);
  return out;
}

// The city becomes ruins (649c:016b).
static void makeRuins(Game& g, City& city) {
  city.razed = true;
  city.razedBy = city.ownerIndex == NONE ? 0 : city.ownerIndex;
  city.ownerIndex = NONE;
  city.claim = rules::NEUTRAL;
  city.slots.clear();
  city.producing = NONE; city.countdown = 0; city.vectorTo = NONE;
  city.defence = 0;
  city.income = 0;
  for (auto& c : g.map->cities) if (c.vectorTo == city.index) c.vectorTo = NONE;
  scn::setCityTiles(*g.map, city);
  move::invalidate(g);
}

void raze(Game& g, Side& side, City& city, const std::vector<Army*>& stack, bool ownCity) {
  makeRuins(g, city);
  if (ownCity) {
    addDiploScore(g, side, g.rng.dice(1, 25, 25));
    return;
  }
  addDiploScore(g, side, g.rng.dice(1, 15, 10));
  quest::Event ev;
  ev.city = &city;
  ev.stack = stack;
  quest::event(g, side, "raze", ev);
}

void resign(Game& g, Side& side) {
  for (auto& c : g.map->cities) {
    if (!c.razed && c.ownerIndex == side.index) makeRuins(g, c);
  }
  disband(g, side, sideArmies(g, side));
  if (g.map->options.hiddenMap != 0) {
    g.explored[side.index].assign(g.map->width * g.map->height, 1);
  }
}

// Tiles a side has seen: a mask per side.

static std::vector<uint8_t>* maskFor(Game& g, int side, bool make) {
  auto it = g.explored.find(side);
  if (it != g.explored.end()) return &it->second;
  if (!make) return nullptr;
  auto& m = g.explored[side];
  m.assign(g.map->width * g.map->height, 0);
  return &m;
}

bool seen(const Game& g, int side, int x, int y) {
  if (g.map->options.hiddenMap == 0) return true;
  auto it = g.explored.find(side);
  if (it == g.explored.end()) return false;
  return it->second[y * g.map->width + x] == 1;
}

int reveal(Game& g, int side, int x, int y, bool flying) {
  if (g.map->options.hiddenMap == 0) return 0;
  auto& mask = *maskFor(g, side, true);
  bool onCity = x >= 0 && y >= 0 && x < g.map->width && y < g.map->height && g.map->cityTile[y * g.map->width + x];
  int r = (flying || onCity) ? 2 : 1;
  int found = 0;
  for (int ty = y - r; ty <= y + r; ty++) {
    for (int tx = x - r; tx <= x + r; tx++) {
      if (tx >= 0 && ty >= 0 && tx < g.map->width && ty < g.map->height) {
        int k = ty * g.map->width + tx;
        if (!mask[k]) { mask[k] = 1; found++; }
      }
    }
  }
  return found;
}

const int FOG_DX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int FOG_DY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int FOG_CELL[256] = {
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
    4, 4, 4, 4, 4, 4, 4, 13, 4, 4, 4, 4, 8, 8, 8, 9,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 255, 255, 255, 10,
    255, 255, 255, 255, 255, 255, 255, 10, 255, 255, 255, 255, 3, 3, 3, 12,
    255, 11, 255, 11, 255, 11, 255, 1, 255, 11, 255, 11, 255, 11, 255, 1,
    255, 11, 255, 11, 255, 11, 255, 1, 255, 11, 255, 11, 3, 6, 3, 2,
    255, 11, 255, 11, 255, 11, 255, 1, 255, 11, 255, 11, 255, 11, 255, 1,
    4, 5, 4, 5, 4, 5, 4, 0, 4, 5, 4, 5, 8, 7, 8, 14,
};

int fogCell(const Game& g, int side, int x, int y) {
  if (seen(g, side, x, y)) return NONE;
  int bits = 0;
  for (int i = 0; i < 8; i++) {
    int nx = x + FOG_DX[i], ny = y + FOG_DY[i];
    if (nx < 0 || ny < 0 || nx >= g.map->width || ny >= g.map->height || !seen(g, side, nx, ny)) bits += 1 << i;
  }
  int cell = FOG_CELL[bits];
  return cell == FOG_NONE ? NONE : cell;
}

void tidyExplored(Game& g, int side) {
  if (g.map->options.hiddenMap == 0) return;
  auto& mask = *maskFor(g, side, true);
  int W = g.map->width, H = g.map->height;
  for (int pass = 0; pass < 2; pass++) {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        int k = y * W + x;
        if (!mask[k]) {
          int bits = 0;
          for (int i = 0; i < 8; i++) {
            int nx = x + FOG_DX[i], ny = y + FOG_DY[i];
            if (nx < 0 || ny < 0 || nx >= W || ny >= H || !mask[ny * W + nx]) bits += 1 << i;
          }
          if (FOG_CELL[bits] == FOG_NONE) mask[k] = 1;
        }
      }
    }
  }
}

void revealStart(Game& g, const Side& side) {
  if (g.map->options.hiddenMap == 0) return;
  for (City* c : sideCities(g, side)) reveal(g, side.index, c->x, c->y, false);
  for (Army* a : sideArmies(g, side)) if (!a->transit) reveal(g, side.index, a->x, a->y, false);
  tidyExplored(g, side.index);
}

void acceptSurrender(Game& g, Side& side) {
  for (Side* s : g.sides) if (s->computer) s->alive = false;
  g.won = true;
  history::deed(g, &side, history::VICTORIOUS, side.index, 0, "");
}

Ending checkEnd(Game& g) {
  std::vector<Side*> humans, computers, alive;
  for (Side* s : g.sides) {
    if (s->alive && !sideCities(g, *s).empty()) {
      alive.push_back(s);
      (s->computer ? computers : humans).push_back(s);
    }
  }
  int standing = 0;
  for (auto& c : g.map->cities) if (!c.razed) standing++;

  Ending e;
  if (alive.empty()) {
    g.over = true;
    e.over = true;
    e.message = "Alas! No more players are left!";
    return e;
  }
  if (humans.empty() && computers.size() > 1 && !g.noHumansSaid) {
    g.noHumansSaid = true;
    e.noHumans = true;
    return e;
  }
  if (humans.empty() && computers.size() == 1) {
    g.won = true;
    g.over = true;
    computers[0]->computer = false;          // so the finished game can be looked at
    history::deed(g, computers[0], history::VICTORIOUS, computers[0]->index, 0, "");
    e.over = true;
    e.winner = computers[0];
    e.message = computers[0]->name + " has triumphed!";
    return e;
  }
  if (humans.size() == 1 && computers.empty() && !g.won) {
    int mine = (int)sideCities(g, *humans[0]).size();
    if (mine * 2 > standing) {
      g.won = true;
      history::deed(g, humans[0], history::VICTORIOUS, humans[0]->index, 0, "");
      e.won = true;
      e.winner = humans[0];
      e.message = humans[0]->name + " rules the world!";
      return e;
    }
  }
  if (humans.size() == 1 && !computers.empty()) {
    int mine = (int)sideCities(g, *humans[0]).size();
    int biggest = 0;
    for (Side* s : computers) biggest = std::max(biggest, (int)sideCities(g, *s).size());
    if (mine * 2 > standing && mine > biggest + standing / 8) {
      g.surrenderOffered = true;
      e.surrender = true;
      e.message = "Your enemies offer their surrender!";
      return e;
    }
  }
  return e;
}

std::shared_ptr<SearchResult> searchHere(Game& g, const std::vector<Army*>& stack, bool human) {
  if (stack.empty()) return nullptr;
  return site::search(g, stack, stack[0]->x, stack[0]->y, human);
}

std::string describeSearch(const SearchResult* r) {
  if (!r) return "";
  const std::string& name = r->site->name;
  if (r->kind == "temple") {
    return r->blessed > 0 ? name + " blesses " + std::to_string(r->blessed) + " of your armies."
                          : name + " has blessed them already.";
  }
  if (r->kind == "no hero") return name + " can only be searched by a hero.";
  if (r->kind == "killed") return "Your hero is slain in " + name + " by a " + (r->monster ? r->monster->name : "guardian") + "!";
  if (r->kind == "gold") return name + " yields " + std::to_string(r->gold) + " gold!";
  if (r->kind == "item") return "Your hero finds the " + (r->item ? r->item->name : std::string("treasure")) + " in " + name + "!";
  if (r->kind == "allies") return std::to_string(r->armies.size()) + " " + r->type->name + " join you at " + name + "!";
  if (r->kind == "sage") return "A sage dwells in " + name + ".";
  return name + " holds nothing.";
}

void setProduction(Game& g, City& city, int slotIndex) {
  city.producing = slotIndex;
  city.countdown = slotIndex != NONE ? city.slots[slotIndex].time : 0;
  if (slotIndex == NONE) city.vectorTo = NONE;   // vectoring only sticks while building
}

std::vector<City*> vectoredTo(Game& g, const City* dest, const Side* side) {
  std::vector<City*> out;
  for (auto& c : g.map->cities) {
    if (!dest) {
      if (c.vectorTo == STANDARD && side && c.ownerIndex == side->index) out.push_back(&c);
    } else if (c.vectorTo == dest->index && &c != dest) {
      out.push_back(&c);
    }
  }
  return out;
}

std::pair<int, int> standardAt(const Game& g, const Side& side) {
  if (side.index < (int)g.map->items.size()) {
    const Item& it = g.map->items[side.index];
    if (it.status == 1 && it.planted && it.x != NONE) return {it.x, it.y};
  }
  return {NONE, NONE};
}

bool plantFlag(Game& g, Side& side, Army* h) {
  int t = scn::terrainAt(*g.map, h->x, h->y);
  if (t == move::WATER || t == move::SHORE || t == move::CITY || t == move::SITE) return false;
  for (auto& it : g.map->items) if (it.planted && it.x == h->x && it.y == h->y) return false;
  if (side.index >= (int)g.map->items.size()) return false;
  Item* std = &g.map->items[side.index];
  auto i = std::find(h->items.begin(), h->items.end(), std);
  if (i != h->items.end()) {
    h->items.erase(i);
    std->status = 1; std->x = h->x; std->y = h->y; std->planted = true;
    return true;
  }
  return false;
}

bool vectorToStandard(Game& g, City& city, const Side& side) {
  if (standardAt(g, side).first == NONE) return false;
  if (city.vectorTo != STANDARD && (int)vectoredTo(g, nullptr, &side).size() >= MAX_VECTORED_TO) return false;
  city.vectorTo = STANDARD;
  return true;
}

bool vector(Game& g, City& city, City* destCity) {
  if (destCity == &city) destCity = nullptr;
  if (destCity && city.vectorTo != destCity->index && (int)vectoredTo(g, destCity).size() >= MAX_VECTORED_TO) {
    return false;
  }
  city.vectorTo = destCity ? destCity->index : NONE;
  return true;
}

City* nearestCity(Game& g, int x, int y, const Side* side, const Side* seer) {
  City* best = nullptr;
  int bestD = NONE;
  for (auto& c : g.map->cities) {
    bool ok = !side || c.ownerIndex == side->index;
    if (ok && seer) {
      int s = seer->index;
      ok = seen(g, s, c.x, c.y) || seen(g, s, c.x + 1, c.y) || seen(g, s, c.x, c.y + 1) || seen(g, s, c.x + 1, c.y + 1);
    }
    if (ok) {
      int d = move::distance(x, y, c.x, c.y);
      if (bestD == NONE || d < bestD) { best = &c; bestD = d; }
    }
  }
  return best;
}

std::vector<const ArmyType*> buyableTypes(const Game& g) {
  std::vector<const ArmyType*> out;
  for (auto* a : g.types.list()) if (a->price >= 0) out.push_back(a);
  return out;
}

std::string cannotBuy(const Game& g, const Side& side, const City& city, const ArmyType& a) {
  for (auto& slot : city.slots) if (slot.type == a.id) return "has it";
  if (side.gold < a.price) return "too dear";
  return "";
}

int buySlot(const City& city) {
  if ((int)city.slots.size() < rules::PRODUCTION_SLOTS) return (int)city.slots.size();
  return 0;
}

void buyProduction(Game& g, Side& side, City& city, int n, int typeId) {
  const ArmyType* a = g.types.byId(typeId);
  bool building = city.producing != NONE;
  int was = city.producing;
  Slot s{a->id, a->name, a->strength, a->time, a->cost, a->move, std::abs(a->price)};
  if (n >= (int)city.slots.size()) city.slots.resize(n + 1);
  city.slots[n] = s;
  city.defence = rules::cityDefence((int)city.slots.size());
  side.gold -= a->price;
  // the slot being built was bought over
  if (building && was == n) {
    city.producing = NONE; city.countdown = 0; city.vectorTo = NONE;
  }
}

void sortProduction(City& city) {
  int building = city.producing;
  auto& s = city.slots;
  for (int i = 1; i < (int)s.size(); i++) {
    int j = i;
    while (j > 0 && s[j].price < s[j - 1].price) {
      std::swap(s[j], s[j - 1]);
      if (building == j) building = j - 1;
      else if (building == j - 1) building = j;
      j--;
    }
  }
  if (city.producing != NONE) city.producing = building;
}

void renameCity(Game& g, City& city, const std::string& name) { city.name = name; }

}  // namespace game
}  // namespace w2
