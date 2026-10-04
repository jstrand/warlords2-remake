// The game's state, as structs. The Lua keeps it in tables that grow fields
// as they go; here every field is declared, and Lua's nil is NONE (-1) for an
// index or a coordinate, or std::optional where -1 is a real value.
//
// A Game owns every army it ever made (`pool`), so a pointer held by a quest,
// an AI group or the selection stays valid after the army has left
// `armies`. Being in the game is being in `armies`.
#pragma once

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "util/json.hpp"
#include "warlords/rng.hpp"

namespace w2 {

constexpr int NONE = -1;

struct Side;
struct City;
struct Army;
struct Item;
struct Game;
struct AIData;

// ------------------------------------------------------------- army types

struct ArmyType {
  int id = 0;              // also the sprite index
  std::string name;
  int strength = 0, time = 0;
  int cost = 0;            // per-turn upkeep basis
  int move = 0;
  int price = 0;           // negative: can never be bought
  std::array<int, 64> bonus{};   // keyed by field offset, 32..60
  bool flies = false, siege = false, boat = false, woodsMove = false, hillsMove = false;
};

struct Types {
  std::vector<ArmyType> all;              // in file (display) order
  std::array<ArmyType*, 64> ids{};        // by type id
  const ArmyType* byId(int id) const { return id >= 0 && id < 64 ? ids[id] : nullptr; }
  std::vector<const ArmyType*> list() const {
    std::vector<const ArmyType*> out;
    for (auto& a : all) out.push_back(&a);
    return out;
  }
};

// --------------------------------------------------------------- the map

/** A production slot: an army type with the city's own numbers for it. */
struct Slot {
  int type = 0;
  std::string name;
  int strength = 0, time = 0, cost = 0, move = 0, price = 0;
  bool operator==(const Slot& o) const {
    return type == o.type && name == o.name && strength == o.strength && time == o.time &&
           cost == o.cost && move == o.move && price == o.price;
  }
};

struct City {
  int index = 0;
  int x = 0, y = 0;
  std::string name;
  int income = 0;
  std::vector<int> produces;   // army type ids, as the scenario gives them
  int defence = 0;
  Side* owner = nullptr;       // only while the scenario loads
  int ownerIndex = NONE;       // the side holding it; NONE for neutral
  std::vector<Slot> slots;
  int producing = NONE;        // slot index being built
  int countdown = 0;
  int vectorTo = NONE;         // a city index, or STANDARD
  bool razed = false;
  int razedBy = NONE;
  int claim = NONE;            // the computer whose ground it is
};

struct Item {
  int index = 0;
  std::string name;
  int type = 0, value = 0;
  int status = 0;              // 0 out of play, 1 on the ground, 2 hidden, 3 carried
  int x = NONE, y = NONE;
  bool planted = false;
  int standardOf = NONE;       // items 0-7: the eight sides' standards
};

struct PoolItem {
  std::string name;
  int type = 0, value = 0;
};

struct Site {
  int index = 0;
  int x = 0, y = 0;
  std::string name;
  int type = 0;                // 1 = temple, 2 = ruin
  int content = 0;
  int item = NONE;
  int guardian = NONE;
  int allyType = NONE;
  bool rich = false;
  int revealed = 0;            // a bit per side the site is shown to
  bool searched = false;
  std::string band;            // "rich", "near" or "far"
  int templeIndex = NONE;
};

struct Monster {
  int index = 0;
  std::string name;
  int strength = 0;
};

struct Sign {
  int index = 0;
  int x = 0, y = 0;
  std::array<std::string, 2> lines;
};

struct Options {
  int neutralCities = 0, diplomacy = 0, quests = 0, randomTurns = 0, hiddenMap = 0;
  int intenseCombat = 0, quickStart = 0, viewEnemies = 0, militaryAdvisor = 0;
  int tutorial = 0, viewProduction = 0;
  /** By the Lua's name for it; unknown names are ignored. */
  int* field(const std::string& name);
  static const std::vector<std::string>& names();
};

// ------------------------------------------------------------------ sides

/** What a side's turn start produced, for the report (side.produced). */
struct Produced {
  std::string kind;            // "built", "sent", "arrived"
  int type = 0;
  int city = NONE;
  bool standard = false;
};

/** A quest (quest.lua). The target is one of the pointers, as targetKind says. */
struct Quest {
  int type = 0;
  Army* hero = nullptr;
  std::string targetKind;      // "city", "side", "item", "armytype", "army", "none"
  City* city = nullptr;
  Side* side = nullptr;
  Item* item = nullptr;
  const ArmyType* armyType = nullptr;
  Army* army = nullptr;
  int required = NONE;
  int done = 0;
  bool hasTarget() const {
    return targetKind == "none" || city || side || item || armyType || army;
  }
};

struct Reward;
struct QuestResult;

/** The advisor's marks (cues.lua). */
struct Advisor {
  int dir = 0, mark = 0;
};

struct HeroOffer;

struct Side {
  int index = 0;
  std::string name;
  int colour = 0, edge = 0;
  int gold = 0;
  int capX = 0, capY = 0;
  bool computer = false;
  int level = 0;
  bool enhanced = false;
  bool observe = false;
  int diploScore = 0;
  int card = 0;
  bool inUse = false;
  City* capital = nullptr;
  bool alive = false;
  int ownerIndex = 0;
  int income = 0, upkeepTotal = 0;
  std::vector<Produced> produced;
  std::vector<std::string> diploNews;
  std::shared_ptr<HeroOffer> heroOffer;
  std::shared_ptr<Quest> quest;
  std::shared_ptr<QuestResult> questNews;
  std::shared_ptr<AIData> ai;
  int aiSolidarity = 0;
  Advisor advisor;
};

// ------------------------------------------------------------------ armies

struct Transit {
  int turns = 0;
  int dest = NONE;             // a city index, or STANDARD
};

struct Army {
  int x = NONE, y = NONE;      // NONE while in transit
  int owner = NONE;            // NONE for neutral
  int type = 0;
  std::string name;
  bool female = false;
  int strength = 0;
  int moves = 0, maxMoves = 0;
  int upkeep = 0;
  int homeCity = NONE;
  bool atSea = false;
  int group = 0;
  std::optional<std::pair<int, int>> target;   // where it is walking to
  bool fortified = false;
  // heroes
  int level = 0, experience = 0;
  std::string title;
  std::vector<Item*> items;
  std::set<int> blessings;     // temples that have blessed it
  std::optional<Transit> transit;
  bool returning = false;
  // the turn's marks
  bool done = false, offered = false, disbanded = false;
  // the computer player's
  int aiOrder = 0;
  int aiDest = NONE;
  int aiGroup = 0;
  bool aiExplore = false, aiNeutral = false, aiParty = false, aiMoved = false;

  bool hero() const;
};

// ----------------------------------------------------------------- the map

constexpr int MAP_W = 112, MAP_H = 156;

struct Map {
  std::string name;
  std::array<Side, 8> sides;
  std::vector<City> cities;
  std::vector<City*> cityAt;     // a city's top-left tile
  std::vector<City*> cityTile;   // every tile of its 2x2 footprint
  std::vector<Site> sites;
  std::vector<Site*> siteAt;
  std::vector<Item> items;
  std::array<std::optional<Monster>, 10> monsters;
  std::vector<bool> crossing;
  std::optional<std::vector<PoolItem>> itemPool;
  std::map<int, std::array<std::string, 3>> cityText, siteText;
  std::vector<Sign> signs;
  Options options;
  std::array<int, 256> terrainType{};
  std::vector<std::vector<int>> fightOrder;   // 9 rows of 29, row 8 neutral
  int combatCap = 0;
  std::vector<int> tiles;
  std::string roads;
  int width = MAP_W, height = MAP_H;

  Side* side(int i) { return i >= 0 && i < 8 ? &sides[i] : nullptr; }
  City* city(int i) { return i >= 0 && i < (int)cities.size() ? &cities[i] : nullptr; }
};

// ----------------------------------------------------------------- the game

/** A deed of the round, for the history (history.lua). */
struct Deed {
  int side = 0, type = 0, v1 = 0, v2 = 0;
  std::string name;
};
/** A round's record: what History plays back. */
struct HistoryRecord {
  std::array<int, 8> gold{}, score{}, cities{};
  std::vector<int> owners;
  std::vector<Deed> events;
};
struct Ending {
  bool over = false, won = false, surrender = false, noHumans = false;
  Side* winner = nullptr;
  std::string message;
};

struct Grids;     // move.cpp's cached cost grids
struct Neighbours;

struct Diplomacy {
  std::array<int, 64> state{};
  std::array<int, 64> proposal{};   // NONE where none was made
};

struct Game {
  Game();
  ~Game();
  Rng rng;
  std::string dataDir;
  Types types;
  std::unique_ptr<Map> map;
  std::vector<Army*> armies;
  std::vector<std::unique_ptr<Army>> pool;
  int turn = 1;
  std::vector<std::string> log;
  bool greatest = false;
  std::vector<Side*> sides;
  int current = 0;
  Side* side = nullptr;
  Diplomacy diplomacy;
  std::map<int, std::vector<uint8_t>> explored;
  std::set<int> towers;
  bool won = false, over = false, noHumansSaid = false, surrenderOffered = false;
  Ending ending;
  std::vector<HistoryRecord> history;
  std::map<int, std::vector<Deed>> deeds;
  std::map<int, std::map<int, std::array<int, 5>>> triumphs;
  std::set<std::string> tutorialSeen;   // the tutorial's moments, as shown
  std::map<int, std::vector<std::pair<std::string, bool>>> heroNames;
  std::shared_ptr<Grids> grids;
  std::shared_ptr<Neighbours> aiNeighbours;

  /** A new army, owned by the game but not yet in it. */
  Army* make(const Army& a);
  /** A new army, in the game. */
  Army* add(const Army& a) {
    Army* p = make(a);
    armies.push_back(p);
    return p;
  }
  bool alive(const Army* a) const;
  void remove(const Army* a);
};

}  // namespace w2
