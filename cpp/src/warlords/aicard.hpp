// The computer players' character cards: CARDS/K|L|W nnn.CRD and .DSC.
//
// docs/formats/crd.md. A computer side's level picks the deck (K Knight,
// L Lord, W Warlord) and .SCN 0x00e0 + 2*side the card; at game start the
// card overwrites the level's built-in AI settings (59bf:0d7b).
#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace w2::aicard {

constexpr int RECORD = 0x67;   // 103 bytes

struct Card {
  int groups = 0, bold = 0;
  std::vector<int> fightOrder;
  int cautious = 0, rebuildType = 0, rebuildTypeRich = 0, rebuildLimit = 0;
  int raze = 0, sack = 0, pillage = 0, perCity = 0;
  int bonusHuman = 0, bonusKnight = 0, bonusLord = 0, bonusWarlord = 0;
  int dieHuman = 0, dieKnight = 0, dieLord = 0, dieWarlord = 0;
  int poor = 0, early = 0, humanShare = 0, solidarity = 0;
};

/** The file name of a card: CARDS\%c%03d.%s. */
std::string path(const std::string& dataDir, int level, int number, const std::string& ext = "CRD");
/** Read a card, or none when the file is missing or short. */
std::optional<Card> load(const std::string& dataDir, int level, int number);
/** A card's name and description lines, from its .DSC, or none. */
std::optional<std::pair<std::string, std::vector<std::string>>> describe(const std::string& dataDir, int level, int number);
/** How many cards a level has on disk, 000 up to the first missing one. */
int count(const std::string& dataDir, int level);

}  // namespace w2::aicard
