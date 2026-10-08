// Game state and the turn loop.
//
// Headless on purpose: this never touches SDL, so the rules can be run and
// checked by the tests. Rendering and input sit on top.
#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "warlords/combat.hpp"
#include "warlords/types.hpp"

namespace w2 {
struct SearchResult;
struct QuestResult;
}

namespace w2::game {

constexpr int STANDARD = -2;      // vector destination meaning "the side's standard"
constexpr int MAX_VECTORED_TO = 4;  // no more than four cities may send to one place

/** How a new game is set up. */
struct SideSetup {
  bool off = false, computer = false;
  int level = NONE;
  int card = 0;
  std::string name;   // retyped on the setup screen; empty keeps the scenario's
};
struct NewGameOptions {
  double seed = 0;
  std::vector<std::pair<std::string, int>> options;   // option name, value
  std::map<int, SideSetup> sides;                     // by side index
  bool greatest = false;
};

/** Build a fresh game from the original data files. */
std::unique_ptr<Game> newGame(const std::string& dataDir, const std::string& scenario,
                              const NewGameOptions& opts = NewGameOptions());

/** Every army standing on a tile (in transit armies are nowhere). */
std::vector<Army*> armiesAt(const Game& g, int x, int y);
/** The city standing on a tile, anywhere in its 2x2 footprint. */
City* cityAt(const Game& g, int x, int y);
/** Give a city to a side (NONE for neutral) and drop the cached cost grids. */
void setCityOwner(Game& g, City& city, int sideIndex);
std::vector<City*> sideCities(const Game& g, const Side& side);
std::vector<Army*> sideArmies(const Game& g, const Side& side);
/** Items carried by a side's heroes, summed by effect type. */
int itemBonus(const Game& g, const Side& side, int itemType);
/** A tile in the city with room for another army: the four tiles of its
 *  footprint in order, then -- if `spill` -- up to 20 random tries within one
 *  step. {NONE, NONE} when there is none (FUN_6f8c_0bff). */
std::pair<int, int> freeTileIn(Game& g, const City& city, bool spill);
/** docs/rules.md > Start of a side's turn, step 4. */
int income(const Game& g, const Side& side);
int upkeep(const Game& g, const Side& side);

/** Is (x, y) an encampment? */
bool towerAt(const Game& g, int x, int y);
/** Take the tower flag off every tile nobody stands on any more. */
void tidyTowers(Game& g);
/** Is one of the side's heroes carrying a double-movement item on this tile? */
bool heroWithDoubleMoveAt(const Game& g, const Side& side, int x, int y);

/** Run the start of `side`'s turn; false if it has no turn to play: a
 *  computer side left with no city (8cc6:0000) does nothing more until the
 *  round's end puts it out. A human's turn (8cc6:0259) goes on regardless. */
bool startTurn(Game& g, Side& side);
/** Hand the turn to the next living side, starting a new game turn when the
 *  list wraps. Returns the side now to play, or null if the game is over. */
Side* endTurn(Game& g);
/** Start the very first turn of a new game. */
Side* begin(Game& g);

/** The signpost on a tile, if there is one. */
Sign* signAt(Game& g, int x, int y);
/** Order > Disband (1b62:076a). */
void disband(Game& g, Side& side, const std::vector<Army*>& armies);
/** Gold taken with a city won from another side. */
int loot(const Game& g, const Side* loser);
/** Fight for a tile, deciding it and nothing more (combat_resolve, 67cc:08a6). */
Battle decideAttack(Game& g, const std::vector<Army*>& stack, int x, int y);
/** Fight for a tile and apply the outcome at once. */
Battle resolveAttack(Game& g, const std::vector<Army*>& stack, int x, int y);
/** Let a decided fight take effect (after_battle, 67cc:0a6b). Once only. */
Battle& applyAttack(Game& g, Battle& result);

/** Raise a side's diplomatic score. */
void addDiploScore(Game& g, Side& side, int n);
/** Keep the city as it is (city_occupy, 63fa:03fe): ask the quest. */
std::shared_ptr<QuestResult> occupy(Game& g, Side& side, City& city, const std::vector<Army*>& stack);
/** What pillaging or sacking took: the gold and the types lost. */
struct Spoils {
  int gold = 0;
  std::vector<std::pair<int, int>> lost;   // type, gold
};
/** Strip the most expensive production type for gold (63fa:0886). */
Spoils pillage(Game& g, Side& side, City& city, const std::vector<Army*>& stack);
/** Strip every production type but the cheapest (63fa:0941). */
Spoils sack(Game& g, Side& side, City& city, const std::vector<Army*>& stack);
/** Burn the city to the ground. */
void raze(Game& g, Side& side, City& city, const std::vector<Army*>& stack, bool ownCity = false);
/** Order > Resign (7721:1608). */
void resign(Game& g, Side& side);

/** Has this side seen the tile? Always true with Hidden Map off. */
bool seen(const Game& g, int side, int x, int y);
/** Uncover the tiles around (x, y): 2 out for a flier or on a city, else 1
 *  (FUN_8611_1298). Returns how many were new. */
int reveal(Game& g, int side, int x, int y, bool flying);
extern const int FOG_DX[8], FOG_DY[8];
extern const int FOG_CELL[256];
constexpr int FOG_NONE = 255, FOG_BLACK = 14;
/** The HIDDEN.PCK cell over (x, y) for this side, or NONE for a tile it has
 *  seen (or one whose edge has no cell). */
int fogCell(const Game& g, int side, int x, int y);
/** The start of a turn tidies the hidden map (8611:1558). */
void tidyExplored(Game& g, int side);
/** Uncover everything a side can already see: its cities and its armies. */
void revealStart(Game& g, const Side& side);

/** The surrender offer taken (8065:1e4e). */
void acceptSurrender(Game& g, Side& side);
/** STRING.DAT group 11 holds five ways of saying a side is gone. */
constexpr int FALLEN_LINES = 5;
/** Every side in the game left without a city is put out of it, in side
 *  order (8065:18ab); told in a box while a human plays or when the side was
 *  a human's, otherwise in the status bar. */
std::vector<Fallen> eliminateFallen(Game& g);
/** The round's end (8065:17f6): the fallen are put out, then the end is
 *  looked for. Leaves the finding in g.ending. */
Ending endRound(Game& g);
/** Is the game over, or nearly? Run at the round's end, once the fallen are
 *  out (end_game_check, 8065:1aed). */
Ending checkEnd(Game& g);

/** Search whatever the stack is standing on, if anything. */
std::shared_ptr<SearchResult> searchHere(Game& g, const std::vector<Army*>& stack, bool human);
/** A line of prose for a search result. */
std::string describeSearch(const SearchResult* r);

/** Choose what a city builds: a slot index from 0, or NONE to stop. */
void setProduction(Game& g, City& city, int slotIndex);
/** The cities sending what they build to `dest` (7087:0410) -- a city, or
 *  (dest null) the cities of `side` sending theirs to its standard. */
std::vector<City*> vectoredTo(Game& g, const City* dest, const Side* side = nullptr);
/** Where the side's standard is planted, or {NONE, NONE} (7563:0b47). */
std::pair<int, int> standardAt(const Game& g, const Side& side);
/** Hero > Plant Flag (7563:09f7). True if it was planted. */
bool plantFlag(Game& g, Side& side, Army* h);
/** Send what a city builds to its side's planted standard. */
bool vectorToStandard(Game& g, City& city, const Side& side);
/** Send what a city builds to another city; null (or itself) stops it. */
bool vector(Game& g, City& city, City* destCity);
/** The city nearest a tile by map_distance, the first of equals (828e:04fa). */
City* nearestCity(Game& g, int x, int y, const Side* side = nullptr, const Side* seer = nullptr);
/** The army types a side may buy for a city, in ARMYTYPE.DAT's own order. */
std::vector<const ArmyType*> buyableTypes(const Game& g);
/** Why a type cannot be bought for this city right now, or "". */
std::string cannotBuy(const Game& g, const Side& side, const City& city, const ArmyType& a);
/** The slot the Build Production screen starts on (7087:0978), from 0. */
int buySlot(const City& city);
/** Buy an army type into a city's production slot `n` (buy_production_type, 7087:1299). */
void buyProduction(Game& g, Side& side, City& city, int n, int typeId);
/** Put a city's production back in price order (7087:1544). */
void sortProduction(City& city);
/** Give a city a new name (Rename, 7204:204d). */
void renameCity(Game& g, City& city, const std::string& name);

}  // namespace w2::game
