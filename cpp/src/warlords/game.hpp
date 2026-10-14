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

/** The side whose turn it is while the game is set up: the scenario's own
 *  current player (.SCN 0x110), 0 in every shipped scenario. Its Enhanced flag
 *  is the one best_production_for and city_produce_army read. */
constexpr int SETUP_PLAYER = 0;

/** Every army standing on a tile (in transit armies are nowhere), the army
 *  table walked from the top down as armies_at_tile (1b62:007d) walks it. */
std::vector<Army*> armiesAt(const Game& g, int x, int y);
/** The city standing on a tile, anywhere in its 2x2 footprint. */
City* cityAt(const Game& g, int x, int y);
/** Give a city to a side (NONE for neutral) and drop the cached cost grids. */
void setCityOwner(Game& g, City& city, int sideIndex);
std::vector<City*> sideCities(const Game& g, const Side& side);
std::vector<Army*> sideArmies(const Game& g, const Side& side);
/** Items carried by a side's heroes, summed by effect type. */
int itemBonus(const Game& g, const Side& side, int itemType);
/** A tile in the city with room for another army (6f8c:0bff): the four tiles
 *  of its footprint in the order (x,y), (x+1,y), (x,y+1), (x+1,y+1)
 *  (DS:0e54/0e5c); then -- if `spill` -- up to 20 random tries within one
 *  step for a tile no army stands on. {NONE, NONE} when there is none, with
 *  the last tile tried in `last`, where hero_recruit puts its hero regardless. */
std::pair<int, int> freeTileIn(Game& g, const City& city, bool spill, std::pair<int, int>* last = nullptr);
/** docs/rules.md > Start of a side's turn, step 4. */
int income(const Game& g, const Side& side);
int upkeep(const Game& g, const Side& side);

/** Is (x, y) an encampment? */
bool towerAt(const Game& g, int x, int y);
/** Take the tower flag off every tile nobody stands on any more. */
void tidyTowers(Game& g);
/** Is one of the side's heroes carrying a double-movement item on this tile? */
bool heroWithDoubleMoveAt(const Game& g, const Side& side, int x, int y);

/** Before every side's turn the side's capital is put where it now is
 *  (67cc:10d7): while more than one side plays and the side holds cities but
 *  not its own capital, the city of its own nearest the capital it had; else
 *  its own capital again. The turn opens centred there. */
void followCapital(Game& g, Side& side);
/** Run the start of `side`'s turn; false if the side has no turn to play.
 *  The prelude every side gets (turn_loop, 8065:2123): its armies in a city
 *  come ashore, its capital follows it. Then a computer's turn
 *  (start_of_turn, 8cc6:0000): diplomacy; a side left with no city does
 *  nothing more until the round's end puts it out; the hero, hired at once
 *  (7563:0301); promotions, gold, production, the movement reset, the
 *  encampments, the quest. A human's (8cc6:0259) goes on regardless: the
 *  movement reset, the encampments and the gold come first, then the quest;
 *  promotions and the hero follow (8cc6:04bd), and the cities build only
 *  once the hero offer is settled (humanProduction). */
bool startTurn(Game& g, Side& side);
/** A human's cities build, once the turn's hero offer is taken or turned down
 *  (8cc6:04bd(1)) -- on turn 1 they do not: the capital's production opens
 *  instead. Each city that built starts its next army (6f8c:06bd). */
void humanProduction(Game& g, Side& side);
/** Is the city building? Its countdown is above 0 (7204:06de shows "none"
 *  otherwise). */
bool producing(const City& c);
/** Hand the turn to the next side in the turn order (next_side, 8065:17f6),
 *  ending the round when the order runs out. Returns the side now to play,
 *  or null if the game is over. */
Side* endTurn(Game& g);
/** Start the very first turn of a new game. */
Side* begin(Game& g);
/** The human who plays next in the round, the side to play included; null
 *  with no human left. While the computers before it play, the screen is
 *  this side's: the original keeps one seen bit per tile, set for its human
 *  only (8611:1298), so a computer's turn shows the human's hidden map. */
Side* nextHuman(Game& g);

/** The signpost on a tile, if there is one. */
Sign* signAt(Game& g, int x, int y);
/** Order > Disband (1b62:076a). */
void disband(Game& g, Side& side, const std::vector<Army*>& armies);
/** Gold taken with a city won from another side. */
int loot(const Game& g, const Side* loser);
/** Fight for a tile, deciding it and nothing more (combat_setup, 6a89:008b,
 *  and combat_resolve, 67cc:08a6); applyAttack makes it take effect. A city
 *  attacked is marked with the attacker's bit (67cc:02b1): a neutral one
 *  that holds out builds from then on (neutralProduction). */
Battle decideAttack(Game& g, const std::vector<Army*>& stack, int x, int y);
/** What an attack costs (67cc:0522): a stack that is not flying and not at
 *  sea, attacking onto water or shore, puts to sea first -- every army that
 *  cannot fly, and has no flight item, at sea with no moves, boats too. Then
 *  every army loses the tile's movement cost, 2 at least, down to 0, and
 *  forgets its goal. */
void chargeAttack(Game& g, const std::vector<Army*>& stack, int x, int y);
/** Armies left on water without a boat drown (auto_ui_armies_sunk,
 *  1b62:0ea0). On a water tile, or a crossing (map word bit 15), every army
 *  of the side's that is not at sea, not a hero and cannot fly is lost --
 *  unless the armies there could fly off together. With x, y only that tile
 *  is looked at, with NONE every tile the side stands on. Returns how many
 *  sank. The original asks it for a human player only: as the turn opens and
 *  as a stack is picked up. */
int sinkStranded(Game& g, Side& side, int x = NONE, int y = NONE);
/** Fight for a tile and apply the outcome at once. */
Battle resolveAttack(Game& g, const std::vector<Army*>& stack, int x, int y);
/** Let a decided fight take effect (after_battle, 67cc:0a6b). Once only;
 *  sets `captured` to the city taken, and `medal` to the army that won one. */
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
/** The round's end (8065:17f6): the sides left without a city are put out
 *  of the game, then the end is looked for, then the neutral cities build.
 *  `last` is the side that played last: its gold and Enhanced flag are the
 *  ones the neutral cities' production reads. Leaves the finding in g.ending. */
Ending endRound(Game& g, Side* last = nullptr);
/** set_city_production (623c:0e5a) for side `who`: refused after turn 5 when
 *  that side has less than the type's cost + 30 in gold. True if set. */
bool setCityProduction(Game& g, const Side* who, City& c, int slot);
/** neutral_production_turn (6bd8:025a), with Neutral Cities at 2 or more: a
 *  neutral city that has beaten off an attack (City::attacked, 67cc:02b1)
 *  counts its build down and, while fewer than 4 armies stand on its top-left
 *  tile, starts the best balanced type (purpose 2). The last city first. */
void neutralProduction(Game& g, const Side* who);
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
