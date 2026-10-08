// Headless tests for the rules core: the Lua remake's love2d/test/run.lua,
// ported (by way of web/test/run.js). No SDL, no graphics.
//
//     cpp/build/w2test            -- from the repository root
//
// Every assertion cites the rule it checks; docs/rules.md is the spec. Where
// the Lua counts from 1 these count from 0, so a city's first production
// slot is slot 0 here.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "util/util.hpp"
#include "warlords/ai.hpp"
#include "warlords/ai/core.hpp"
#include "warlords/ai/diplomacy.hpp"
#include "warlords/ai/groups.hpp"
#include "warlords/aicard.hpp"
#include "warlords/armytype.hpp"
#include "warlords/combat.hpp"
#include "warlords/cues.hpp"
#include "warlords/diplomacy.hpp"
#include "warlords/game.hpp"
#include "warlords/hero.hpp"
#include "warlords/history.hpp"
#include "warlords/move.hpp"
#include "warlords/pck.hpp"
#include "warlords/quest.hpp"
#include "warlords/randommap.hpp"
#include "warlords/report.hpp"
#include "warlords/rules.hpp"
#include "warlords/save.hpp"
#include "warlords/scn.hpp"
#include "warlords/site.hpp"
#include "warlords/slots.hpp"
#include "warlords/uidata.hpp"

using namespace w2;
namespace movement = w2::move;

static const std::string DATA = "original";
static const std::vector<std::string> SCENARIOS = {"ERYTHEA", "ILLURIA", "DRAGON", "HADESHA", "ISLADIA", "SORCERY", "TUTORIA"};

static int passed = 0, failed = 0;

static void ok(bool cond, const std::string& what) {
  if (cond) passed++;
  else { failed++; printf("  FAIL  %s\n", what.c_str()); }
}

static std::string show(bool v) { return v ? "true" : "false"; }
template <class T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
static std::string show(T v) { return std::is_signed_v<T> && (long long)v == NONE ? "nil" : std::to_string(v); }
static std::string show(const std::string& v) { return v; }
static std::string show(const char* v) { return v ? v : "nil"; }
template <class T>
static std::string show(T* p) { return p ? fmt("%p", (const void*)p) : "nil"; }

template <class A, class B>
static void eq(const A& got, const B& want, const std::string& what) {
  if (got == want) passed++;
  else { failed++; printf("  FAIL  %s: got %s, want %s\n", what.c_str(), show(got).c_str(), show(want).c_str()); }
}
static void eq(const std::string& got, const char* want, const std::string& what) { eq(got, std::string(want), what); }

struct GameOpts {
  double seed = 0;
  std::vector<std::pair<std::string, int>> options;
  std::map<int, game::SideSetup> sides;
  bool greatest = false;
};
static std::unique_ptr<Game> newGame(const std::string& scenario, GameOpts o = GameOpts()) {
  game::NewGameOptions n;
  n.seed = o.seed;
  n.options = o.options;
  n.sides = o.sides;
  n.greatest = o.greatest;
  return game::newGame(DATA, scenario, n);
}
static GameOpts seed(double s) { GameOpts o; o.seed = s; return o; }
static GameOpts seedWith(double s, std::vector<std::pair<std::string, int>> options) {
  GameOpts o;
  o.seed = s;
  o.options = options;
  return o;
}
static game::SideSetup sideSetup(bool computer, int level = NONE, bool off = false) {
  game::SideSetup s;
  s.computer = computer;
  s.level = level;
  s.off = off;
  return s;
}

static Army* addArmy(Game& g, int x, int y, int owner, int type, int strength, int moves = 0, int maxMoves = 0) {
  Army a;
  a.x = x; a.y = y; a.owner = owner; a.type = type; a.strength = strength;
  a.moves = moves; a.maxMoves = maxMoves;
  a.name = g.types.byId(type) ? g.types.byId(type)->name : "";
  return g.add(a);
}

// ------------------------------------------------------------------ dice

static void testDice() {
  printf("dice\n");
  Rng r(1234);
  int lo = 1 << 30, hi = -(1 << 30);
  for (int i = 0; i < 20000; i++) {
    int v = r.dice(1, 100, 0);
    lo = std::min(lo, v); hi = std::max(hi, v);
  }
  eq(lo, 1, "1d100 lower bound");
  eq(hi, 100, "1d100 upper bound");
  lo = 1 << 30; hi = -(1 << 30);
  for (int i = 0; i < 20000; i++) {
    int v = r.dice(3, 500, 500);
    lo = std::min(lo, v); hi = std::max(hi, v);
  }
  ok(lo >= 503, "3d500+500 lower bound");
  ok(hi <= 2000, "3d500+500 upper bound");
  std::set<int> seen;
  for (int i = 0; i < 2000; i++) seen.insert(r.dice(1, 5, -1));
  for (int i = 0; i <= 4; i++) ok(seen.count(i), "dice(1,5,-1) reaches " + std::to_string(i));
  ok(!seen.count(5), "dice(1,5,-1) stays below 5");
  Rng a(7), b(7);
  bool same = true;
  for (int i = 0; i < 100; i++) if (a.dice(2, 6, 0) != b.dice(2, 6, 0)) same = false;
  ok(same, "a seed reproduces the sequence");
}

static void testArmyTypes() {
  printf("ARMYTYPE.DAT\n");
  Types t;
  armytype::load(DATA + "/TERRAIN0/ARMYTYPE.DAT", t);
  eq((int)t.all.size(), 29, "29 army types");
  ok(t.byId(armytype::HERO) != nullptr, "type 28 exists");
  eq(t.byId(armytype::HERO)->name, "Hero", "type 28 is the Hero");
  eq(t.byId(armytype::SCOUTS)->name, "Scouts", "type 11 is Scouts");
  const ArmyType* hcav = nullptr;
  for (auto& a : t.all) if (a.name == "Heavy Cav.") hcav = &a;
  ok(hcav != nullptr, "Heavy Cav. is in the table");
  if (hcav) {
    eq(hcav->strength, 4, "Heavy Cav. strength");
    eq(hcav->time, 3, "Heavy Cav. production time");
    eq(hcav->cost, 8, "Heavy Cav. cost");
    eq(hcav->move, 16, "Heavy Cav. base move");
  }
  bool anyFly = false;
  for (auto& a : t.all) if (a.flies) anyFly = true;
  ok(anyFly, "some types fly");
}

static void testProduction() {
  printf("production slots and garrisons\n");
  Types t;
  armytype::load(DATA + "/TERRAIN0/ARMYTYPE.DAT", t);
  Rng r(42);
  auto sl = rules::citySlots({4, 11, 2}, t, r);
  eq((int)sl.size(), 3, "one slot per production type");
  bool sorted = true;
  for (size_t i = 1; i < sl.size(); i++) if (sl[i].price < sl[i - 1].price) sorted = false;
  ok(sorted, "slots are sorted by price");
  bool okBounds = true;
  for (int n = 1; n <= 2000; n++) {
    Rng rn(n);
    for (auto& s : rules::citySlots({1, 2, 3, 4}, t, rn)) {
      const ArmyType* base = t.byId(s.type);
      if (s.strength < 1 || s.strength > 9) okBounds = false;
      if (s.strength < base->strength - 1 || s.strength > base->strength + 1) okBounds = false;
      if (s.move < 6) okBounds = false;
      if (s.move > base->move + 4) okBounds = false;
      if (s.time < 1 || s.time > base->time + 1) okBounds = false;
    }
  }
  ok(okBounds, "nudged stats stay within the documented range");
  bool anyFlier = false;
  for (auto& s : sl) if (t.byId(s.type)->flies) anyFlier = true;
  int best4 = rules::bestSlot(sl, 4, t, false);
  if (anyFlier) ok(best4 != NONE, "purpose 4 finds a flier");
  else ok(best4 == NONE, "purpose 4 refuses a city with no fliers");
  int fast = rules::bestSlot(sl, 6, t, false);
  int strong = rules::bestSlot(sl, 3, t, false);
  ok(fast != NONE && strong != NONE, "purposes 3 and 6 always choose something");
  eq(rules::cityDefence(2), 1, "fewer than 3 types: defence 1");
  eq(rules::cityDefence(3), 2, "3 types: defence 2");
  eq(rules::cityDefence(4), 2, "4 types: defence 2");
  Slot slot{4, "x", 8, 2, 8, 12, 0};
  eq(rules::armyFromSlot(slot, true).strength, 9, "Enhanced caps strength at 9");
  eq(rules::armyFromSlot(slot, false).strength, 8, "no Enhanced, no bonus");
  eq(rules::armyFromSlot(slot, false).upkeep, 4, "upkeep is half the slot cost");
}

static void testGame(const std::string& scenario) {
  printf("game: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(3));
  Game& g = *gp;
  ok(g.sides.size() >= 2, "at least two sides in play");
  eq((int)g.armies.size(), (int)g.map->cities.size(), "exactly one starting army per city");
  for (Side* s : g.sides) {
    eq((int)game::sideCities(g, *s).size(), 1, s->name + " owns only its capital");
    eq(game::sideCities(g, *s)[0]->index, s->capital->index, s->name + "'s city is its capital");
  }
  for (auto& c : g.map->cities) {
    for (auto& slot : c.slots) ok(slot.type != armytype::NAVY, c.name + " does not produce Navy");
    eq(c.defence, rules::cityDefence((int)c.slots.size()), c.name + " defence");
    ok(c.slots.size() <= 4, c.name + " has at most 4 slots");
  }
  for (Army* a : g.armies) {
    ok(g.types.byId(a->type) != nullptr, "army has a real type");
    eq(a->moves, 0, "a starting garrison has no moves");
  }
  std::map<int, int> perTile;
  for (Army* a : g.armies) {
    int k = a->y * g.map->width + a->x;
    perTile[k]++;
    ok(perTile[k] <= rules::MAX_STACK, "stack limit holds");
  }
  auto hp = newGame(scenario, seed(3));
  bool same = hp->armies.size() == g.armies.size();
  for (size_t i = 0; i < g.armies.size() && same; i++) {
    Army* a = g.armies[i];
    Army* b = hp->armies[i];
    if (b->type != a->type || b->strength != a->strength) same = false;
  }
  ok(same, "the same seed rebuilds the same game");
}

static void testTurnLoop(const std::string& scenario) {
  printf("turn loop: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(5));
  Game& g = *gp;
  Side* side = game::begin(g);
  ok(side != nullptr, "the first side is to play");
  for (Army* a : game::sideArmies(g, *side)) eq(a->moves, a->maxMoves, "a garrison starts the turn with its full move");
  int gold0 = side->gold;
  eq(side->income, side->capital->income, "income is the capital's income");
  ok(side->gold == std::max(0, gold0), "gold was applied");
  {
    Army* one = game::sideArmies(g, *side)[0];
    one->done = true; one->offered = true; one->fortified = true;
    for (size_t i = 0; i < g.sides.size(); i++) game::endTurn(g);
    eq(one->done, false, "done for this turn wears off with the turn");
    eq(one->offered, false, "and so does having been offered this pass");
    eq(one->fortified, true, "but digging in outlasts it");
    one->fortified = false;
  }
  Army* a = game::sideArmies(g, *side)[0];
  a->moves = 5;
  int before = a->maxMoves;
  for (size_t i = 0; i < g.sides.size(); i++) game::endTurn(g);
  eq(a->moves, before + rules::MOVE_CARRY, "at most 2 unused moves carry over");
  a->moves = 1;
  for (size_t i = 0; i < g.sides.size(); i++) game::endTurn(g);
  eq(a->moves, before + 1, "carry-over below the cap is exact");

  City* city = side->capital;
  game::setProduction(g, *city, 0);
  int want = city->slots[0].time;
  size_t n0 = game::sideArmies(g, *side).size();
  for (size_t i = 0; i < want * g.sides.size(); i++) game::endTurn(g);
  ok(game::sideArmies(g, *side).size() == n0 + 1, fmt("%s produced after %d turns", city->name.c_str(), want));
  Army* built = nullptr;
  for (Army* army : game::sideArmies(g, *side)) if (army->homeCity == city->index && army != a) built = army;
  ok(built != nullptr, "the new army belongs to the city that built it");
  if (built) {
    eq(built->upkeep, city->slots[0].cost / 2, "upkeep is half the slot cost");
    eq(built->x, city->x, "the new army stands in its city");
  }
  // a side with no cities stays in play until the round ends (8065:18ab),
  // then goes, with its armies, its gold and its diplomacy
  Side* victim = g.sides.back();
  for (City* c : game::sideCities(g, *victim)) c->ownerIndex = NONE;
  victim->gold = 500;
  g.diplomacy.state[victim->index * 8] = diplomacy::WAR;
  ok(!game::sideArmies(g, *victim).empty(), "the beaten side still has armies");
  int roundOf = g.turn;
  while (g.turn == roundOf) {
    ok(victim->alive, "a side with no cities is in play until the round ends");
    game::endTurn(g);
  }
  ok(!victim->alive, "a side with no cities is eliminated as the round ends");
  eq((int)game::sideArmies(g, *victim).size(), 0, "the eliminated side's armies are gone");
  eq(victim->gold, 0, "the eliminated side's gold is gone");
  eq(diplomacy::state(g, victim->index, 0), diplomacy::INTERMEDIATE,
     "the eliminated side's diplomacy is reset to uneasy");
  ok(!g.ending.fallen.empty() && g.ending.fallen[0].side == victim, "the elimination is announced");
  auto g2 = newGame(scenario, seed(6));
  game::begin(*g2);
  eq(g2->turn, 1, "the game starts on turn 1");
  for (size_t i = 0; i < g2->sides.size(); i++) game::endTurn(*g2);
  eq(g2->turn, 2, "a full round advances the turn");
}

static void testVectoring(const std::string& scenario) {
  printf("vectoring: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(9));
  Game& g = *gp;
  Side* side = game::begin(g);
  City* from = side->capital;
  City* dest = nullptr;
  for (auto& c : g.map->cities) if (!dest && c.ownerIndex == NONE && !c.slots.empty()) dest = &c;
  ok(dest != nullptr, "found a neutral city to capture");
  if (!dest) return;
  dest->ownerIndex = side->index;
  for (Army* army : game::armiesAt(g, dest->x, dest->y)) army->owner = side->index;
  game::setProduction(g, *from, 0);
  game::vector(g, *from, dest);
  int time = from->slots[0].time;
  size_t atDest = game::armiesAt(g, dest->x, dest->y).size();
  for (size_t i = 0; i < (time + 2) * g.sides.size(); i++) game::endTurn(g);
  ok(game::armiesAt(g, dest->x, dest->y).size() > atDest, "a vectored army arrives two turns after it is built");
  game::setProduction(g, *from, NONE);
  ok(from->vectorTo == NONE, "vectoring only sticks while the city is building");
  std::vector<City*> senders;
  for (auto& c : g.map->cities) {
    if (&c != dest && senders.size() < 5) {
      c.ownerIndex = side->index;
      senders.push_back(&c);
    }
  }
  for (int i = 0; i < 4; i++) ok(game::vector(g, *senders[i], dest), "vector " + std::to_string(i + 1) + " is taken");
  ok(!game::vector(g, *senders[4], dest), "a fifth is refused");
  eq(senders[4]->vectorTo, NONE, "and the fifth city keeps no vector");
  eq((int)game::vectoredTo(g, dest).size(), 4, "four cities send to the destination");
  ok(game::vector(g, *senders[0], dest), "a city already sending there may be set again");
  ok(game::vector(g, *senders[0], senders[0]), "vectoring a city to itself lifts the vector");
  eq(senders[0]->vectorTo, NONE, "and leaves it with none");
  eq(game::nearestCity(g, dest->x, dest->y, side), dest, "a click on a city picks it");
  eq(game::nearestCity(g, dest->x + 1, dest->y - 1, side), dest, "and a click next to it too");
}

static void testBuyProduction(const std::string& scenario) {
  printf("buying production: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(17));
  Game& g = *gp;
  Side* side = game::begin(g);
  City* city = side->capital;
  side->gold = 5000;
  auto list = game::buyableTypes(g);
  ok(!list.empty() && list.size() <= 20, "the Build Production screen has room for every type");
  for (auto* a : list) ok(a->price >= 0, a->name + " is for sale");
  eq(game::cannotBuy(g, *side, *city, *g.types.byId(city->slots[0].type)), "has it", "a type already built is greyed out");
  int n0 = (int)city->slots.size();
  eq(game::buySlot(*city), n0 < 4 ? n0 : 0, "the starting slot");
  const ArmyType* pick = nullptr;
  for (auto* a : list) if (game::cannotBuy(g, *side, *city, *a).empty() && (!pick || a->price > pick->price)) pick = a;
  ok(pick != nullptr, "something to buy");
  if (!pick) return;
  if (n0 == 4) { city->slots.pop_back(); n0 = 3; }
  game::setProduction(g, *city, 0);
  Slot building = city->slots[0];
  int gold = side->gold;
  game::buyProduction(g, *side, *city, n0, pick->id);
  eq(side->gold, gold - pick->price, "the price comes off the treasury");
  const Slot& slot = city->slots[n0];
  eq(slot.type, pick->id, "the bought type fills the slot");
  eq(slot.strength, pick->strength, "a bought type takes ARMYTYPE's strength unmodified");
  eq(slot.time, pick->time, "and its time");
  eq(slot.move, pick->move, "and its move");
  eq(city->defence, rules::cityDefence((int)city->slots.size()), "defence follows the number of types");
  eq(city->producing, 0, "buying into a new slot leaves production alone");
  game::sortProduction(*city);
  for (size_t i = 1; i < city->slots.size(); i++) ok(city->slots[i - 1].price <= city->slots[i].price, "sorted by price, cheapest first");
  ok(city->slots[city->producing] == building, "the city builds the same type after sorting");
  game::vector(g, *city, &g.map->cities[0] != city ? &g.map->cities[0] : &g.map->cities[1]);
  const ArmyType* other = nullptr;
  for (auto* a : list) if (!other && game::cannotBuy(g, *side, *city, *a).empty()) other = a;
  game::buyProduction(g, *side, *city, city->producing, other->id);
  eq(city->producing, NONE, "production stops when its type is bought over");
  eq(city->vectorTo, NONE, "and the vector goes with it");
  game::renameCity(g, *city, "Newtown");
  auto h = save::decode(save::encode(g), DATA);
  eq(h->map->cities[city->index].name, "Newtown", "a renamed city keeps its name");
  eq(h->map->cities[city->index].slots.back().type, city->slots.back().type, "bought production survives a save");
}

static void testReports(const std::string& scenario) {
  printf("reports: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(23));
  Game& g = *gp;
  Side* side = game::begin(g);
  auto f = report::figures(g, *side, report::CITY);
  eq(f.result, (int)game::sideCities(g, *side).size(), "the City report counts the side's cities");
  eq(f.max % 2, 0, "the top of the scale is even");
  int unused = 0;
  for (int i = 0; i < 8; i++) if (f.out[i]) unused++;
  eq(unused, 8 - (int)g.sides.size(), "only the sides in the game get a bar");
  auto a = report::figures(g, *side, report::ARMY);
  int mine = 0;
  for (Army* army : g.armies) if (army->owner == side->index) mine++;
  eq(a.result, mine, "the Army report counts the side's armies");
  eq(report::figures(g, *side, report::GOLD).result, side->gold, "the Gold report is the treasury");
  auto w = report::figures(g, *side, report::WINNING);
  eq(w.max, 100, "the Winning report's scale is fixed");
  ok(w.result >= 0 && w.result <= 7, "and says where the side comes");
  side->gold = 100000;
  eq(report::figures(g, *side, report::WINNING).result, 0, "a rich enough side comes first");
  eq(report::score(g, *side), 500, "and its score stops at 500");
  City* city = side->capital;
  game::setProduction(g, *city, 0);
  city->countdown = 1;
  int turns = 0;
  do { game::endTurn(g); turns++; } while (!(g.side == side || turns > 20));
  ok(!side->produced.empty(), "the side's production is logged");
  if (!side->produced.empty()) {
    eq(side->produced[0].city, city->index, "with the city that built it");
    eq(side->produced[0].kind, "built", "kept at home");
  }
  eq(report::figures(g, *side, report::PRODUCTION).result, (int)side->produced.size(), "the Production report counts what was built");
}

static void testDisband(const std::string& scenario) {
  printf("disband: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(29));
  Game& g = *gp;
  Side* side = game::begin(g);
  Army* h = hero::recruit(g, *side, *side->heroOffer).first;
  auto stack = game::armiesAt(g, h->x, h->y);
  h->strength = 5;
  auto fight = combat::stackStrengths(g, {h}, h->x, h->y);
  eq(fight[h], 5 + combat::HERO_TABLE[5] + 1, "View > Stack: a hero's strength, its table value and its standard");
  size_t n = g.armies.size();
  side->quest = std::make_shared<Quest>();
  side->quest->type = quest::OCCUPY;
  side->quest->hero = h;
  Item* std = h->items[0];
  game::disband(g, *side, stack);
  eq(g.armies.size(), n - stack.size(), "the stack is gone");
  eq((int)game::armiesAt(g, h->x, h->y).size(), 0, "from its tile");
  eq(std->status, 1, "the hero's standard lies on the ground");
  ok(!side->quest, "and the quest is lost with the hero");
  Sign* sg = game::signAt(g, 18, 12);
  ok(sg != nullptr, "a sign at (18, 12)");
  if (sg) {
    eq(sg->lines[0], "Traveller! Pray excuse the ", "its first line");
    eq(sg->lines[1], "sulphur fumes. Keep On!", "and its second");
    sg->lines[0] = "Hello there";
  }
  auto back = save::decode(save::encode(g), DATA);
  eq(game::signAt(*back, 18, 12)->lines[0], "Hello there", "an edited sign survives a save");
  g.map->fightOrder[side->index][3] = 26;
  back = save::decode(save::encode(g), DATA);
  eq(back->map->fightOrder[side->index][3], 26, "so does an edited fight order");
  int diplo = side->diploScore;
  game::resign(g, *side);
  eq((int)game::sideCities(g, *side).size(), 0, "a side that resigns holds no city");
  eq((int)game::sideArmies(g, *side).size(), 0, "and has no army");
  ok(side->capital->razed, "its capital is ruins");
  eq(side->diploScore, diplo, "burning its own on resigning costs nothing");
}

static void testHistory(const std::string& scenario) {
  printf("history: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(41));
  Game& g = *gp;
  Side* side = game::begin(g);
  history::deed(g, side, history::FINDS, history::SAGE, 0, "Hero");
  history::deed(g, side, history::WON, side->index, 3, side->name);
  history::deed(g, side, history::EMERGES, 1, 0, "Hero");
  eq((int)g.deeds[side->index].size(), 2, "two deeds a side");
  std::vector<int> types = {g.deeds[side->index][0].type, g.deeds[side->index][1].type};
  std::sort(types.begin(), types.end());
  eq(types[0], history::EMERGES, "a lower type pushes out the higher");
  eq(types[1], history::WON, "and the lower of the two stays");
  history::deed(g, side, history::PEACE, 0, 1, "");
  eq((int)g.deeds[side->index].size(), 2, "a higher type does not get in");
  int guard = 0;
  while (g.turn < 3 && guard < 50) { game::endTurn(g); guard++; }
  ok(!g.history.empty(), "a record for the round");
  if (!g.history.empty()) {
    const auto& r = g.history[0];
    eq(r.owners.size(), g.map->cities.size(), "every city's owner");
    ok(r.events.size() >= 2, "with the deeds");
  }
  ok(!g.deeds.count(side->index) || g.deeds[side->index].empty(), "and the slots cleared");
  auto back = save::decode(save::encode(g), DATA);
  eq(back->history.size(), g.history.size(), "history survives a save");
  Side* foe = g.sides[1];
  Item std;
  std.index = foe->index;
  Army dead;
  dead.owner = foe->index;
  dead.type = 28;
  dead.items = {&std};
  history::tally(g, &dead, side->index);
  eq(history::triumph(g, side->index, foe->index, history::HEROES), 1, "a hero killed");
  eq(history::triumph(g, side->index, foe->index, history::STANDARDS), 1, "and the standard it carried");
  eq(history::triumph(g, foe->index, foe->index, history::HEROES), 1, "the loser counts its own loss");
}

static void testSetup() {
  printf("new-game setup\n");
  GameOpts o = seedWith(5, {{"quests", 0}});
  o.sides[0] = sideSetup(false);
  o.sides[1] = sideSetup(true, 2);
  o.sides[2] = sideSetup(false, NONE, true);
  auto gp = newGame("ERYTHEA", o);
  Game& g = *gp;
  eq(g.map->options.quests, 0, "the options chosen");
  ok(!g.map->sides[0].computer, "side 0 human");
  ok(g.map->sides[1].computer && g.map->sides[1].level == 2, "side 1 a Warlord");
  ok(!g.map->sides[2].inUse, "side 2 left out");
  City* cap = g.map->sides[2].capital;
  eq(cap ? cap->ownerIndex : NONE, NONE, "and its capital neutral");
  for (Side* s : g.sides) ok(s->index != 2, "not among the sides in play");
  GameOpts q = seedWith(5, {{"quickStart", 1}});
  q.sides[2] = sideSetup(false, NONE, true);
  auto quick = newGame("ERYTHEA", q);
  std::map<int, int> counts;
  int neutral = 0;
  for (auto& c : quick->map->cities) {
    if (c.ownerIndex == NONE) neutral++;
    else counts[c.ownerIndex]++;
  }
  eq(neutral, 0, "with Quick Start no city is left neutral");
  ok(!counts.count(2), "a side left out gets none");
  int least = 1 << 30, most = 0;
  for (Side* s : quick->sides) {
    least = std::min(least, counts[s->index]);
    most = std::max(most, counts[s->index]);
    ok(s->capital->ownerIndex == s->index, "each side keeps its capital");
  }
  ok(most - least <= 1, "and the cities go round evenly");
  for (Army* a : quick->armies) {
    City* c = game::cityAt(*quick, a->x, a->y);
    if (c) eq(a->owner, c->ownerIndex, "a dealt city's garrison is its owner's");
  }
  GameOpts sl = seed(5);
  sl.sides[2] = sideSetup(false, NONE, true);
  auto slow = newGame("ERYTHEA", sl);
  neutral = 0;
  for (auto& c : slow->map->cities) if (c.ownerIndex == NONE) neutral++;
  ok(neutral > 0, "without it the other cities stay neutral");
}

static void testSage(const std::string& scenario) {
  printf("sage: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(31));
  Game& g = *gp;
  Side* side = game::begin(g);
  int hx = side->capital->x, hy = side->capital->y;
  for (auto& s : g.map->sites) { s.rich = false; s.revealed = 0xff; s.searched = false; }
  eq((int)site::sageList(g, *side, hx, hy).size(), 0, "no rich site unshown: nothing to tell");
  auto d2 = [&](const Site* s) { return (s->x - hx) * (s->x - hx) + (s->y - hy) * (s->y - hy); };
  std::vector<Site*> near;
  for (auto& s : g.map->sites) if (std::sqrt((double)d2(&s)) < site::SAGE_RANGE) near.push_back(&s);
  std::stable_sort(near.begin(), near.end(), [&](Site* a, Site* b) { return d2(a) < d2(b); });
  ok(near.size() >= 3, "three sites within reach");
  if (near.size() < 3) return;
  Site* g1 = near[2];
  Site* g2 = near[0];
  Site* isite = near[1];
  for (Site* s : {g1, g2, isite}) { s->rich = true; s->revealed = 0; }
  g1->content = site::GOLD;
  g2->content = site::GOLD;
  isite->content = site::ITEM;
  isite->item = g.map->items[9].index;
  auto list = site::sageList(g, *side, hx, hy);
  eq((int)list.size(), 2, "gold once however many, and the item");
  const site::SageEntry* gold = nullptr;
  for (auto& e : list) if (e.kind == "gold") gold = &e;
  ok(gold != nullptr, "a gold entry");
  if (!gold) return;
  site::SageEntry goldE = *gold;
  eq(site::sageShow(g, *side, goldE, hx, hy), g2, "the nearest gold site is shown");
  ok(((g2->revealed >> side->index) & 1) == 1, "and marked shown to the side");
  eq(site::sageShow(g, *side, goldE, hx, hy), g1, "then the next one");
  auto items = site::sageList(g, *side, hx, hy);
  eq((int)items.size(), 1, "only the item is left");
  if (!items.empty()) eq(items[0].item, &g.map->items[9], "by its record");
  int before = side->gold;
  int n = site::sageGem(g, *side);
  ok(n >= 503 && n <= 2000, "a gem is 3d500 + 500");
  eq(side->gold, before + n, "and is paid");
  g.map->options.hiddenMap = 1;
  g.explored.clear();
  auto r = site::sageMap(g, *side, 3, 3);
  int x0 = r[0], y0 = r[1], w = r[2], h = r[3];
  eq(x0, 0, "the patch is kept on the map");
  ok(w >= 16 && w <= 25 && h >= 16 && h <= 25, "16-25 tiles a side");
  ok(game::seen(g, side->index, x0 + w - 1, y0 + h - 1), "and uncovered");
}

static void testHeroItems(const std::string& scenario) {
  printf("hero items: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(29));
  Game& g = *gp;
  Side* side = game::begin(g);
  Army* h = hero::recruit(g, *side, *side->heroOffer).first;
  eq(h->items[0], &g.map->items[side->index], "the first hero carries the side's own standard record");
  eq(h->items[0]->status, 3, "which is carried");
  eq((int)hero::itemsHere(g, h).size(), 1, "nothing else is listed yet");
  Item* it = &g.map->items[8];
  it->status = 1; it->x = h->x; it->y = h->y;
  auto list = hero::itemsHere(g, h);
  eq((int)list.size(), 2, "an item on the hero's tile is listed");
  if (list.size() > 1) eq(list[1], it, "after what the hero carries");
  hero::takeItem(g, h, it);
  eq(it->status, 3, "taking an item carries it");
  eq((int)h->items.size(), 2, "the hero has it");
  hero::dropItem(g, h, it);
  eq(it->status, 1, "dropping puts it on the ground");
  eq(it->x, h->x, "where the hero stands");
  eq((int)h->items.size(), 1, "and the hero no longer has it");
  ok(!game::plantFlag(g, *side, h), "no flag in a city");
  int fx = NONE, fy = NONE;
  for (int dy = -3; dy <= 4; dy++) {
    for (int dx = -3; dx <= 4; dx++) {
      int x = h->x + dx, y = h->y + dy;
      int t = scn::terrainAt(*g.map, x, y);
      if (fx == NONE && (t == movement::PLAIN || t == movement::ROAD) && !game::cityAt(g, x, y)) { fx = x; fy = y; }
    }
  }
  ok(fx != NONE, "found open ground near the capital");
  h->x = fx; h->y = fy;
  ok(game::plantFlag(g, *side, h), "the flag is planted");
  eq(game::standardAt(g, *side).first, fx, "where the hero stands");
  ok(!game::plantFlag(g, *side, h), "and only once");
  City* city = side->capital;
  game::setProduction(g, *city, 0);
  city->countdown = 1;
  ok(game::vectorToStandard(g, *city, *side), "a city may vector to the standard");
  size_t before = game::armiesAt(g, fx, fy).size();
  for (size_t i = 0; i < 3 * g.sides.size() + 1; i++) game::endTurn(g);
  ok(game::armiesAt(g, fx, fy).size() > before, "a vectored army arrives at the standard");
  hero::takeItem(g, h, &g.map->items[side->index]);
  eq(game::standardAt(g, *side).first, NONE, "a standard picked up is no longer planted");
}

static void testMovement(const std::string& scenario) {
  printf("movement: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(11));
  Game& g = *gp;
  Side* side = game::begin(g);
  Army* army = game::sideArmies(g, *side)[0];
  std::vector<Army*> stack = {army};
  eq(movement::COST[movement::MOUNTAINS], 0, "mountains are impassable");
  eq(movement::COST[movement::ROAD], 1, "a road costs 1");
  eq(movement::COST[movement::HILLS], 6, "hills cost 6");
  auto& grid = movement::grid(g, side->index);
  int roaded = 0;
  bool roadCostOk = true;
  for (int y = 0; y < g.map->height; y++) {
    for (int x = 0; x < g.map->width; x++) {
      if (scn::roadAt(*g.map, x, y) % 0x20 != 0) {
        roaded++;
        if (grid[y * g.map->width + x] % 8 != 1) roadCostOk = false;
      }
    }
  }
  ok(roaded > 0, "the scenario has roads");
  ok(roadCostOk, "every road tile costs 1");
  const int W = movement::WATER_F;
  const int land = 2, water = 2 | W;
  auto sc = [](std::optional<int> v) { return v ? *v : NONE; };
  eq(sc(movement::stepCost(land, water, movement::LAND, false, false, 10)), NONE, "a land stack cannot enter open water");
  eq(sc(movement::stepCost(land, 0, movement::LAND, false, false, 10)), NONE, "nothing enters a cost-0 tile on land");
  eq(sc(movement::stepCost(land, 6 | movement::HILLS_F, movement::LAND, false, false, 10)), 6, "hills cost 6 without the bonus");
  eq(sc(movement::stepCost(land, 6 | movement::HILLS_F, movement::LAND, false, true, 10)), 2, "the hills bonus brings them to 2");
  eq(sc(movement::stepCost(land, 4 | movement::FOREST_F, movement::LAND, true, false, 10)), 2, "the woods bonus brings forest to 2");
  const int cross = 1 | W | movement::CROSS_F;
  eq(sc(movement::stepCost(land, cross, movement::LAND, false, false, 10)), 1, "a crossing tile is free of the water charge");
  eq(sc(movement::stepCost(cross, water, movement::LAND, false, false, 10)), 2 + 10, "stepping from a crossing into water costs the penalty");
  eq(sc(movement::stepCost(cross, water, movement::LAND, false, false, 20)), 2 + 20, "a move aimed at water pays 20");
  eq(sc(movement::stepCost(land, 0, movement::FLYING, false, false, 10)), 2, "a flier crosses mountains at 2");
  eq(sc(movement::stepCost(land, water, movement::FLYING, false, false, 10)), 2, "a flier crosses water at 2");
  eq(sc(movement::stepCost(land, 1, movement::FLYING, false, false, 10)), 1, "a flier pays 1 on a road");
  eq(sc(movement::stepCost(land, 6 | movement::HILLS_F, movement::FLYING, false, false, 10)), 2, "a flier pays 2 over hills");
  eq(sc(movement::stepCost(land, 0 + movement::CITY_F, movement::FLYING, false, false, 10)), NONE,
     "a flier cannot cross a city that is not its own (1555:08bf)");
  eq(movement::distance(5, 5, 6, 6), 1, "a diagonal neighbour is one away");
  eq(movement::distance(5, 5, 7, 6), 2, "a knight's move is two");
  eq(movement::direction(5, 5, 6, 4), 1, "up and right is north-east");
  eq(movement::direction(5, 5, 9, 4), 1, "however far right");
  eq(movement::direction(5, 5, 5, 9), 4, "straight down is south");
  eq(sc(movement::stepCost(water, land, movement::BOAT, false, false, 10)), NONE, "a boat cannot go ashore");
  eq(sc(movement::stepCost(water, water, movement::BOAT, false, false, 10)), 2, "a boat moves on water");
  army->moves = 7;
  Army m3, m9;
  m3.moves = 3;
  m9.moves = 9;
  eq(movement::stackMoves({army, &m3, &m9}), 3, "a stack moves at the pace of its slowest army");

  army->moves = army->maxMoves;
  int fromX = army->x, fromY = army->y;
  bool found = false;
  int tx = 0, ty = 0;
  for (int dx = -6; dx <= 6; dx++) {
    for (int dy = -6; dy <= 6; dy++) {
      int x = fromX + dx, y = fromY + dy;
      if (!found && x >= 0 && y >= 0 && x < g.map->width && y < g.map->height && !game::cityAt(g, x, y)) {
        auto p = movement::findPath(g, stack, fromX, fromY, x, y);
        if (p && p->size() >= 2 && p->back().cost < movement::PAST_SHORE &&
            !movement::has(movement::grid(g, side->index)[y * g.map->width + x], movement::WATER_F)) {
          found = true; tx = x; ty = y;
        }
      }
    }
  }
  ok(found, "found somewhere to walk to");
  if (found) {
    int before = army->moves;
    auto r = movement::moveTo(g, stack, tx, ty);
    ok(r.steps > 0, "the stack moved");
    eq(army->moves, std::max(0, before - r.spent), "the cost was charged to the army");
    ok(army->x != fromX || army->y != fromY, "the army is somewhere else");
  }
  army->moves = 0;
  auto r = movement::moveTo(g, stack, fromX, fromY + 1);
  eq(r.steps, 0, "an army with no moves stays put");
  City* enemy = nullptr;
  for (auto& c : g.map->cities) if (!enemy && c.ownerIndex != side->index) enemy = &c;
  ok(enemy != nullptr, "there is a city to attack");
  if (enemy) {
    army->moves = 99;
    army->x = enemy->x;
    army->y = enemy->y - 1;
    auto path = movement::findPath(g, stack, army->x, army->y, enemy->x, enemy->y);
    if (path && !path->empty()) {
      auto res = movement::walk(g, stack, *path);
      eq(res.stopped, "attack", "stepping into an enemy city starts an attack");
      ok(res.attack && res.attack->city == enemy, "the attack names the city");
      eq(army->x, enemy->x, "the attacker has not entered the city");
      ok(army->y == enemy->y - 1, "the attacker stayed where it was");
    }
    auto& grid2 = movement::grid(g, side->index);
    eq(grid2[enemy->y * g.map->width + enemy->x] % 8, 0, "an enemy city is impassable in the cost grid");
  }
  {
    Army* a = addArmy(g, army->x, army->y, side->index, 11, 9, 20, 20);
    a->upkeep = 1;
    int farX = 0, farY = 0;
    std::optional<movement::Preview> route;
    for (int d = 12; d <= 30; d += 2) {
      farX = std::min(g.map->width - 2, a->x + d);
      farY = a->y;
      route = movement::preview(g, {a}, farX, farY);
      if (route && route->reach < (int)route->path.size()) break;
    }
    if (route) {
      ok(!route->path.empty(), "a stack under orders has a route to draw");
      ok(route->reach <= (int)route->path.size(), "it can walk no more of it than there is");
      eq(route->path.back().x, farX, "the route ends where it was sent");
      eq(route->path.back().y, farY, "on that row");
      int px = a->x, py = a->y;
      bool straight = true;
      for (auto& step : route->path) {
        if (std::max(std::abs(step.x - px), std::abs(step.y - py)) != 1) straight = false;
        px = step.x; py = step.y;
      }
      ok(straight, "and it is a chain of single steps");
      r = movement::moveTo(g, {a}, farX, farY);
      eq((int)r.walked.size(), r.steps, "a walk reports the tiles it covered, one per step taken");
      if (r.steps > 0) {
        eq(r.walked.back().first, a->x, "ending where the stack now stands");
        eq(r.walked.back().second, a->y, "on that row");
      }
      auto left = movement::preview(g, {a}, farX, farY);
      if (left) ok(left->reach < (int)left->path.size(), "what is left of it is out of reach this turn");
    }
    g.remove(a);
  }
}

static void testStackLimit(const std::string& scenario) {
  printf("stack limit: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(13));
  Game& g = *gp;
  Side* side = game::begin(g);
  Army* army = game::sideArmies(g, *side)[0];
  int tx = army->x, ty = army->y - 1;
  if (ty < 0) { tx = army->x; ty = army->y + 1; }
  for (int i = 0; i < rules::MAX_STACK; i++) {
    Army* a = addArmy(g, tx, ty, side->index, army->type, 1, 10, 10);
    a->homeCity = army->homeCity;
  }
  eq((int)game::armiesAt(g, tx, ty).size(), rules::MAX_STACK, "the tile is full");
  army->moves = 99;
  auto r = movement::moveTo(g, {army}, tx, ty);
  eq(r.steps, 0, "a stack cannot stop on a full tile");
  ok(army->x != tx || army->y != ty, "the army stayed off the full tile");
}

static void testCombat(const std::string& scenario) {
  printf("combat: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(17));
  Game& g = *gp;
  Side* side = game::begin(g);
  eq(combat::HERO_TABLE[0], 0, "a strength-0 hero commands nothing");
  eq(combat::HERO_TABLE[4], 1, "strength 4 gives +1");
  eq(combat::HERO_TABLE[9], 3, "strength 9 gives +3");
  eq(combat::CITY, 0, "city is class 0");
  eq(combat::OPEN, 1, "open is class 1");
  eq(combat::WOODS, 2, "woods is class 2");
  eq(combat::HILLS, 3, "hills is class 3");
  std::set<int> seen;
  for (int y = 0; y < g.map->height; y += 7) {
    for (int x = 0; x < g.map->width; x += 7) {
      int cls = combat::terrainClass(g, x, y);
      int t = scn::terrainAt(*g.map, x, y);
      seen.insert(cls);
      if (t == movement::FOREST) eq(cls, combat::WOODS, "forest fights as woods");
      if (t == movement::HILLS || t == movement::MOUNTAINS) eq(cls, combat::HILLS, "hills and mountains fight as hills");
      if (t == movement::CITY || t == movement::SITE) eq(cls, combat::CITY, "cities and sites fight as city");
      if (t == movement::PLAIN || t == movement::MARSH || t == movement::WATER) eq(cls, combat::OPEN, "plain, marsh and water fight as open");
    }
  }
  ok(seen.count(combat::OPEN), "the map has open ground");
  eq(g.map->combatCap, 5, "the combat cap is 5");
  const auto& row = g.map->fightOrder[0];
  std::set<int> used;
  for (int t = 0; t <= 28; t++) used.insert(row[t]);
  eq((int)used.size(), 29, "fight order is a permutation of 29 ranks");
  City* neutral = nullptr;
  for (auto& c : g.map->cities) if (!neutral && c.ownerIndex == NONE && c.defence == 2) neutral = &c;
  if (neutral) {
    int cls = combat::terrainClass(g, neutral->x, neutral->y);
    eq(combat::fortify(g, {}, neutral->x, neutral->y, cls), 1, "a neutral city's defence 2 fortifies for 1");
    game::setCityOwner(g, *neutral, g.sides.back()->index);
    eq(combat::fortify(g, {}, neutral->x, neutral->y, cls), 2, "an owned city's defence 2 fortifies for 2");
    game::setCityOwner(g, *neutral, NONE);
  }
  const ArmyType* siege = nullptr;
  for (auto& a : g.types.all) if (a.bonus[52] == combat::SIEGE) siege = &a;
  if (siege && neutral) {
    int cls = combat::terrainClass(g, neutral->x, neutral->y);
    Army s;
    s.type = siege->id;
    eq(combat::fortify(g, {&s}, neutral->x, neutral->y, cls), 0, "a Siege attacker cancels the city bonus");
  }
  Army boat;
  boat.type = 11; boat.strength = 9; boat.atSea = true;
  eq(combat::strength(g, boat, 5, combat::OPEN, movement::WATER), 4, "an army at sea on water fights at 4");
  eq(combat::strength(g, boat, 5, combat::OPEN, movement::SHORE), 4, "an army at sea on shore fights at 4");
  boat.atSea = false;
  ok(combat::strength(g, boat, 5, combat::OPEN, movement::PLAIN) > 4, "ashore it fights normally");
  Army strong;
  strong.type = 11; strong.strength = 9;
  eq(combat::strength(g, strong, 99, combat::OPEN, movement::PLAIN), 15, "strength is capped at 15");
  Army* a1 = game::sideArmies(g, *side)[0];
  City* enemy = nullptr;
  for (auto& c : g.map->cities) if (!enemy && c.ownerIndex != side->index) enemy = &c;
  auto l = combat::lines(g, {a1}, enemy->x, enemy->y);
  eq((int)l.attackers.size(), 1, "the attacking line is the stack");
  ok(!l.defenders.empty(), "the city has defenders");
  eq(l.city, enemy, "the battle names the city");
  for (Army* d : l.defenders) ok(d->owner != side->index, "no defender belongs to the attacker");
  bool sorted = true;
  for (size_t i = 1; i < l.defenders.size(); i++) {
    const auto& ownerRow = g.map->fightOrder[l.defOwner == NONE ? 8 : l.defOwner];
    if (ownerRow[l.defenders[i]->type] < ownerRow[l.defenders[i - 1]->type]) sorted = false;
  }
  ok(sorted, "the defending line is sorted by fight order");
  auto r = combat::resolve(g, l.attackers, l.defenders, enemy->x, enemy->y);
  ok(!r.log.empty(), "the battle logged at least one death");
  eq(r.log.size(), r.deadAttackers.size() + r.deadDefenders.size(), "one log entry per dead army");
  ok(r.won == r.defenders.empty(), "the attacker wins only if every defender is dead");
  auto odds = [&](int atkStrength, int nAtk, int defStrength, int nDef) {
    std::vector<Army> A(nAtk), D(nDef);
    std::vector<Army*> pa, pd;
    for (auto& a : A) { a.type = 11; a.strength = atkStrength; a.owner = side->index; pa.push_back(&a); }
    for (auto& d : D) { d.type = 11; d.strength = defStrength; d.owner = NONE; pd.push_back(&d); }
    int wins = 0;
    for (int i = 0; i < 200; i++) if (combat::resolve(g, pa, pd, 0, 0).won) wins++;
    return wins;
  };
  ok(odds(9, 8, 1, 1) > 190, "eight strong armies beat one weak one");
  ok(odds(1, 1, 9, 8) < 10, "one weak army loses to eight strong ones");
  int many = odds(3, 8, 9, 1);
  ok(many > 150, "many weak armies beat one strong one: " + std::to_string(many));
  eq(combat::ADVICE_BATTLES, 19, "the advisor fights 19 battles");
  auto [verdict, wins] = combat::advise(g, l.attackers, l.defenders, enemy->x, enemy->y);
  ok(!verdict.empty(), "the advisor has a verdict");
  ok(wins >= 0 && wins <= 19, "the advisor's win count is in range");
  eq(verdict, std::string(combat::ADVICE[wins / 2]), "the verdict is wins/2 into the table");
}

static void testCapture(const std::string& scenario) {
  printf("capture: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(23));
  Game& g = *gp;
  Side* side = game::begin(g);
  City* target = nullptr;
  for (auto& c : g.map->cities) if (!target && c.ownerIndex == NONE) target = &c;
  ok(target != nullptr, "found a neutral city");
  std::vector<Army*> stack;
  for (int i = 0; i < 8; i++) {
    Army* a = addArmy(g, target->x, target->y - 1, side->index, 11, 9, 20, 20);
    a->upkeep = 1;
    stack.push_back(a);
  }
  {
    Army* far = addArmy(g, 0, 0, side->index, 11, 9, 20, 20);
    far->upkeep = 1;
    movement::WalkResult walk;
    for (auto off : std::vector<std::pair<int, int>>{{-3, -3}, {3, -3}, {-3, 3}, {3, 3}, {0, -3}, {0, 3}, {-3, 0}, {3, 0}}) {
      far->x = target->x + off.first;
      far->y = target->y + off.second;
      far->moves = 20;
      walk = movement::moveTo(g, {far}, target->x, target->y);
      if (walk.stopped == "attack") break;
    }
    eq(walk.stopped, "attack", "walking at a city ends in an assault");
    if (walk.attack) {
      eq(walk.attack->x, target->x, "on the city's own tile");
      eq(walk.attack->y, target->y, "and row");
      eq(walk.attack->city, target, "which is the city");
    }
    ok(std::max(std::abs(far->x - target->x), std::abs(far->y - target->y)) == 1, "and the stack is left standing beside it, not in it");
    ok(far->x != target->x || far->y != target->y, "it did not walk in");
    eq(target->ownerIndex, NONE, "and the city is still the defender's");
    g.remove(far);
  }
  int before = (int)game::sideCities(g, *side).size();
  Battle r;
  {
    int i = target->y * MAP_W + target->x;
    int tile = g.map->tiles[i];
    size_t armies = g.armies.size();
    int from = stack[0]->x;
    r = game::decideAttack(g, stack, target->x, target->y);
    ok(r.won, "the fight is decided");
    eq(target->ownerIndex, NONE, "but the city is not yet taken");
    eq(g.map->tiles[i], tile, "nor its castle repainted");
    eq(g.armies.size(), armies, "nor the dead removed");
    eq(stack[0]->x, from, "nor the survivors moved in");
    eq(r.captured, (City*)nullptr, "and nothing is said to be captured");
    game::applyAttack(g, r);
    ok(g.map->tiles[i] != tile, "applied, the castle is in the victor's colours");
    int owner = target->ownerIndex;
    size_t n = g.armies.size();
    game::applyAttack(g, r);
    eq(g.armies.size(), n, "and applying again changes nothing");
    eq(target->ownerIndex, owner, "and the owner stays");
  }
  ok(r.won, "the overwhelming stack took the city");
  eq(r.lines.attackers.size(), stack.size(), "every attacker is in the line");
  eq(r.lines.city, target, "and the line knows it was a city");
  size_t down = 0;
  for (int s : r.log) { ok(s == 0 || s == 1, "the log says which side lost an army"); down++; }
  eq(down, r.deadAttackers.size() + r.deadDefenders.size(), "the log has one entry per army that fell");
  eq(target->ownerIndex, side->index, "the city changed hands");
  eq((int)game::sideCities(g, *side).size(), before + 1, "the side owns one more city");
  eq(target->producing, NONE, "a captured city is not building anything");
  for (Army* a : r.attackers) eq(a->x, target->x, "the survivors moved in");
  for (Army* dead : r.deadDefenders) ok(!g.alive(dead), "a dead defender is removed");
  Side* victim = g.sides.back();
  victim->gold = 400;
  eq(game::loot(g, victim), 200, "one city: half its gold");
  City* extra = nullptr;
  for (auto& c : g.map->cities) if (!extra && c.ownerIndex == NONE && &c != target) extra = &c;
  if (extra) {
    game::setCityOwner(g, *extra, victim->index);
    eq(game::loot(g, victim), (400 / 2) / 2, "two cities: half the per-city share");
  }
}

static void testTutorialHero() {
  printf("tutorial: a hero cannot die attacking neutrals\n");
  auto gp = newGame("TUTORIA", seed(31));
  Game& g = *gp;
  Side* side = game::begin(g);
  eq(g.map->options.tutorial, 1, "TUTORIA sets the tutorial flag");
  ok(!side->computer, "the tutorial player is human");
  Army h;
  h.type = armytype::HERO; h.strength = 1; h.owner = side->index;
  std::vector<Army> D(8);
  std::vector<Army*> defenders;
  for (auto& d : D) { d.type = 11; d.strength = 9; d.owner = NONE; defenders.push_back(&d); }
  int deaths = 0;
  for (int i = 0; i < 50; i++) deaths += (int)combat::resolve(g, {&h}, defenders, 0, 0).deadAttackers.size();
  eq(deaths, 0, "the tutorial hero never dies against neutrals");
  g.map->options.tutorial = 0;
  deaths = 0;
  for (int i = 0; i < 50; i++) deaths += (int)combat::resolve(g, {&h}, defenders, 0, 0).deadAttackers.size();
  ok(deaths > 0, "without the tutorial flag the hero can die");
}

static void testCityChoices(const std::string& scenario) {
  printf("pillage, sack and raze: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(29));
  Game& g = *gp;
  Side* side = game::begin(g);
  auto captured = [&](size_t want) -> City* {
    for (auto& c : g.map->cities) {
      if (c.ownerIndex == NONE && c.slots.size() >= want) {
        game::setCityOwner(g, c, side->index);
        return &c;
      }
    }
    return nullptr;
  };
  City* city = captured(2);
  ok(city != nullptr, "found a city with something to pillage");
  if (city) {
    size_t n = city->slots.size();
    Slot dear = city->slots.back();
    int want = std::abs(g.types.byId(dear.type)->price) / 2;
    int gold0 = side->gold, score0 = side->diploScore;
    auto got = game::pillage(g, *side, *city, {});
    eq(got.gold, want, "pillage pays half the type's purchase price");
    eq(side->gold, gold0 + want, "the gold was paid");
    eq(city->slots.size(), n - 1, "pillage removed one type");
    eq(city->defence, rules::cityDefence((int)city->slots.size()), "defence was recomputed");
    int d = side->diploScore - score0;
    ok(d >= 1 && d <= 5, "pillage costs 1d5 diplomatic score: " + std::to_string(d));
  }
  City* city2 = captured(3);
  if (city2) {
    Slot cheapest = city2->slots[0];
    int want = 0;
    for (size_t i = 1; i < city2->slots.size(); i++) want += std::abs(g.types.byId(city2->slots[i].type)->price) / 2;
    int gold0 = side->gold, score0 = side->diploScore;
    size_t n2 = city2->slots.size();
    auto got = game::sack(g, *side, *city2, {});
    eq(got.gold, want, "sack pays for every type it strips");
    eq(got.lost.size(), n2 - 1, "sack lists every type it strips");
    int sum = 0;
    for (auto& l : got.lost) sum += l.second;
    eq(sum, want, "and what each was worth adds up to the gold");
    eq(side->gold, gold0 + want, "the gold was paid");
    eq((int)city2->slots.size(), 1, "only the cheapest type is left");
    ok(city2->slots[0] == cheapest, "and it is the cheapest one");
    eq(city2->defence, 1, "one type means defence 1");
    int d = side->diploScore - score0;
    ok(d >= 6 && d <= 15, "sack costs 1d10+5 diplomatic score: " + std::to_string(d));
  }
  City* city3 = captured(1);
  if (city3) {
    int before = (int)game::sideCities(g, *side).size();
    City* other = nullptr;
    for (auto& c : g.map->cities) if (!other && c.ownerIndex == side->index && &c != city3) other = &c;
    if (other) game::vector(g, *other, city3);
    int score0 = side->diploScore;
    game::raze(g, *side, *city3, {});
    eq(city3->ownerIndex, NONE, "a razed city is neutral");
    eq((int)city3->slots.size(), 0, "a razed city produces nothing");
    eq(city3->income, 0, "a razed city earns nothing");
    eq((int)game::sideCities(g, *side).size(), before - 1, "the side no longer owns it");
    if (other) eq(other->vectorTo, NONE, "vectoring to it was cancelled");
    int d = side->diploScore - score0;
    ok(d >= 11 && d <= 25, "raze costs 1d15+10 diplomatic score: " + std::to_string(d));
    int rb = movement::grid(g, side->index)[city3->y * g.map->width + city3->x];
    ok(!movement::has(rb, movement::CITY_F) && rb % 8 != 0, "a razed city's ruins can be walked over: " + std::to_string(rb));
  }
  City empty;
  empty.index = -1;
  eq(game::pillage(g, *side, empty, {}).gold, 0, "pillaging an empty city pays nothing");
  City one;
  one.index = -1;
  one.slots.push_back(Slot{11, "", 0, 0, 0, 0, 0});
  eq(game::sack(g, *side, one, {}).gold, 0, "sacking a one-type city pays nothing");
}

static void testHeroes(const std::string& scenario) {
  printf("heroes: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(37));
  Game& g = *gp;
  Side* side = game::begin(g);
  auto offer = side->heroOffer;
  ok(offer != nullptr, "a hero offers itself on turn 1");
  if (!offer) return;
  eq(offer->price, 0, "the first hero is free");
  eq(offer->city, side->capital, "the first hero appears at the capital");
  const auto& names = hero::names(g, *side);
  eq((int)names.size(), 100, "a side has a hundred candidate heroes");
  ok(!offer->name.empty(), "the offer carries a name");
  bool known = false;
  for (auto& n : names) if (n.first == offer->name) known = true;
  ok(known, "and the name comes from that side's file");
  bool heroine = false;
  for (auto& n : hero::names(g, g.map->sides[0])) if (n.second) heroine = true;
  ok(!heroine, "side 0 has no heroines to offer");
  auto [h, allies] = hero::recruit(g, *side, *offer);
  eq(h->type, armytype::HERO, "the recruit is a hero");
  eq(h->name, offer->name, "the hero keeps the name it was offered under");
  eq(h->strength, 5, "a new hero has strength 5");
  eq(h->maxMoves, 14, "a new hero has 14 movement");
  eq(h->moves, 14, "a new hero joins with its moves already full");
  eq((int)allies.size(), 0, "the first hero brings no allies");
  eq((int)h->items.size(), 1, "the first hero carries one item");
  eq(h->items[0]->type, rules::ITEM_STANDARD, "and it is the side's standard");
  h->experience = 15;
  auto promoted = hero::checkPromotions(g, *side);
  eq((int)promoted.size(), 1, "15 experience promotes a Hero");
  eq(h->level, 2, "the hero is now level 2");
  eq(h->title, "Cavalier", "level 2 is a Cavalier");
  eq(h->strength, 6, "promotion adds a strength");
  eq(h->maxMoves, 16, "promotion adds 2 movement");
  eq((int)hero::checkPromotions(g, *side).size(), 0, "no second promotion on the same experience");
  h->experience = 60;
  hero::checkPromotions(g, *side);
  eq(h->level, 3, "60 experience promotes one step only");
  hero::checkPromotions(g, *side);
  eq(h->title, "Paladin", "the next check reaches Paladin");
  eq((int)hero::checkPromotions(g, *side).size(), 0, "a Paladin is not promoted again");
  hero::addExperience(g, h, 99);
  eq(h->experience, rules::MAX_HERO_XP, "experience is capped at 60");
  auto [type, n] = hero::allies(g);
  ok(type != nullptr, "the allies have a type");
  ok(n >= 1 && n <= 3, "1 to 3 allies arrive");
  g.turn = 4;
  HeroOffer later;
  later.city = side->capital;
  auto escort = hero::recruit(g, *side, later).second;
  ok(!escort.empty(), "a later hero arrives with an escort");
  for (Army* a : escort) {
    eq(a->moves, a->maxMoves, "an ally joins with its moves already full");
    eq(a->upkeep, 0, "an ally costs no upkeep");
  }
  g.turn = 1;
  eq(hero::MAX_PER_SIDE, 5, "a side may hold 5 heroes");
  eq(hero::MAX_IN_GAME, 40, "the game holds 40 heroes");
  bool blocked = true;
  for (int i = 0; i < 5; i++) addArmy(g, 0, 0, side->index, armytype::HERO, 5);
  g.turn = 2;
  for (int i = 0; i < 50; i++) if (hero::offer(g, *side)) blocked = false;
  ok(blocked, "no hero is offered once the side is at its limit");
  Item fire;
  fire.name = "Firesword"; fire.type = rules::ITEM_BATTLE; fire.value = 1;
  Army carrier;
  carrier.type = armytype::HERO; carrier.owner = side->index; carrier.items = {&fire};
  int dx = NONE, dy = NONE;
  for (int y = 0; y < g.map->height && dx == NONE; y++)
    for (int x = 0; x < g.map->width; x++) if (dx == NONE && scn::terrainAt(*g.map, x, y) == movement::PLAIN) { dx = x; dy = y; }
  auto dropped = hero::dropItems(g, &carrier, dx, dy);
  eq((int)dropped.size(), 1, "the item was dropped");
  if (!dropped.empty()) {
    eq(dropped[0]->status, 1, "a dropped item lies on the ground");
    eq(dropped[0]->x, dx, "it lies where the hero fell");
  }
  eq((int)carrier.items.size(), 0, "the dead hero carries nothing");
  int wx = NONE, wy = NONE;
  for (int y = 0; y < g.map->height && wx == NONE; y++)
    for (int x = 0; x < g.map->width; x++) if (wx == NONE && scn::terrainAt(*g.map, x, y) == movement::WATER) { wx = x; wy = y; }
  if (wx != NONE) {
    Item ice;
    ice.name = "Icesword"; ice.type = rules::ITEM_BATTLE; ice.value = 1; ice.status = 3;
    Army drowner;
    drowner.type = armytype::HERO; drowner.owner = side->index; drowner.items = {&ice};
    eq((int)hero::dropItems(g, &drowner, wx, wy).size(), 0, "nothing is dropped in water");
    eq(ice.status, 0, "an item lost at sea is gone for good");
  }
}

static void testHeroExperienceBug() {
  printf("hero experience: the original's bug\n");
  auto gp = newGame("ERYTHEA", seed(41));
  Game& g = *gp;
  auto run = [&]() {
    Army defHero, att;
    defHero.type = armytype::HERO; defHero.strength = 5; defHero.owner = 1;
    att.type = 11; att.strength = 3; att.owner = 0;
    Battle b;
    hero::battleExperience(g, {&att}, {&defHero}, b, false);
    return defHero.experience;
  };
  rules::bugs.heroExperienceReadsAttackerTypes = true;
  eq(run(), 0, "with the bug on, a defending hero facing a non-hero gains nothing");
  rules::bugs.heroExperienceReadsAttackerTypes = false;
  eq(run(), 1, "with the bug off, the defender is credited");
  rules::bugs.heroExperienceReadsAttackerTypes = true;
  Army h;
  h.type = armytype::HERO; h.strength = 5; h.owner = 0;
  Battle none;
  hero::battleExperience(g, {&h}, {}, none, false);
  eq(h.experience, 1, "an attacking hero gains 1 in the open");
  hero::battleExperience(g, {&h}, {}, none, true);
  eq(h.experience, 3, "attacking a city is worth 2");
  Army dead;
  dead.type = armytype::HERO; dead.strength = 5; dead.owner = 0;
  Battle died;
  died.deadByArmy.insert(&dead);
  hero::battleExperience(g, {&dead}, {}, died, true);
  eq(dead.experience, 0, "a hero that died gains nothing");
}

static void testAIGame(const std::string& scenario, int turns) {
  printf("AI game: %s, %d turns\n", scenario.c_str(), turns);
  auto gp = newGame(scenario, seed(101));
  Game& g = *gp;
  Side* side = game::begin(g);
  std::map<int, int> ownedAtStart;
  for (Side* s : g.sides) ownedAtStart[s->index] = (int)game::sideCities(g, *s).size();
  while (side && g.turn <= turns) {
    ai::playTurn(g, *side);
    side = game::endTurn(g);
  }
  ok(g.turn > turns || side == nullptr, "the game ran to the turn limit");
  std::map<int, int> perTile;
  for (Army* a : g.armies) {
    ok(g.types.byId(a->type) != nullptr, "every army has a real type");
    if (!a->transit) {
      ok(a->x != NONE && a->y != NONE, "a placed army has a position");
      ok(a->x >= 0 && a->x < g.map->width && a->y >= 0 && a->y < g.map->height, "every army is on the map");
      perTile[a->y * g.map->width + a->x]++;
    }
    ok(a->moves >= 0, "movement never goes negative");
    ok(a->moves <= rules::MAX_MOVE, "movement never exceeds the cap");
  }
  int worst = 0;
  for (auto& [k, v] : perTile) worst = std::max(worst, v);
  ok(worst <= rules::MAX_STACK, "no tile ever holds more than 8 armies: " + std::to_string(worst));
  for (Side* s : g.sides) {
    ok(s->gold >= 0, s->name + " never goes into debt");
    if (!s->alive) ok(game::sideCities(g, *s).empty(), "a dead side owns nothing");
  }
  for (auto& c : g.map->cities) {
    if (c.ownerIndex != NONE) ok(g.map->side(c.ownerIndex) != nullptr, "a city's owner is a real side");
    ok(c.slots.size() <= 4, "a city never gains production slots");
  }
  bool moved = false;
  int captures = 0;
  for (Side* s : g.sides) {
    int now = (int)game::sideCities(g, *s).size();
    if (now != ownedAtStart[s->index]) moved = true;
    captures += std::max(0, now - ownedAtStart[s->index]);
  }
  ok(moved, "cities changed hands over " + std::to_string(turns) + " turns");
  ok(captures > 0, fmt("the computer players took %d cities", captures));
  ok(g.armies.size() > g.map->cities.size(),
     fmt("armies were built: %d from %d cities", (int)g.armies.size(), (int)g.map->cities.size()));
}

static void testSlots() {
  printf("army slots\n");
  // a stand-in game: the fight-order table and the army list are all the
  // model wants
  struct Stand {
    Game g;
    std::vector<Army*> a;
  };
  auto stack = [](int n) {
    auto s = std::make_unique<Stand>();
    s->g.map = std::make_unique<Map>();
    s->g.map->fightOrder.assign(9, std::vector<int>(29, 0));
    for (int i = 1; i <= n; i++) {
      Army a;
      a.type = i;
      a.moves = 10 + i;
      a.group = 0;
      s->g.map->fightOrder[0][i] = n - i;          // the first army ranks highest
      s->a.push_back(s->g.add(a));
    }
    return s;
  };
  {
    auto st = stack(3);
    auto& a = st->a;
    auto s = slots::build(st->g, a, 0);
    eq(s.n, 3, "three armies fill three slots");
    eq(s.army[0], a[0], "the highest in the fight order takes the first slot");
    ok(s.inGroup[0] && !s.inGroup[1] && !s.inGroup[2], "clicking a tile selects one army, not the stack");
    eq(s.group[0], 0, "ungrouped armies are each their own group");
    eq(s.group[2], 2, "numbered off down the bar");
    eq(s.mark[0], slots::TICK, "the one that moves is ticked");
    eq(s.mark[1], slots::CROSS, "the others are crossed");
    eq(slots::moves(s), 11, "and Group Move is that army's own");
    eq(slots::grouped(s), false, "so the Grp button is not green yet");
    eq((int)slots::selected(s).size(), 1, "one army moves");
  }
  {
    auto st = stack(3);
    auto s = slots::build(st->g, st->a, 0);
    slots::toggle(s, st->g, 1);
    ok(s.inGroup[1] && s.group[1] == s.group[0], "a click adds an army to the group");
    eq(s.mark[1], slots::NOMARK, "only the head of a group is marked");
    eq(slots::moves(s), 11, "the group moves at its slowest army's pace");
    slots::toggle(s, st->g, 1);
    ok(!s.inGroup[1], "clicking it again drops it out");
    ok(s.group[1] != s.group[0], "into a group of its own");
    slots::toggle(s, st->g, 0);
    ok(s.inGroup[0], "the last army of a group cannot be dropped");
  }
  {
    auto st = stack(3);
    auto s = slots::build(st->g, st->a, 0);
    slots::all(s, st->g);
    eq((int)slots::selected(s).size(), 3, "Grp takes the whole stack");
    eq(slots::grouped(s), true, "and turns the button green");
    eq(s.mark[1], slots::NOMARK, "one mark for the one group");
    slots::single(s, st->g);
    eq((int)slots::selected(s).size(), 1, "and Grp again breaks it up");
    eq(slots::grouped(s), false, "leaving the button red");
    slots::pickGroup(s, st->g, 2);
    ok(s.inGroup[2] && !s.inGroup[0], "a mark picks out that group alone");
    eq(s.mark[2], slots::TICK, "which is then the ticked one");
  }
  {
    auto st = stack(3);
    auto s = slots::build(st->g, st->a, 0);
    slots::all(s, st->g);
    s.inGroup[2] = false;
    slots::pickGroup(s, st->g, 0);
    ok(s.inGroup[2], "picking a group takes all of it");
  }
  {
    auto st = stack(3);
    auto& a = st->a;
    Game& g = st->g;
    auto s = slots::build(g, a, 0);
    slots::all(s, g);
    slots::commit(s, g);
    int id = a[0]->group;
    ok(id != slots::UNGROUPED && id == a[1]->group && id == a[2]->group, "a group of more than one is written back into its armies");
    auto again = slots::build(g, a, 0);
    eq((int)slots::selected(again).size(), 3, "so picking it up again takes the whole group");
    eq(slots::grouped(again), true, "with the Grp button still green");
    eq(again.group[0], again.group[2], "and all three in one group in the bar");
    slots::commit(again, g);
    eq(a[0]->group, id, "picking it up does not renumber it");
    slots::toggle(again, g, 2);
    slots::commit(again, g);
    auto third = slots::build(g, a, 0);
    eq((int)slots::selected(third).size(), 2, "dropping one out leaves the rest grouped");
    eq(a[2]->group, slots::UNGROUPED, "and the one dropped out is on its own");
    eq(a[0]->group, id, "while the group keeps its id");
    slots::single(third, g);
    slots::commit(third, g);
    eq(a[0]->group, slots::UNGROUPED, "and breaking it up clears the grouping");
    eq((int)slots::selected(slots::build(g, a, 0)).size(), 1, "so a click picks up one army again");
  }
  {
    auto st = stack(3);
    auto& a = st->a;
    auto s = slots::build(st->g, a, 0);
    slots::all(s, st->g);
    auto left = slots::keep(s, st->g, {a[0], a[2]});
    ok(left.has_value(), "the survivors are still a selection");
    if (left) {
      eq(left->n, 2, "the dead leave the bar");
      ok(left->inGroup[0] && left->inGroup[1], "the survivors keep moving together");
    }
    ok(!slots::keep(s, st->g, {}).has_value(), "and a wiped-out stack is no selection at all");
  }
}

static void testScreenLayout() {
  printf("screen layout\n");
  auto ui = uidata::load(DATA);
  eq(ui.bitmaps[0], "startup.pck", "bitmap 0 is the startup screen");
  eq(ui.bitmaps[4], "button.pck", "bitmap 4 is the button sheet");
  eq(ui.bitmaps[78], "movebar7.pck", "there are 79 bitmaps, 0 to 78");
  eq((int)ui.joins.size(), 36, "JOIN.DAT names 36 dialogs");
  eq(ui.joins[0].button, 0, "dialog 0 uses button group 0");
  eq(ui.joins[0].area, 2, "dialog 0 uses area screen 2");
  eq(ui.joins[9].button, 11, "dialog 9 takes button group 11");
  eq(ui.joins[12].button, 9, "and dialog 12 takes group 9");
  auto d = uidata::dialog(ui, uidata::MAIN_SCREEN);
  eq((int)d.controls.size(), 43, "the main screen has 43 controls");
  eq((int)d.regions.size(), 6, "and 6 regions");
  std::map<int, std::array<int, 4>> want = {
      {1, {400, 30, 224, 312}}, {2, {16, 30, 360, 360}}, {3, {0, 0, 640, 18}}, {9, {16, 408, 360, 56}}};
  for (auto& r : d.regions) {
    auto w = want.find(r.id);
    if (w != want.end()) {
      eq(r.x, w->second[0], fmt("region %d x", r.id));
      eq(r.y, w->second[1], fmt("region %d y", r.id));
      eq(r.w, w->second[2], fmt("region %d width", r.id));
      eq(r.h, w->second[3], fmt("region %d height", r.id));
    }
  }
  eq(ui.shortcutNames[507], "Move All", "UDB.DAT names menu item 507");
  eq(ui.shortcutNames[535], "End Turn", "and item 535");
  const char* wantSc[4] = {"Search", "Move All", "Heroes", "End Turn"};
  for (int i = 0; i < 4; i++) {
    eq(ui.shortcutNames[ui.shortcuts[i]], wantSc[i], fmt("button %d (control %d) is %s", i, 179 + i, wantSc[i]));
  }
  {
    auto img = pck::decode(DATA + "/PICS/MENUBUTT.PCK");
    int w = img->w, h = img->h;
    eq(w, 320, "MENUBUTT.PCK is 320 wide");
    eq(h, 197, "and 197 tall: 7 rows 28 apart, plus the last row's border");
    std::set<std::string> cells;
    int n = 0;
    for (auto& [id, item] : ui.shortcutItems) {
      eq(item.w, 32, fmt("item %d icon width", id));
      eq(item.h, 29, fmt("item %d icon height", id));
      for (int st = 0; st <= 2; st++) {
        auto src = item.src[st];
        eq(src.x % 32, 0, fmt("item %d state %d sits on a column", id, st));
        eq(src.y % 28, 0, fmt("item %d state %d sits on a row", id, st));
        ok(src.x + 32 <= w && src.y + 29 <= h, fmt("item %d state %d is inside the sheet", id, st));
        std::string key = fmt("%d,%d", src.x, src.y);
        ok(!cells.count(key), "cell " + key + " is used once");
        cells.insert(key);
        n++;
      }
    }
    eq(n, 63, "21 items, three cells each");
    auto& endTurn = ui.shortcutItems[535];
    eq(endTurn.src[uidata::NORMAL].x, 0, "End Turn rests at the sheet's origin");
    eq(endTurn.src[uidata::NORMAL].y, 0, "at the top of it");
    eq(endTurn.src[uidata::ACTIVE].x, 32, "and lights up one cell right");
  }
  {
    auto img = pck::decode(DATA + "/PICS/ABITS.PCK");
    int w = img->w, h = img->h;
    const auto& px = img->px;
    eq(w, 480, "ABITS.PCK is 480 wide");
    eq(h, 40, "and 40 tall");
    const int BG = 3;
    std::set<int> seen;
    for (int cell = 0; cell <= 8; cell++) {
      std::map<int, int> count;
      for (int y = 0; y <= 29; y++) {
        for (int x = cell * 32; x <= cell * 32 + 31; x++) {
          int v = px[y * w + x];
          if (v != BG) count[v]++;
        }
      }
      int best = NONE, bestN = 0;
      for (auto& [v, c] : count) if (c > bestN) { best = v; bestN = c; }
      ok(bestN > 40, fmt("ring %d has ink", cell));
      if (cell > 0) {
        ok(!seen.count(best), fmt("ring %d has its own colour", cell));
        seen.insert(best);
      }
    }
    int ringRow = 0, stripRow = 0;
    for (int x = 0; x <= 287; x++) {
      if (px[15 * w + x] != BG) ringRow++;
      if (px[38 * w + x] != BG) stripRow++;
    }
    ok(stripRow > ringRow * 2, "row 38 is far denser than row 15, so it is not part of the rings");
    struct Icon { int sx, sy; const char* name; };
    for (Icon ic : {Icon{344, 0, "cities"}, Icon{344, 20, "treasury"}, Icon{384, 0, "income"}, Icon{384, 20, "upkeep"}}) {
      ok(ic.sx >= 288, fmt("the %s icon is past the nine rings", ic.name));
      ok(ic.sx + 40 <= w && ic.sy + 20 <= h, fmt("the %s icon is inside the sheet", ic.name));
      int ink = 0;
      for (int y = ic.sy; y <= ic.sy + 19; y++)
        for (int x = ic.sx; x <= ic.sx + 39; x++) if (px[y * w + x] != BG) ink++;
      ok(ink > 40, fmt("the %s icon has ink", ic.name));
      int edge = 0;
      for (int y = ic.sy; y <= ic.sy + 19; y++) if (px[y * w + ic.sx - 1] != BG) edge++;
      eq(edge, 0, fmt("the %s icon starts on a clear column", ic.name));
    }
  }
  std::map<std::string, std::pair<int, int>> sizes;
  std::set<std::string> missing;
  int checked = 0;
  for (auto& [gid, group] : ui.buttons) {
    for (auto& c : group.controls) {
      if (c.bitmap != 0 && c.w > 0) {
        std::string name = ui.bitmaps[c.bitmap];
        if (!sizes.count(name) && !missing.count(name)) {
          bool found = false;
          for (const char* dir : {"/PICS/", "/TERRAIN0/", "/"}) {
            std::string p = DATA + dir + upper(name);
            if (fileExists(p)) {
              auto im = pck::decode(p);
              sizes[name] = {im->w, im->h};
              found = true;
              break;
            }
          }
          if (!found) missing.insert(name);
        }
        ok(sizes.count(name), "bitmap resolves to a file: " + name);
        if (sizes.count(name)) {
          auto [sw, sh] = sizes[name];
          for (int st = 0; st <= 2; st++) {
            ok(c.src[st].x + c.w <= sw && c.src[st].y + c.h <= sh, fmt("control %d state %d lies inside %s", c.id, st, name.c_str()));
            checked++;
          }
        }
      }
    }
  }
  ok(checked >= 400, fmt("checked %d source rects", checked));
}

static void testCityCastles() {
  printf("city castles\n");
  auto gp = newGame("TUTORIA", seed(11));
  Game& g = *gp;
  Map& map = *g.map;
  std::map<int, int> WANT = {{-1, 96}, {0, 98}, {1, 100}, {2, 102}, {3, 104}, {4, 106}, {5, 108}, {6, 128}, {7, 130}};
  City& city = map.cities[0];
  for (auto [owner, base] : WANT) {
    city.ownerIndex = owner >= 0 ? owner : NONE;
    city.razed = false;
    ok(scn::cityTileBase(city) == base, fmt("owner %d takes castle block %d", owner, base));
    scn::setCityTiles(map, city);
    ok(scn::tileAt(map, city.x, city.y) == base, "top-left cell is the block");
    ok(scn::tileAt(map, city.x + 1, city.y) == base + 1, "top-right is +1");
    ok(scn::tileAt(map, city.x, city.y + 1) == base + 16, "bottom-left is +16");
    ok(scn::tileAt(map, city.x + 1, city.y + 1) == base + 17, "bottom-right is +17");
    for (int dx = 0; dx <= 1; dx++)
      for (int dy = 0; dy <= 1; dy++)
        ok(scn::terrainAt(map, city.x + dx, city.y + dy) == 10, fmt("castle cell %d,%d is city terrain", dx, dy));
  }
  for (int owner = 0; owner <= 7; owner++) {
    city.ownerIndex = NONE;
    city.razed = true;
    city.razedBy = owner;
    ok(scn::cityTileBase(city) == 0xa0 + 2 * owner, fmt("ruins of side %d take block %d", owner, 0xa0 + 2 * owner));
    scn::setCityTiles(map, city);
    for (int dx = 0; dx <= 1; dx++)
      for (int dy = 0; dy <= 1; dy++)
        ok(scn::terrainAt(map, city.x + dx, city.y + dy) == 11, fmt("ruin cell %d,%d is ruins terrain", dx, dy));
  }
  auto g2p = newGame("TUTORIA", seed(12));
  Game& g2 = *g2p;
  City& c2 = g2.map->cities[0];
  for (Side* s : g2.sides) {
    game::setCityOwner(g2, c2, s->index);
    scn::setCityTiles(*g2.map, c2);
  }
  auto mineList = game::sideCities(g2, *g2.sides[0]);
  City* mine = mineList.empty() ? nullptr : mineList[0];
  ok(mine != nullptr, "the first side holds a city to raze");
  if (mine) {
    int was = mine->ownerIndex;
    game::raze(g2, *g2.sides[0], *mine, {});
    ok(mine->razedBy == was, "raze remembers who held the city");
    ok(scn::tileAt(*g2.map, mine->x, mine->y) == 0xa0 + 2 * was, "raze restamps the map with that side's ruins");
  }
}

static void testSites(const std::string& scenario) {
  printf("sites: %s\n", scenario.c_str());
  auto gp = newGame(scenario, seed(53));
  Game& g = *gp;
  int rich = 0;
  for (auto& s : g.map->sites) {
    ok(s.content != site::EMPTY, "no site is left empty");
    if (s.rich) {
      rich++;
      ok(s.type != site::TEMPLE, "a temple is never rich");
    }
    if (s.content == site::ALLIES) {
      ok(s.allyType != NONE, "an ally ruin knows what joins");
      ok(g.types.byId(s.allyType) != nullptr, "and it is a real army type");
    }
    if (s.content != site::TEMPLE) ok(s.guardian >= 1 && s.guardian <= 9, "a ruin has a guardian");
  }
  eq(rich, (int)g.map->sites.size() * 3 / 10, "three in ten sites are rich");
  std::set<int> placed;
  for (auto& s : g.map->sites) {
    if (s.content == site::ITEM) {
      ok(!placed.count(s.item), "an item is hidden in only one ruin");
      placed.insert(s.item);
    }
  }
  for (auto& it : g.map->items) if (placed.count(it.index)) eq(it.status, 2, "a hidden item is marked hidden");
  std::map<int, Item*> byIndex;
  for (auto& it : g.map->items) byIndex[it.index] = &it;
  for (auto& s : g.map->sites) {
    if (s.content == site::ITEM) eq(site::itemReserved(*byIndex[s.item]), s.rich, "a reserved item needs a rich ruin");
  }
  if (g.map->itemPool) {
    std::set<std::string> names;
    for (auto& p : *g.map->itemPool) names.insert(p.name);
    for (int i = 8; i <= 21; i++) if (byIndex.count(i)) ok(names.count(byIndex[i]->name), "item " + std::to_string(i) + " came from the pool");
    for (int i = 0; i <= 7; i++) if (byIndex.count(i)) eq(byIndex[i]->type, rules::ITEM_STANDARD, "records 0-7 are standards");
  }
  Site* ruin = nullptr;
  for (auto& s : g.map->sites) if (!ruin && s.content == site::GOLD) ruin = &s;
  if (ruin) {
    Side* side = g.sides[0];
    Army* grunt = addArmy(g, ruin->x, ruin->y, side->index, 11, 3, 10, 10);
    auto r = site::search(g, {grunt}, ruin->x, ruin->y);
    eq(r->kind, "no hero", "a stack without a hero cannot search a ruin");
    ok(!ruin->searched, "and the ruin is not used up");
    Army* h = addArmy(g, ruin->x, ruin->y, side->index, armytype::HERO, 9, 14, 14);
    int gold0 = side->gold;
    r = site::search(g, {h, grunt}, ruin->x, ruin->y);
    ok(r->kind == "gold" || r->kind == "killed", "a hero gets a result: " + r->kind);
    if (r->kind == "gold") {
      ok(r->gold >= 503 && r->gold <= 4000, "the gold is in range: " + std::to_string(r->gold));
      eq(side->gold, gold0 + r->gold, "the gold was paid");
      eq(h->experience, 3, "searching is worth 3 experience");
    }
    ok(ruin->searched, "a searched ruin is used up");
    ok(site::search(g, {h}, ruin->x, ruin->y) == nullptr, "and cannot be searched again");
  }
  Site* temple = nullptr;
  for (auto& s : g.map->sites) if (!temple && s.content == site::TEMPLE && s.templeIndex == 0) temple = &s;
  if (temple) {
    Side* side = g.sides[0];
    Army a;
    a.x = temple->x; a.y = temple->y; a.owner = side->index; a.type = 11; a.strength = 3;
    auto r = site::search(g, {&a}, temple->x, temple->y);
    eq(r->kind, "temple", "a temple blesses");
    eq(r->blessed, 1, "one army was blessed");
    eq(a.strength, 4, "the blessing adds a strength");
    eq(site::search(g, {&a}, temple->x, temple->y)->blessed, 0, "the same temple does not bless twice");
    a.strength = 9;
    a.blessings.clear();
    site::search(g, {&a}, temple->x, temple->y);
    eq(a.strength, 9, "a blessing never passes 9");
  }
  Army strong, weak;
  strong.strength = 9;
  weak.strength = 1;
  int survived = 0, died = 0;
  for (int i = 0; i < 400; i++) {
    if (site::survivesGuardian(g, strong, {&strong}, 3)) survived++;
    if (!site::survivesGuardian(g, weak, {&weak}, 8)) died++;
  }
  eq(survived, 400, "a strong hero always beats a weak guardian");
  ok(died > 0, "a weak hero sometimes dies to a strong one");
}

static void testDiplomacy() {
  printf("diplomacy\n");
  namespace d = diplomacy;
  auto off = newGame("ERYTHEA", seed(61));
  eq(off->map->options.diplomacy, 0, "the shipped scenarios have diplomacy off");
  eq(d::state(*off, 0, 1), d::WAR, "with the option off every pair starts at war");
  ok(d::mayAttack(*off, 0, 1), "and may attack freely");
  auto gp = newGame("ERYTHEA", seedWith(61, {{"diplomacy", 1}}));
  Game& g = *gp;
  eq(d::state(g, 0, 1), d::PEACE, "with the option on every pair starts at peace");
  ok(!d::mayAttack(g, 0, 1), "a side at peace may not attack");
  ok(d::mayAttack(g, 0, NONE), "neutrals may always be attacked");
  eq(d::state(g, 0, 0), d::PEACE, "a side is at peace with itself");
  d::propose(g, 0, 1, d::WAR);
  auto msgs = d::apply(g, *g.sides[0]);
  eq(d::state(g, 0, 1), d::WAR, "declaring war takes effect at once");
  eq(d::state(g, 1, 0), d::WAR, "and binds the other side too");
  ok(!msgs.empty() && contains(msgs[0], "War declared"), "and is announced");
  eq(d::proposal(g, 1, 0), d::WAR, "the other side's proposal is raised to match");
  auto g2p = newGame("ERYTHEA", seedWith(62, {{"diplomacy", 1}}));
  Game& g2 = *g2p;
  d::propose(g2, 0, 1, d::WAR);
  d::apply(g2, *g2.sides[0]);
  g2.diplomacy.proposal[1 * 8 + 0] = NONE;                // forget their matching proposal
  d::propose(g2, 0, 1, d::PEACE);
  d::apply(g2, *g2.sides[0]);
  eq(d::state(g2, 0, 1), d::WAR, "one-sided peace does not land");
  d::propose(g2, 0, 1, d::PEACE);
  d::propose(g2, 1, 0, d::PEACE);
  auto m2 = d::apply(g2, *g2.sides[0]);
  eq(d::state(g2, 0, 1), d::PEACE, "matched proposals make peace");
  ok(!m2.empty() && contains(m2[0], "Peace negotiated"), "and it is announced");
  {
    auto g3p = newGame("ERYTHEA", seedWith(63, {{"diplomacy", 1}}));
    Game& g3 = *g3p;
    d::propose(g3, 0, 1, d::WAR);
    d::apply(g3, *g3.sides[0]);
    d::propose(g3, 0, 1, d::PEACE);
    int before = g3.sides[0]->diploScore;
    d::scoreUpdate(g3, *g3.sides[0]);
    int gain = g3.sides[0]->diploScore - before;
    ok(gain >= 11 && gain <= 20, "peace offered from war is worth 1d10+10: " + std::to_string(gain));
    d::propose(g3, 1, 0, d::PEACE);
    before = g3.sides[0]->diploScore;
    d::scoreUpdate(g3, *g3.sides[0]);
    eq(g3.sides[0]->diploScore, before, "not once the other side offers it too");
  }
  auto g3p = newGame("ERYTHEA", seed(63));
  Game& g3 = *g3p;
  for (size_t i = 0; i < g3.sides.size(); i++) g3.sides[i]->diploScore = (int)(i + 1) * 10;
  auto r = d::ratings(g3);
  eq(r[g3.sides[0]->index], "Statesman", "the lowest score is the Statesman");
  eq(r[g3.sides.back()->index], "Running Dog", "the highest is the Running Dog");
  const auto& ranks = d::ratingRanks(2);
  eq((int)ranks.size(), 2, "two sides take two titles");
  eq(std::string(d::TITLES[ranks[0] - 1]), "Statesman", "best of two");
  eq(std::string(d::TITLES[ranks[1] - 1]), "Running Dog", "worst of two");
  auto g4p = newGame("ERYTHEA", seedWith(64, {{"diplomacy", 1}}));
  Game& g4 = *g4p;
  Side* side = game::begin(g4);
  Side* victim = nullptr;
  for (Side* s : g4.sides) if (s->index != side->index) victim = s;
  City* city = victim->capital;
  Army* army = addArmy(g4, city->x, city->y - 1, side->index, 11, 3, 20, 20);
  army->upkeep = 1;
  auto path = movement::findPath(g4, {army}, army->x, army->y, city->x, city->y);
  if (path && !path->empty()) {
    auto r2 = movement::walk(g4, {army}, *path);
    eq(r2.stopped, "at peace", "a stack at peace cannot walk into their city");
    eq(army->x, city->x, "and has not moved into it");
  }

  // but a stack of a side at peace in the open is passed over: the walk may
  // not stop on it, and goes on beyond it (1a8b:07f9)
  Side* other = victim;
  int W = g4.map->width;
  auto open = [&](int x, int y) {
    int t = scn::terrainAt(*g4.map, x, y);
    return t != movement::WATER && t != movement::SHORE && t != movement::CITY && t != movement::BRIDGE &&
           !g4.map->cityTile[y * W + x] && game::armiesAt(g4, x, y).empty();
  };
  int sx = NONE, sy = NONE;
  for (int y = 10; y <= g4.map->height - 10; y++)
    for (int x = 10; x <= W - 10; x++)
      if (sx == NONE && open(x, y) && open(x + 1, y) && open(x + 2, y)) { sx = x; sy = y; }
  Army* walker = addArmy(g4, sx, sy, side->index, 11, 3, 20, 20);
  Army* blocker = addArmy(g4, sx + 1, sy, other->index, 11, 3, 20, 20);
  walker->upkeep = blocker->upkeep = 1;
  movement::Path steps = {{sx + 1, sy, 2}, {sx + 2, sy, 2}};
  auto r3 = movement::walk(g4, {walker}, steps);
  eq(walker->x, sx + 2, "a stack at peace in the open is passed over");
  eq(walker->moves, 16, "paying for the step past it");
  eq(r3.stopped, "arrived", "and the walk arrives");
  walker->x = sx;
  walker->moves = 20;
  g4.diplomacy.state[side->index * 8 + other->index] = d::WAR;
  auto r4 = movement::walk(g4, {walker}, steps);
  eq(r4.stopped, "attack", "a stack at war is attacked");
  eq(walker->x, sx, "from where the walker stands");

  // ruins are open ground: a fight there is for no city, and takes none
  City* ruin = nullptr;
  for (auto& c : g4.map->cities)
    if (!ruin && c.ownerIndex != NONE && c.ownerIndex != side->index) ruin = &c;
  game::raze(g4, g4.map->sides[ruin->ownerIndex], *ruin, {}, true);
  std::vector<Army*> onRuin;
  for (Army* a : g4.armies)
    if (a->x != NONE && a->x >= ruin->x && a->x <= ruin->x + 1 && a->y >= ruin->y && a->y <= ruin->y + 1)
      onRuin.push_back(a);
  for (Army* a : onRuin) g4.remove(a);
  g4.diplomacy.state[side->index * 8 + other->index] = d::WAR;
  addArmy(g4, ruin->x, ruin->y, other->index, 11, 1, 20, 20);
  addArmy(g4, ruin->x + 1, ruin->y + 1, other->index, 11, 1, 20, 20);
  std::vector<Army*> strike;
  for (int i = 0; i < 8; i++) strike.push_back(addArmy(g4, ruin->x - 1, ruin->y, side->index, 11, 9, 20, 20));
  auto fight = game::resolveAttack(g4, strike, ruin->x, ruin->y);
  eq(fight.lines.city, (City*)nullptr, "a fight on ruins is not for a city");
  eq((int)fight.lines.defenders.size(), 1, "and only the tile fought for defends");
  eq(fight.captured, (City*)nullptr, "nothing is captured");
  eq(ruin->ownerIndex, NONE, "the ruins belong to nobody");
}

static std::shared_ptr<Quest> cityQuest(int type, Army* h, City* target) {
  auto q = std::make_shared<Quest>();
  q->type = type;
  q->hero = h;
  q->targetKind = "city";
  q->city = target;
  q->done = 0;
  return q;
}

static void testQuests() {
  printf("quests\n");
  auto off = newGame("ERYTHEA", seed(71));
  Side* sideOff = game::begin(*off);
  Army heroOff;
  heroOff.x = 10; heroOff.y = 10; heroOff.owner = sideOff->index; heroOff.type = armytype::HERO; heroOff.strength = 5;
  ok(quest::assign(*off, *sideOff, &heroOff) == nullptr, "no quests when the option is off");
  auto gp = newGame("ERYTHEA", seedWith(71, {{"quests", 1}}));
  Game& g = *gp;
  Side* side = game::begin(g);
  Army* h = addArmy(g, side->capital->x, side->capital->y, side->index, armytype::HERO, 5);
  auto quest1 = quest::assign(g, *side, h);
  ok(quest1 != nullptr, "a quest is assigned");
  if (quest1) {
    ok(quest1->type >= 0 && quest1->type <= 6, "with a known type");
    ok(quest1->hasTarget(), "and a target");
    ok(!quest::describe(*quest1).empty(), "a quest describes itself");
  }
  ok(quest::assign(g, *side, h) == nullptr, "only one quest at a time");
  std::map<int, int> counts;
  for (int t : quest::TYPE_TABLE) counts[t]++;
  eq(counts[0], 1, "type 0 has one slot");
  eq(counts[4], 2, "type 4 has two");
  eq(counts[6], 2, "type 6 has two");
  eq((int)quest::TYPE_TABLE.size(), 10, "the table is rolled with 1d10");
  auto C = [&](int i) { return &g.map->cities[i - 1]; };   // the Lua's cities[i]
  quest::Event ev;
  side->quest = cityQuest(quest::OCCUPY, h, C(2));
  side->gold = 5000;
  ev = quest::Event();
  ev.city = C(2);
  ev.stack = {h};
  auto r = quest::event(g, *side, "occupy", ev);
  ok(r && r->failed.empty(), "occupying the target completes the quest");
  ok(!side->quest, "and the quest is cleared");
  eq(h->experience, quest::EXPERIENCE, "the hero gains 10 experience");
  ok(r && r->reward, "a reward was given");
  bool wasComputer = side->computer;
  side->computer = false;
  side->questNews.reset();
  side->quest = cityQuest(quest::OCCUPY, h, C(2));
  r = quest::event(g, *side, "occupy", ev);
  ok(side->questNews == r, "the human keeps the news to be shown");
  side->computer = true;
  side->questNews.reset();
  side->quest = cityQuest(quest::OCCUPY, h, C(2));
  quest::event(g, *side, "occupy", ev);
  ok(!side->questNews, "the computer's quest ends unannounced");
  side->computer = wasComputer;
  side->questNews.reset();
  h->experience = 0;
  side->quest = cityQuest(quest::OCCUPY, h, C(3));
  ev = quest::Event();
  ev.city = C(3);
  r = quest::event(g, *side, "occupy", ev);
  ok(r && !r->failed.empty(), "occupying without the hero fails the quest");
  if (r) eq(r->why, 0x22, "said as 'Alas! The city was not taken by thy hero!'");
  eq(h->experience, 0, "and pays no experience");
  City* c5 = C(5);
  int owner5 = c5->ownerIndex;
  bool razed5 = c5->razed;
  side->quest = cityQuest(quest::RAZE, h, c5);
  c5->razed = true;
  r = quest::event(g, *side, "turn");
  ok(r && !r->failed.empty() && r->why == 0x21, "a raze quest's city razed by another fails it");
  c5->razed = false;
  c5->ownerIndex = side->index;
  side->quest = cityQuest(quest::RAZE, h, c5);
  r = quest::event(g, *side, "turn");
  ok(r && !r->failed.empty() && r->why == 0x2a, "one taken and kept fails it too");
  c5->razed = razed5;
  c5->ownerIndex = owner5;
  side->quest = cityQuest(quest::OCCUPY, h, C(4));
  ev = quest::Event();
  ev.city = C(4);
  ev.stack = {h};
  r = quest::event(g, *side, "raze", ev);
  ok(r && !r->failed.empty(), "razing a city you were to keep fails the quest");
  Side* victim = g.sides.back();
  auto slaughter = [&](int required) {
    auto q = std::make_shared<Quest>();
    q->type = quest::SLAUGHTER; q->hero = h; q->targetKind = "side"; q->side = victim; q->required = required;
    return q;
  };
  side->quest = slaughter(3);
  Army d1, d2;
  d1.owner = victim->index;
  d2.owner = victim->index;
  ev = quest::Event();
  ev.stack = {h};
  ev.killed = {&d1, &d2};
  ok(quest::event(g, *side, "battle", ev) == nullptr, "two of three is not enough");
  eq(side->quest->done, 2, "the count rises");
  r = quest::event(g, *side, "battle", ev);
  ok(r && r->failed.empty(), "the third kill completes it");
  side->quest = slaughter(2);
  ev.stack = {};
  quest::event(g, *side, "battle", ev);
  eq(side->quest->done, 0, "kills away from the hero do not count");
  {
    auto q = std::make_shared<Quest>();
    q->type = quest::PILLAGE_GOLD; q->hero = h; q->targetKind = "none"; q->required = 100;
    side->quest = q;
  }
  ev = quest::Event();
  ev.stack = {h};
  ev.gold = 60;
  quest::event(g, *side, "pillage", ev);
  eq(side->quest->done, 60, "pillaged gold counts");
  r = quest::event(g, *side, "pillage", ev);
  ok(r && r->failed.empty(), "reaching the total completes it");
  Item item;
  item.index = 99; item.name = "Testsword"; item.type = rules::ITEM_BATTLE; item.value = 1; item.status = 3;
  h->items = {&item};
  {
    auto q = std::make_shared<Quest>();
    q->type = quest::RETRIEVE_ITEM; q->hero = h; q->targetKind = "item"; q->item = &item;
    side->quest = q;
  }
  ev = quest::Event();
  ev.hero = h;
  r = quest::event(g, *side, "item", ev);
  ok(r && r->failed.empty(), "carrying the item completes the quest");
  ok(std::find(h->items.begin(), h->items.end(), &item) == h->items.end(), "the quest item is taken away (the reward may add another)");
  eq(item.status, 0, "it leaves play");
  Army prey;
  prey.type = armytype::HERO;
  prey.owner = victim->index;
  {
    auto q = std::make_shared<Quest>();
    q->type = quest::SLAY_HERO; q->hero = h; q->targetKind = "army"; q->army = &prey;
    side->quest = q;
  }
  ev = quest::Event();
  ev.killed = {&prey};
  ok(quest::event(g, *side, "battle", ev) == nullptr, "someone else killing the quarry does not count");
  ev.stack = {h};
  r = quest::event(g, *side, "battle", ev);
  ok(r && r->failed.empty(), "the hero killing the quarry completes it");
  Army ghost;
  ghost.type = armytype::HERO;
  {
    auto q = std::make_shared<Quest>();
    q->type = quest::PILLAGE_GOLD; q->hero = &ghost; q->targetKind = "none"; q->required = 1;
    side->quest = q;
  }
  r = quest::event(g, *side, "turn");
  ok(r && !r->failed.empty(), "a quest with a dead hero is abandoned");
  side->gold = 50;
  Quest withHero;
  withHero.hero = h;
  auto reward = quest::reward(g, *side, &withHero);
  eq(reward->kind, "gold", "a side with no gold is given gold");
  ok(reward->gold >= 1002 && reward->gold <= 3000, "2d1000+1000: " + std::to_string(reward->gold));
  g.turn = 20;
  side->gold = 5000;
  reward = quest::reward(g, *side, &withHero);
  eq(reward->kind, "allies", "a small side past turn 15 is given allies");
  ok(!reward->armies.empty() && reward->armies.size() <= 8, "1d3+5 allies arrive");
}

static void testEndGame() {
  printf("end of the game\n");
  auto gp = newGame("ERYTHEA", seed(81));
  Game& g = *gp;
  for (Side* s : g.sides) s->computer = true;
  Side* winner = g.sides[0];
  for (auto& c : g.map->cities) c.ownerIndex = c.ownerIndex != NONE ? winner->index : NONE;
  for (size_t i = 1; i < g.sides.size(); i++) g.sides[i]->alive = false;
  auto r = game::checkEnd(g);
  ok(r.triumph, "the last side standing has triumphed");
  ok(!r.over, "and the game goes on, to be looked over");
  eq(r.winner, winner, "the last side standing wins");
  ok(!winner->computer, "and is switched to human control");
  ok(g.won, "the game-won flag is set");
  ok(!r.noHumans, "a game that never had a human does not say the last one fell");
  auto g2 = newGame("ERYTHEA", seed(82));
  for (auto& c : g2->map->cities) c.ownerIndex = NONE;
  ok(!game::checkEnd(*g2).over, "sides still in the game are players, cities or not");
  auto r2 = game::endRound(*g2);
  ok(r2.over, "with no cities owned the game is over at the round's end");
  eq(r2.winner, (Side*)nullptr, "and nobody won");
  ok(contains(r2.message, "No more players"), "with the right message");
  eq(r2.fallen.size(), g2->sides.size(), "every side fell");

  // the round's end puts a side with no city out (8065:18ab)
  auto g6p = newGame("ERYTHEA", seedWith(86, {{"diplomacy", 1}}));
  Game& g6 = *g6p;
  for (size_t i = 0; i < g6.sides.size(); i++) g6.sides[i]->computer = i > 0;
  game::begin(g6);
  Side* doomed = g6.sides[1];
  Side* other = g6.sides[2];
  for (City* c : game::sideCities(g6, *doomed)) c->ownerIndex = other->index;
  g6.diplomacy.state[doomed->index * 8 + other->index] = diplomacy::WAR;
  g6.diplomacy.state[other->index * 8 + doomed->index] = diplomacy::WAR;
  ok(!game::sideArmies(g6, *doomed).empty(), "the doomed side still has armies");
  game::endTurn(g6);
  eq(g6.side, other, "a computer side with no city has no turn");
  ok(doomed->alive, "but it is not out before the round's end");
  while (g6.turn == 1) game::endTurn(g6);
  ok(!doomed->alive, "the round's end puts it out");
  eq((int)game::sideArmies(g6, *doomed).size(), 0, "with all its armies");
  eq(doomed->gold, 0, "and no gold");
  eq(diplomacy::state(g6, other->index, doomed->index), diplomacy::INTERMEDIATE, "others stand uneasy with it");
  eq(diplomacy::proposal(g6, doomed->index, other->index), diplomacy::INTERMEDIATE, "both ways, proposals too");
  const Fallen* f = g6.ending.fallen.empty() ? nullptr : &g6.ending.fallen[0];
  ok(f && f->side == doomed, "its fall is told");
  ok(f && f->line >= 0 && f->line < game::FALLEN_LINES, "in a line of group 11");
  ok(f && f->boxed, "in a box while a human plays");

  // a human left with no city still plays its turn until the round's end
  auto g7p = newGame("ERYTHEA", seed(87));
  Game& g7 = *g7p;
  for (size_t i = 0; i < g7.sides.size(); i++) g7.sides[i]->computer = i + 1 < g7.sides.size();
  game::begin(g7);
  Side* me7 = g7.sides.back();
  for (City* c : game::sideCities(g7, *me7)) c->ownerIndex = g7.sides[0]->index;
  for (size_t i = 1; i < g7.sides.size(); i++) game::endTurn(g7);
  eq(g7.side, me7, "the human's turn comes round without a city");
  ok(me7->alive, "and it is still in the game");
  game::endTurn(g7);
  ok(!me7->alive, "the round's end puts it out");
  ok(me7->computer, "a fallen human is the computer's from then on");
  ok(g7.ending.noHumans, "the last human's fall is said");
  ok(g7.noHumansSaid, "once");
  for (size_t i = 0; i < g7.sides.size(); i++) game::endTurn(g7);
  ok(!g7.ending.noHumans, "and only once");
  auto g3p = newGame("ERYTHEA", seed(83));
  Game& g3 = *g3p;
  for (Side* s : g3.sides) s->computer = false;
  Side* me = g3.sides[0];
  for (size_t i = 1; i < g3.sides.size(); i++) g3.sides[i]->alive = false;
  int standing = (int)g3.map->cities.size(), given = 0;
  for (auto& c : g3.map->cities) c.ownerIndex = NONE;
  for (auto& c : g3.map->cities) if (given < standing / 2) { c.ownerIndex = me->index; given++; }
  ok(!game::checkEnd(g3).over, "exactly half is not enough");
  for (auto& c : g3.map->cities) if (c.ownerIndex == NONE) { c.ownerIndex = me->index; break; }
  auto r3 = game::checkEnd(g3);
  ok(r3.won, "more than half wins");
  ok(!r3.over, "and the game goes on, to be looked over");
  eq(r3.winner, me, "and names the winner");
  ok(!game::checkEnd(g3).won, "which is said once");
  auto g4p = newGame("ERYTHEA", seed(84));
  Game& g4 = *g4p;
  Side* human = g4.sides[0];
  human->computer = false;
  for (size_t i = 1; i < g4.sides.size(); i++) g4.sides[i]->computer = true;
  for (auto& c : g4.map->cities) c.ownerIndex = NONE;
  Side* rival = g4.sides[1];
  int n = (int)g4.map->cities.size();
  for (int i = 0; i < n; i++) {
    City& c = g4.map->cities[i];
    if (i + 1 <= n * 3 / 4) c.ownerIndex = human->index;
    else if (i + 1 == n) c.ownerIndex = rival->index;
  }
  auto r4 = game::checkEnd(g4);
  ok(r4.surrender, "a dominant human is offered surrender");
  ok(!r4.over, "but the game is not over");
  ok(g4.surrenderOffered, "and the flag is set");
  ok(!game::checkEnd(g4).surrender, "the offer is made once");
  auto g5p = newGame("ERYTHEA", seed(85));
  Game& g5 = *g5p;
  Side* side5 = g5.sides[0];
  for (size_t i = 1; i < g5.sides.size(); i++) g5.sides[i]->alive = false;
  for (Side* s : g5.sides) s->computer = false;
  for (auto& c : g5.map->cities) c.ownerIndex = NONE;
  g5.map->cities[0].ownerIndex = side5->index;
  for (size_t i = 1; i < g5.map->cities.size(); i++) g5.map->cities[i].razed = true;
  ok(game::checkEnd(g5).won, "one city among ruins is still more than half");
  game::acceptSurrender(g4, *human);
  ok(g4.won, "surrender accepted wins the game");
  ok(!rival->alive, "and the computer sides are out");
}

// ------------------------------------------------------------ saving a game

static void testSave() {
  printf("save and load\n");
  auto gp = newGame("ERYTHEA", seedWith(91, {{"quests", 1}, {"diplomacy", 1}}));
  Game& g = *gp;
  Side* side = game::begin(g);
  while (side && g.turn <= 8) {
    ai::playTurn(g, *side);
    side = game::endTurn(g);
  }
  Army* h = nullptr;
  for (Army* a : g.armies) if (a->type == armytype::HERO) h = a;
  ok(h != nullptr, "the game has a hero to save");
  if (!h) return;
  Side* owner = g.map->side(h->owner);
  owner->quest = cityQuest(quest::OCCUPY, h, &g.map->cities[4]);
  h->items = {&g.map->items[0]};
  g.map->items[0].status = 3;
  g.armies[0]->group = 4;
  g.armies[1]->group = 4;
  g.armies[0]->fortified = true;
  std::string text = save::encode(g);
  ok(text.size() > 1000, "the save has content");
  auto h2p = save::decode(text, DATA);
  Game& h2 = *h2p;
  eq(h2.turn, g.turn, "the turn survives");
  eq(h2.current, g.current, "whose turn it is survives");
  eq(h2.armies.size(), g.armies.size(), "every army survives");
  eq(h2.rng.state, g.rng.state, "the dice carry on where they left off");
  for (size_t i = 0; i < g.armies.size() && i < h2.armies.size(); i++) {
    Army* a = g.armies[i];
    Army* b = h2.armies[i];
    std::string n = std::to_string(i);
    eq(b->x, a->x, "army " + n + " keeps its place");
    eq(b->type, a->type, "army " + n + " keeps its type");
    eq(b->strength, a->strength, "army " + n + " keeps its strength");
    eq(b->moves, a->moves, "army " + n + " keeps its movement");
    eq(b->owner, a->owner, "army " + n + " keeps its owner");
    eq(b->group, a->group, "army " + n + " keeps the group it moves with");
    eq(b->fortified, a->fortified, "army " + n + " stays dug in");
  }
  for (size_t i = 0; i < g.map->cities.size(); i++) {
    const City& c = g.map->cities[i];
    const City& d = h2.map->cities[i];
    std::string n = std::to_string(i);
    eq(d.ownerIndex, c.ownerIndex, "city " + n + " keeps its owner");
    eq(d.producing, c.producing, "city " + n + " keeps its production");
    eq(d.slots.size(), c.slots.size(), "city " + n + " keeps its slots");
  }
  for (size_t i = 0; i < g.sides.size(); i++) {
    Side* s = g.sides[i];
    eq(h2.sides[i]->gold, s->gold, s->name + " keeps its gold");
    eq(h2.sides[i]->alive, s->alive, s->name + " keeps its standing");
    eq(h2.sides[i]->diploScore, s->diploScore, s->name + " keeps its score");
  }
  for (size_t i = 0; i < g.map->sites.size(); i++) {
    const Site& s = g.map->sites[i];
    const Site& t = h2.map->sites[i];
    std::string n = std::to_string(i);
    eq(t.content, s.content, "site " + n + " keeps its contents");
    eq(t.searched, s.searched, "site " + n + " remembers being searched");
    eq(t.rich, s.rich, "site " + n + " keeps its rich flag");
  }
  Side* owner2 = h2.map->side(owner->index);
  ok(owner2->quest != nullptr, "the quest survives");
  if (owner2->quest) {
    eq(owner2->quest->type, quest::OCCUPY, "with its type");
    eq(owner2->quest->city ? owner2->quest->city->index : NONE, g.map->cities[4].index, "and its target city");
    ok(owner2->quest->hero != nullptr, "and its hero");
    if (owner2->quest->hero) eq(owner2->quest->hero->type, armytype::HERO, "which is a hero");
  }
  bool carried = false;
  for (Army* a : h2.armies) if (!a->items.empty()) carried = true;
  ok(carried, "carried items survive");
  eq(diplomacy::state(h2, 0, 1), diplomacy::state(g, 0, 1), "the diplomatic state survives");
  Side* side2 = h2.sides[h2.current];
  int before = h2.turn;
  for (size_t i = 0; i < h2.sides.size() * 2; i++) {
    if (!side2) break;
    ai::playTurn(h2, *side2);
    side2 = game::endTurn(h2);
  }
  ok(h2.turn > before, "the reloaded game plays on");
  // a file round trip
  std::string path = (std::filesystem::temp_directory_path() / "w2-test-save.json").string();
  writeFile(path, text);
  auto h3 = save::decode(mustRead(path), DATA);
  eq(h3->armies.size(), g.armies.size(), "a save written to a file reads back");
  removeFile(path);
}

// ------------------------------------------------------------- the hidden map

static void testHiddenMap() {
  printf("hidden map\n");
  auto off = newGame("ERYTHEA", seed(101));
  ok(game::seen(*off, 0, 5, 5), "with the option off every tile is seen");
  eq(game::reveal(*off, 0, 5, 5, false), 0, "and nothing is revealed");
  auto gp = newGame("ERYTHEA", seedWith(101, {{"hiddenMap", 1}}));
  Game& g = *gp;
  Side* side = game::begin(g);
  ok(game::seen(g, side->index, side->capital->x, side->capital->y), "its capital is seen");
  ok(!game::seen(g, side->index, 0, 0), "the far corner is not");
  auto g2p = newGame("ERYTHEA", seedWith(102, {{"hiddenMap", 1}}));
  Game& g2 = *g2p;
  int ox = NONE, oy = NONE;
  for (int y = 20; y <= 60; y++)
    for (int x = 20; x <= 60; x++)
      if (ox == NONE && !game::cityAt(g2, x, y)) { ox = x; oy = y; }
  // side 20 is nobody, so its map starts completely dark
  eq(game::reveal(g2, 20, ox, oy, false), 9, "in the open a stack sees 3x3");
  eq(game::reveal(g2, 20, ox, oy, false), 0, "seeing it again reveals nothing new");
  eq(game::reveal(g2, 20, ox + 20, oy, true), 25, "a flying stack sees 5x5");
  City& city = g2.map->cities[0];
  eq(game::reveal(g2, 21, city.x, city.y, false), 25, "standing on a city sees 5x5");
  Army* army = game::sideArmies(g, *side)[0];
  army->moves = 99;
  City* unseen = nullptr;
  for (auto& c : g.map->cities)
    if (!unseen && !game::seen(g, side->index, c.x, c.y) && c.ownerIndex != side->index) unseen = &c;
  ok(unseen != nullptr, "there is a city the side has not seen");
  if (unseen) ok(!movement::findPath(g, {army}, army->x, army->y, unseen->x, unseen->y), "a side cannot path into the dark");
  auto count = [&]() {
    int n = 0;
    auto it = g.explored.find(side->index);
    if (it != g.explored.end()) for (uint8_t v : it->second) if (v) n++;
    return n;
  };
  int before = count();
  bool found = false;
  int tx = 0, ty = 0;
  for (int dx = -3; dx <= 3; dx++) {
    for (int dy = -3; dy <= 3; dy++) {
      int x = army->x + dx, y = army->y + dy;
      if (!found && x >= 0 && y >= 0 && game::seen(g, side->index, x, y) && (x != army->x || y != army->y)) {
        auto p = movement::findPath(g, {army}, army->x, army->y, x, y);
        if (p && !p->empty() && p->back().cost < movement::PAST_SHORE) { found = true; tx = x; ty = y; }
      }
    }
  }
  if (found) {
    movement::moveTo(g, {army}, tx, ty);
    int after = count();
    ok(after > before, fmt("walking uncovered %d more tiles", after - before));
  }
  Side* other = nullptr;
  for (Side* s : g.sides) if (s->index != side->index) other = s;
  ok(!game::seen(g, other->index, side->capital->x, side->capital->y), "another side has not seen our capital");
}

// --------------------------------------------------------------------- bugs

static void testBugFlags() {
  printf("bug compatibility\n");
  ok(rules::bugs.heroExperienceReadsAttackerTypes, "the original's bugs are reproduced by default");
  // every flag must actually control something: a flag nothing reads is a
  // promise the engine does not keep
  std::set<std::string> used;
  for (const char* module : {"hero", "combat", "game", "move", "ai", "site", "quest"}) {
    auto text = readFile(std::string("cpp/src/warlords/") + module + ".cpp");
    if (!text) continue;
    std::regex re("rules::bugs\\.(\\w+)");
    for (auto it = std::sregex_iterator(text->begin(), text->end(), re); it != std::sregex_iterator(); ++it) used.insert((*it)[1]);
  }
  for (const char* name : {"heroExperienceReadsAttackerTypes"}) ok(used.count(name), std::string("the ") + name + " flag is read by the engine");
}

// ------------------------------------------------------------ the computer AI

// The random map generator (warlords/randommap.cpp, docs/re/random_map.md):
// what it makes, and a game played on it and saved.
static void testRandomMap() {
  printf("random map\n");
  auto make = [](int seed, std::array<int, 4> sliders, bool allies) {
    Rng r(seed);
    randommap::Options o;
    o.dataDir = DATA;
    o.rng = &r;
    o.sliders = sliders;
    o.allies = allies;
    return randommap::generateNow(o);
  };
  auto a = make(7, {{3, 3, 2, 3}}, false), b = make(7, {{3, 3, 2, 3}}, false);
  for (const char* ext : randommap::FILES) {
    ok(a.count(ext) && !a[ext].empty(), std::string("the generator writes RANDOM.") + ext);
    ok(a[ext] == b[ext], std::string("the same dice make the same RANDOM.") + ext);
  }
  eq((int)a["SCN"].size(), 12001, "RANDOM.SCN is Erythea's .SCN rewritten (4fef:1001)");
  eq((int)a["MAP"].size(), 2 * 112 * 156, "RANDOM.MAP is a word a tile");
  randommap::install(DATA, a);
  auto map = scn::load(DATA + "/RANDOM", "RANDOM");
  eq((int)map->cities.size(), 80, "Cities at its third place gives RANDOM.DAT's 80 (4bed:0000)");
  eq((int)map->sites.size(), 40, "forty sites (513d:003a)");
  ok(map->signs.size() >= 41 && map->signs.size() <= 70, "1d30+40 signposts (4fef:113d)");
  std::set<City*> capitals;
  for (auto& sd : map->sides) {
    ok(sd.capital && sd.capital->owner == &sd, sd.name + " holds its capital (513d:0689)");
    capitals.insert(sd.capital);
  }
  eq((int)capitals.size(), 8, "eight capitals, one a side");
  for (auto& c : map->cities) {
    eq(map->terrainType[scn::tileAt(*map, c.x, c.y) % 256], 10, c.name + " stands on a castle tile");
    ok(!c.produces.empty(), c.name + " makes something (513d:161d)");
    bool capital = c.owner != nullptr;
    ok(c.income >= (capital ? 33 : 15) && c.income <= (capital ? 40 : 28),
       c.name + "'s income is value x 2 + 1d8 + 14 (513d:1171)");
    auto t = map->cityText.find(c.index);
    ok(t != map->cityText.end() && startsWith(t->second[0], c.name + " is a"), c.name + " has its description");
  }
  Types types;
  armytype::load(DATA + "/TERRAIN0/ARMYTYPE.DAT", types);
  auto magical = [&](int t) { return types.byId(t) && types.byId(t)->bonus[48] != 0; };
  bool none = true;
  for (auto& c : map->cities) for (int t : c.produces) if (magical(t)) none = false;
  ok(none, "without allies no city makes a magical army (513d:161d, 1c1d)");
  int allies = 0;
  for (int seed = 1; seed <= 10; seed++) {
    randommap::install(DATA, make(seed, {{3, 3, 2, 3}}, true));
    auto m = scn::load(DATA + "/RANDOM", "RANDOM");
    for (auto& c : m->cities)
      for (int t : c.produces) if (magical(t)) allies++;
  }
  ok(allies > 0, "with allies on, 2d3 cities may make them (513d:1b3f)");
  randommap::install(DATA, make(3, {{0, 0, 6, 0}}, false));
  eq((int)scn::load(DATA + "/RANDOM", "RANDOM")->cities.size(), 100, "Cities at its last place: 80 + 20");
  randommap::install(DATA, make(3, {{6, 6, 0, 6}}, false));
  eq((int)scn::load(DATA + "/RANDOM", "RANDOM")->cities.size(), 70, "Cities at its first place: 80 - 10");
  Rng r5(5);
  auto rolled = randommap::settle({{7, 7, 7, 7}}, r5);
  bool inRange = true;
  for (int v : rolled) if (v < 0 || v > 6) inRange = false;
  ok(inRange, "a slider left to chance is rolled 1d7-1 (7bab:10e8)");
  // the shipped RANDOM folder is the original's last world, left alone
  std::ifstream disk(DATA + "/RANDOM/RANDOM.SCN", std::ios::binary);
  if (disk) {
    std::string onDisk((std::istreambuf_iterator<char>(disk)), std::istreambuf_iterator<char>());
    ok(onDisk != a["SCN"], "the made world is kept in memory, not written over RANDOM.SCN");
  }

  // a game on it, and a save that carries the world with it
  randommap::install(DATA, a);
  game::NewGameOptions o;
  o.seed = 4;
  auto gp = game::newGame(DATA, "RANDOM", o);
  Game& g = *gp;
  for (Side* sd : g.sides) sd->computer = true;
  Side* side = game::begin(g);
  while (side && g.turn <= 6) { ai::playTurn(g, *side); side = game::endTurn(g); }
  eq(g.turn, 7, "the computer players play a random world");
  std::string text = save::encode(g);
  randommap::install(DATA, make(8, {{3, 3, 2, 3}}, false));
  auto g2 = save::decode(text, DATA);
  ok(g2->map->tiles == g.map->tiles, "a saved random world comes back with its own map");
  bool names = g2->map->cities.size() == g.map->cities.size();
  for (size_t i = 0; names && i < g.map->cities.size(); i++) names = g2->map->cities[i].name == g.map->cities[i].name;
  ok(names, "... and its own cities");
  eq(g2->map->signs.size(), g.map->signs.size(), "... and its own signposts");
}


static void testComputerPlayers() {
  printf("computer players\n");
  namespace core = ai::core;
  namespace groups = ai::groups;
  for (int level = 0; level <= 2; level++) eq(aicard::count(DATA, level), 9, "nine cards for level " + std::to_string(level));
  auto w = *aicard::load(DATA, 2, 0);
  eq(w.groups, 4, "the Standard Warlord runs four assault groups");
  eq(w.raze, 5, "and razes 5 in 1000");
  eq(w.humanShare, 3, "and turns on a human with 3x10+1d10 percent");
  eq(w.solidarity, 1, "and stands with the other computers");
  auto k = *aicard::load(DATA, 0, 0);
  eq(k.cautious, 1, "the Standard Knight is cautious");
  eq(k.groups, 1, "and runs one group");
  eq(aicard::describe(DATA, 2, 1)->first, "Attila the Hun", "a card's name is the first line of its .DSC");
  auto lords = aicard::deck(DATA, 1);
  eq((int)lords.size(), 9, "the setup screen lists a Lord's nine characters");
  eq(lords[0], "Standard Lord", "the Standard one first");
  eq(lords[2], "Roland the Rabid", "each by its .DSC's first line");
  {
    // a character chosen on the setup screen is the card the side plays
    GameOpts ro = seed(70);
    ro.sides[0] = sideSetup(false, 0);
    game::SideSetup rabid = sideSetup(true, 1);
    rabid.card = 2;
    rabid.name = "Rabids";
    ro.sides[1] = rabid;
    auto rp = newGame("ERYTHEA", ro);
    Side& rs = rp->map->sides[1];
    eq(rs.card, 2, "the side keeps its character");
    eq(rs.name, "Rabids", "and the name it was given");
    eq(rs.ai->maxGroups, 2, "Roland the Rabid runs two assault groups");
    eq(rs.ai->raze, 14, "razes 14 in 1000");
    eq(rs.ai->sack, 100, "sacks 100");
    eq(rs.ai->early, 1, "and takes early vengeance on a human");
    eq(rp->map->sides[2].ai->maxGroups, 4, "a side left alone plays its Standard card");
  }
  eq(Rng(5).dice(1, 0, 3), 3, "dice(1, 0, 3) is 3");
  eq(core::dist(0, 0, 3, 4), 5, "distance is Euclidean");
  eq(core::dist(0, 0, 1, 1), 1, "and truncated");

  GameOpts o = seed(71);
  for (int i = 0; i <= 7; i++) o.sides[i] = sideSetup(true, i % 3);
  o.sides[0] = sideSetup(false, 0);
  auto gp = newGame("ERYTHEA", o);
  Game& g = *gp;
  Side& knight = g.map->sides[3];
  Side& lord = g.map->sides[1];
  Side& warlord = g.map->sides[2];
  eq(knight.level, 0, "side 3 plays as a Knight");
  eq(knight.ai->maxGroups, 1, "a Knight runs one group");
  eq(knight.ai->cautious, 1, "and is cautious");
  ok(!knight.ai->bold, "and never attacks a side it is not at war with");
  eq(lord.ai->maxGroups, 3, "a Lord three");
  ok(lord.ai->bold, "a Lord is bold -- the level sets it and a card cannot clear it");
  eq(warlord.ai->maxGroups, 4, "a Warlord four");
  ok(warlord.ai->humanShare >= 31 && warlord.ai->humanShare <= 40,
     "a Warlord turns on a human holding 31-40% of the world: " + std::to_string(warlord.ai->humanShare));
  ok(knight.ai->humanShare >= 81 && knight.ai->humanShare <= 90, "a Knight at 81-90%");
  eq(g.map->fightOrder[3][8], w.fightOrder[8], "the card sets the side's fight order");
  int unclaimed = 0;
  for (auto& c : g.map->cities) if (c.claim == NONE || c.claim == core::NEUTRAL) unclaimed++;
  eq(unclaimed, 0, "the computers claim every city between them (623c:010b)");
  for (Side* s : g.sides) eq(s->capital->claim, s->index, s->name + " claims its capital");
  for (Side* s : g.sides) ok(s->diploScore >= 1 && s->diploScore <= 8, "a diplomatic score starts at 1d8");

  GameOpts og = o;
  og.seed = 72;
  og.greatest = true;
  og.options = {{"diplomacy", 1}};
  auto ggp = newGame("ERYTHEA", og);
  Game& gg = *ggp;
  ok(gg.map->sides[0].diploScore > 400, "I am the Greatest: a human's score starts above 400");
  ok(gg.map->sides[1].diploScore <= 8, "a computer's does not");
  gg.diplomacy.state[1 * 8 + 0] = diplomacy::WAR;
  gg.diplomacy.state[0 * 8 + 1] = diplomacy::WAR;
  auto fights = groups::fightingHumans(gg);
  ok(fights[1], "a computer at war with the human is marked");
  ai::diplo::phase(gg, gg.map->sides[1]);
  eq(diplomacy::proposal(gg, 1, 2), diplomacy::PEACE, "and no computer proposes war on it");

  GameOpts oh = o;
  oh.seed = 73;
  oh.options = {{"hiddenMap", 1}};
  auto hp = newGame("ERYTHEA", oh);
  Game& h = *hp;
  Side& humanS = h.map->sides[0];
  Side& comp = h.map->sides[1];
  auto hgrid = movement::grid(h, humanS.index);
  auto hpd = movement::prepare(h, hgrid, humanS.index, movement::LAND, 0, 0);
  auto cpd = movement::prepare(h, hgrid, comp.index, movement::LAND, 0, 0);
  int fogged = 0;
  for (int k2 = 0; k2 < h.map->width * h.map->height; k2++) {
    if (hpd[k2] == movement::SHUT && cpd[k2] != movement::SHUT) fogged++;
  }
  ok(fogged > 1000, "unseen ground blocks the human's paths only: " + std::to_string(fogged));

  Army* army = nullptr;
  for (Army* a : g.armies) if (a->owner == warlord.index) army = a;
  auto sel = core::select(g, {army});
  eq(core::odds(g, sel, 0, 0), 100, "nothing to fight is a sure thing");
  auto [nb, nd] = core::neighbours(g, g.map->cities[0]);
  eq((int)nb.size(), 6, "a city has six neighbours");
  for (int v : nd) ok(v < 100, "each within reach");

  eq(groups::pickEnemy(g, warlord), NONE, "no enemy is picked in the first turns");
  warlord.ai->battles[4] = 3;
  warlord.ai->citiesLost[4] = 2;
  ok(groups::pickEnemy(g, warlord) != NONE, "a side that has fought us can be picked");

  City* c = &g.map->cities[0];
  for (auto& cc : g.map->cities) if (&cc != knight.capital && &cc != warlord.capital) { c = &cc; break; }
  c->ownerIndex = warlord.index;
  int before = warlord.diploScore;
  ok(groups::earlyVengeance(g, warlord.index, *c, 0), "a Warlord takes vengeance on a human's city");
  ok(warlord.diploScore > before, "and it is an atrocity");
  ok(!groups::earlyVengeance(g, knight.index, *c, 0), "a Knight does not");

  Side* side = game::begin(g);
  while (side && g.turn <= 12) {
    if (side->computer) ai::playTurn(g, *side);
    side = game::endTurn(g);
  }
  auto grp = warlord.ai->groups[1];
  grp->active = 2;
  grp->target = 1;
  grp->rally = warlord.capital->index;
  grp->staged[2] = g.armies.back();
  auto g2 = save::decode(save::encode(g), DATA);
  auto& w2 = *g2->map->sides[warlord.index].ai;
  eq(w2.maxGroups, warlord.ai->maxGroups, "the AI data survives a save");
  eq(w2.groups[1]->rally, warlord.capital->index, "and its groups");
  eq(w2.groups[1]->staged[2], g2->armies.back(), "with their staged armies");
  eq(g2->map->cities[4].claim, g.map->cities[4].claim, "and the cities' claims");
}

// --------------------------------------------------------------- encampments

static void testEncampment() {
  printf("encampments\n");
  auto gp = newGame("ERYTHEA", seed(5));
  Game& g = *gp;
  Side* side = game::begin(g);
  City* cap = side->capital;
  Army* a = nullptr;
  for (Army* b : g.armies) if (!a && b->owner == side->index && !b->transit) a = b;
  int x = NONE, y = NONE;
  for (int r = 2; r <= 8 && x == NONE; r++) {
    for (int dx = -r; dx <= r && x == NONE; dx++) {
      for (int dy = -r; dy <= r && x == NONE; dy++) {
        int tx = cap->x + dx, ty = cap->y + dy;
        if (tx >= 0 && ty >= 0 && tx < g.map->width - 1 && ty < g.map->height &&
            scn::terrainAt(*g.map, tx, ty) == movement::PLAIN && scn::terrainAt(*g.map, tx + 1, ty) == movement::PLAIN &&
            scn::roadAt(*g.map, tx, ty) % 32 == 0 && game::armiesAt(g, tx, ty).empty() && game::armiesAt(g, tx + 1, ty).empty()) {
          x = tx; y = ty;
        }
      }
    }
  }
  ok(a != nullptr && x != NONE, "an army and open ground to encamp on");
  if (!(a && x != NONE)) return;
  a->x = x;
  a->y = y;
  ok(!game::towerAt(g, x, y), "a stack on open ground is no encampment");
  game::startTurn(g, *side);
  ok(!game::towerAt(g, x, y), "nor after a turn in the army cycle");
  a->fortified = true;
  game::startTurn(g, *side);
  ok(game::towerAt(g, x, y), "a defended stack encamps as its turn opens");
  eq(combat::fortify(g, {}, x, y, combat::terrainClass(g, x, y)), 1, "an encampment fights as a fortification of 1");
  auto r = movement::moveTo(g, {a}, x + 1, y);
  eq(r.steps, 1, "the encamped stack walks off");
  ok(!game::towerAt(g, x, y), "and the empty tile is no encampment any more");
  a->x = cap->x;
  a->y = cap->y;
  game::startTurn(g, *side);
  ok(!game::towerAt(g, cap->x, cap->y), "a defended stack in a city does not encamp");
}

// ------------------------------------------------------------- going to sea

static void testSea() {
  printf("going to sea\n");
  auto gp = newGame("ERYTHEA", seed(81));
  Game* g = gp.get();
  int lx = NONE, ly = NONE, wx = NONE, wy = NONE;
  for (int y = 1; y <= g->map->height - 2; y++) {
    for (int x = 1; x <= g->map->width - 2; x++) {
      int t2 = scn::terrainAt(*g->map, x + 1, y);
      if (lx == NONE && scn::terrainAt(*g->map, x, y) == movement::PLAIN && (t2 == movement::WATER || t2 == movement::SHORE) &&
          game::armiesAt(*g, x, y).empty() && game::armiesAt(*g, x + 1, y).empty()) {
        lx = x; ly = y; wx = x + 1; wy = y;
      }
    }
  }
  ok(lx != NONE, "Erythea has a plain beside the sea");
  const ArmyType* dragon = nullptr;
  for (auto& t : g->types.all) if (t.flies && contains(t.name, "Dragon")) dragon = &t;
  auto army = [&](int typeId) { return addArmy(*g, lx, ly, 0, typeId, 5, 20, 20); };
  Army* hero = army(armytype::HERO);
  Army* d = army(dragon->id);
  std::vector<Army*> stack = {hero, d};
  eq(movement::modeOf(*g, stack), movement::FLYING, "a hero with a dragon flies");
  movement::walk(*g, stack, {movement::Step{wx, wy, 2}});
  eq(hero->x, wx, "and flies out over the water");
  ok(!d->atSea && !hero->atSea, "without either going to sea");
  eq(movement::modeOf(*g, stack), movement::FLYING, "so it still flies");
  movement::walk(*g, stack, {movement::Step{lx, ly, 2}});
  eq(hero->x, lx, "and can come back to land");
  Army* foot = army(1);
  movement::settleSea(*g, {foot}, wx, wy, movement::LAND, false);
  ok(foot->atSea, "a land army ending on water is at sea");
  movement::settleSea(*g, {foot}, lx, ly, movement::LAND, true);
  ok(!foot->atSea, "and ashore again on land");
  movement::settleSea(*g, {hero, d, foot}, wx, wy, movement::LAND, false);
  ok(!d->atSea, "a flier never goes to sea");
  ok(hero->atSea && foot->atSea, "the hero and the footman with it do");
  hero->atSea = false;
  foot->atSea = false;
  movement::settleSea(*g, {foot}, wx, wy, movement::BOAT, false);
  ok(!foot->atSea, "a boat move leaves the flag as it was");
  hero->x = wx; hero->y = wy; d->x = wx; d->y = wy;
  hero->atSea = true;
  d->atSea = true;
  g->remove(foot);
  auto g2 = save::decode(save::encode(*g), DATA);
  for (Army* a : g2->armies) if (a->x == wx && a->y == wy) ok(!a->atSea, "a loaded flier and its hero are not at sea");

  auto gp2 = newGame("ERYTHEA", seed(81));
  g = gp2.get();
  City* mirea = nullptr;
  for (auto& c : g->map->cities) if (c.name == "Mirea") mirea = &c;
  ok(mirea && mirea->ownerIndex == 0 && movement::isPort(*g, *mirea), "Mirea is side 0's port");
  if (!mirea) return;
  auto& grid = movement::grid(*g, 0);
  auto byte = [&](int x, int y) { return grid[y * g->map->width + x]; };
  eq(scn::terrainAt(*g->map, 86, 7), movement::SHORE, "the shore runs past Mirea");
  ok(movement::has(byte(86, 7), movement::WATER_F), "and a shore tile is water to a walker");
  ok(!movement::stepCost(byte(83, 9), byte(82, 9), movement::LAND, false, false, 10).has_value(),
     "so a land stack cannot step onto it from a plain");
  Army* sailor = addArmy(*g, mirea->x, mirea->y, 0, 1, 5, 20, 20);
  auto r = movement::moveTo(*g, {sailor}, 86, 4);
  eq(sailor->y, 7, "leaving port it stops on the first sea tile");
  eq(scn::terrainAt(*g->map, sailor->x, sailor->y), movement::SHORE, "the shore");
  ok(sailor->atSea, "at sea");
  eq(r.spent, movement::COST[movement::SHORE], "paying the tile's cost and no water charge");
  eq(sailor->moves, 0, "and going to sea uses up the rest of its move");
  eq(r.stopped, "out of moves", "which ends the walk");
  sailor->moves = 20;
  r = movement::moveTo(*g, {sailor}, 86, 4);
  eq(sailor->y, 4, "next turn it sails out into open water");
  ok(sailor->atSea, "still at sea");
  eq(sailor->moves, 20 - (7 - 4) * movement::COST[movement::WATER], "paying 1 a tile");
  auto p = movement::findPath(*g, {sailor}, 86, 4, 86, 5);
  eq(p ? (*p)[0].cost : NONE, movement::COST[movement::WATER], "sailing on costs no water charge either");
  r = movement::moveTo(*g, {sailor}, mirea->x, mirea->y);
  eq(sailor->x * 1000 + sailor->y, mirea->x * 1000 + mirea->y, "it sails back into port");
  ok(!sailor->atSea, "and comes ashore there");
  eq(sailor->moves, 0, "which uses up its move too");
  City* kuuria = nullptr;
  for (auto& c : g->map->cities) if (c.name == "Kuuria") kuuria = &c;
  int sx = NONE, sy = NONE;
  for (int x = kuuria->x - 1; x <= kuuria->x + 2; x++) {
    for (int y = kuuria->y - 1; y <= kuuria->y + 2; y++) {
      int t = scn::terrainAt(*g->map, x, y);
      if (sx == NONE && (t == movement::WATER || t == movement::SHORE) && !g->map->crossing[y * g->map->width + x] &&
          game::armiesAt(*g, x, y).empty()) { sx = x; sy = y; }
    }
  }
  ok(sx != NONE, "Kuuria has open water beside it");
  sailor->x = sx; sailor->y = sy; sailor->atSea = true; sailor->moves = 20;
  ok(movement::findPath(*g, {sailor}, sx, sy, kuuria->x, kuuria->y).has_value(), "a stack at sea can path into the port it attacks");
}

// --------------------------------------------------------------------- sound

// What plays when. The OPL synthesis is checked in the sound step.
static void testSound() {
  printf("sound\n");
  auto files = uidata::strings(DATA + "/DATA/FILE.DAT");
  auto first = [](int) { return 1; };
  auto [name, loop] = cues::song(files, cues::TITLE, {}, first);
  eq(name, "STARTUP.XMI", "the start screens play STARTUP.XMI");
  ok(loop, "and it starts again when it ends");
  Side human;
  human.computer = false;
  human.alive = true;
  std::tie(name, loop) = cues::song(files, cues::COMPUTER, {&human}, first);
  eq(name, "INT2.XMI", "a computer's turn beside a human plays FILE.DAT group 11");
  ok(!loop, "once");
  eq(cues::song(files, cues::BEGIN, {}, first).first, "INT22.XMI", "the war begins to INT22");
  Game g;
  g.map = std::make_unique<Map>();
  g.turn = 1;
  Side side;
  side.index = 0;
  side.gold = 500;
  g.map->cities.resize(6);
  for (auto& c : g.map->cities) c.ownerIndex = 0;
  eq(cues::advisor(g, side, first), NONE, "the advisor never speaks on turn 1");
  eq(side.advisor.mark, 5, "but he has marked six cities down as five");
  g.turn = 2;
  eq(cues::advisor(g, side, first), NONE, "nothing new, not a seventh turn: silence");
  for (int i = 0; i < 5; i++) g.map->cities[i].ownerIndex = 1;
  eq(cues::advisor(g, side, first), 40, "down to one city: 'Thy sorry efforts...' (VLOSE05)");
  eq(side.advisor.dir, 2, "and he remembers saying so");
  for (int i = 0; i < 5; i++) g.map->cities[i].ownerIndex = 0;
  eq(cues::advisor(g, side, first), 47, "back to six: 'Thou art doing well... so far!'");
  g.turn = 7;
  side.gold = 50;
  eq(cues::advisor(g, side, first), 54, "on a seventh turn, short of gold: VGOLD00");
}

// --------------------------------------------------------------------- main

int main() {
  if (!fileExists(DATA + "/TERRAIN0/ARMYTYPE.DAT")) {
    printf("cannot find the game data in %s: run from the repository root\n", DATA.c_str());
    return 1;
  }
  testDice();
  testArmyTypes();
  testProduction();
  testBugFlags();
  for (auto& s : SCENARIOS) if (fileExists(DATA + "/" + s + "/" + s + ".SCN")) testGame(s);
  testTurnLoop("ERYTHEA");
  testVectoring("ERYTHEA");
  testBuyProduction("ERYTHEA");
  testReports("ERYTHEA");
  testHeroItems("ERYTHEA");
  testSage("ERYTHEA");
  testSetup();
  testHistory("ERYTHEA");
  testDisband("ERYTHEA");
  testMovement("ERYTHEA");
  testMovement("ISLADIA");
  testStackLimit("ERYTHEA");
  testSea();
  testEncampment();
  testCombat("ERYTHEA");
  testCapture("ERYTHEA");
  testCityChoices("ERYTHEA");
  testHeroes("ERYTHEA");
  testHeroExperienceBug();
  testDiplomacy();
  testQuests();
  testEndGame();
  testHiddenMap();
  testSave();
  testSites("ERYTHEA");
  testSites("DRAGON");
  testSlots();
  testScreenLayout();
  testCityCastles();
  testComputerPlayers();
  testRandomMap();
  testAIGame("TUTORIA", 30);
  testAIGame("ERYTHEA", 25);
  if (fileExists(DATA + "/TUTORIA/TUTORIA.SCN")) testTutorialHero();
  testSound();
  printf("\n%d passed, %d failed\n", passed, failed);
  return failed == 0 ? 0 : 1;
}
