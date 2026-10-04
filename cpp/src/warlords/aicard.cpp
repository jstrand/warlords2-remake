#include "warlords/aicard.hpp"

#include "util/util.hpp"
#include "warlords/bytes.hpp"

namespace w2::aicard {

std::string path(const std::string& dataDir, int level, int number, const std::string& ext) {
  static const char* LETTERS[3] = {"K", "L", "W"};   // "KLW", 7bab:21c1
  const char* l = level >= 0 && level < 3 ? LETTERS[level] : "W";
  return fmt("%s/CARDS/%s%03d.%s", dataDir.c_str(), l, number, ext.c_str());
}

std::optional<Card> load(const std::string& dataDir, int level, int number) {
  auto f = readFile(path(dataDir, level, number, "CRD"));
  if (!f || f->size() < (size_t)RECORD) return std::nullopt;
  const std::string& s = *f;
  Card c;
  for (int t = 0; t <= 28; t++) c.fightOrder.push_back(u8(s, 0x0e + t));
  c.groups = i16(s, 0x08);
  c.bold = i16(s, 0x0a);
  c.cautious = i16(s, 0x2b);
  c.rebuildType = i16(s, 0x2d);
  c.rebuildTypeRich = i16(s, 0x2f);
  c.rebuildLimit = i16(s, 0x31);
  c.raze = i16(s, 0x45);
  c.sack = i16(s, 0x47);
  c.pillage = i16(s, 0x49);
  c.perCity = i16(s, 0x4b);
  c.bonusHuman = i16(s, 0x4d);
  c.bonusKnight = i16(s, 0x4f);
  c.bonusLord = i16(s, 0x51);
  c.bonusWarlord = i16(s, 0x53);
  c.dieHuman = i16(s, 0x55);
  c.dieKnight = i16(s, 0x57);
  c.dieLord = i16(s, 0x59);
  c.dieWarlord = i16(s, 0x5b);
  c.poor = i16(s, 0x5f);
  c.early = i16(s, 0x61);
  c.humanShare = i16(s, 0x63);
  c.solidarity = i16(s, 0x65);
  return c;
}

std::optional<std::pair<std::string, std::vector<std::string>>> describe(const std::string& dataDir, int level, int number) {
  auto s = readFile(path(dataDir, level, number, "DSC"));
  if (!s) return std::nullopt;
  std::vector<std::string> lines;
  std::string cur;
  std::string text = *s + "\n";
  for (char ch : text) {
    if (ch == '\n') {
      if (!cur.empty() && cur.back() == '\r') cur.pop_back();
      lines.push_back(cur);
      cur.clear();
    } else {
      cur += ch;
    }
  }
  std::string name = lines.empty() ? "" : lines.front();
  if (!lines.empty()) lines.erase(lines.begin());
  while (!lines.empty() && lines.back().empty()) lines.pop_back();
  return std::make_pair(name, lines);
}

int count(const std::string& dataDir, int level) {
  int n = 0;
  while (n < 100 && fileExists(path(dataDir, level, n, "CRD"))) n++;
  return n;
}

}  // namespace w2::aicard
