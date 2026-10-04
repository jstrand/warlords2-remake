#include "warlords/scn.hpp"

#include <stdexcept>

#include "util/util.hpp"
#include "warlords/bytes.hpp"

namespace w2 {

const std::vector<std::string>& Options::names() {
  static const std::vector<std::string> n = {
      "neutralCities", "diplomacy", "quests", "randomTurns", "hiddenMap", "intenseCombat",
      "quickStart", "viewEnemies", "militaryAdvisor", "tutorial", "viewProduction"};
  return n;
}

int* Options::field(const std::string& name) {
  if (name == "neutralCities") return &neutralCities;
  if (name == "diplomacy") return &diplomacy;
  if (name == "quests") return &quests;
  if (name == "randomTurns") return &randomTurns;
  if (name == "hiddenMap") return &hiddenMap;
  if (name == "intenseCombat") return &intenseCombat;
  if (name == "quickStart") return &quickStart;
  if (name == "viewEnemies") return &viewEnemies;
  if (name == "militaryAdvisor") return &militaryAdvisor;
  if (name == "tutorial") return &tutorial;
  if (name == "viewProduction") return &viewProduction;
  return nullptr;
}

namespace scn {

namespace {
const int SIDE_NAMES = 0, SIDE_STRIDE = 20;
// 0xa0 is the side's own colour, 0xb0 the colour it outlines things in.
const int SIDE_COLOURS = 0xa0, SIDE_EDGES = 0xb0;
const int SIDE_RECS = 387, SIDE_REC_STRIDE = 20;
const int LEVELS = 0xc0, CONTROLLERS = 0xd0, ENHANCED = 0xf0;
const int OBSERVE = 0x147, CARDS = 0xe0;
const int MONSTER_STRENGTH = 0x1007;
const int FIGHT_ORDER = 0x60b, FIGHT_ROWS = 9, FIGHT_TYPES = 29;
const int COMBAT_CAP = 0x112, DIPLO_SCORE = 0x10e3;
const int TERRAIN_TABLE = 0x710, TERRAIN_COUNT = 255;
const int SITES_COUNT = 0x80f, SITES = 0x811, SITE_STRIDE = 31;
const int ITEMS = 3305, ITEM_STRIDE = 29, N_ITEMS = 22;
const int MONSTERS = 3943, MONSTER_STRIDE = 16, N_MONSTERS = 10;
const int CITIES_COUNT = 5499, CITIES = 5501, CITY_STRIDE = 65;

// Option words. They are NOT in menu order; see docs/rules.md > Game setup.
const std::pair<const char*, int> OPTIONS[] = {
    {"neutralCities", 0x11a}, {"diplomacy", 0x11c}, {"quests", 0x11e},
    {"randomTurns", 0x122}, {"hiddenMap", 0x124}, {"intenseCombat", 0x126},
    {"quickStart", 0x128}, {"viewEnemies", 0x12a}, {"militaryAdvisor", 0x12c},
    {"tutorial", 0x12e}, {"viewProduction", 0x132},
};

std::optional<std::vector<std::string>> lines(const std::string& path) {
  auto t = readFile(path);
  if (!t) return std::nullopt;
  return splitLines(*t);
}

long leadingInt(const std::string& s) {
  size_t i = 0;
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
  bool neg = false;
  if (i < s.size() && (s[i] == '-' || s[i] == '+')) neg = s[i++] == '-';
  long v = 0;
  bool any = false;
  while (i < s.size() && s[i] >= '0' && s[i] <= '9') { v = v * 10 + (s[i++] - '0'); any = true; }
  return any ? (neg ? -v : v) : 0;
}
}  // namespace

std::optional<std::vector<PoolItem>> loadItemPool(const std::string& path) {
  auto ls = lines(path);
  if (!ls || ls->empty()) return std::nullopt;
  long count = leadingInt((*ls)[0]);
  std::vector<PoolItem> pool;
  for (size_t i = 1; i < std::min(ls->size(), (size_t)count + 1); i++) {
    const std::string& line = (*ls)[i];
    PoolItem p;
    std::string nm = line.substr(0, std::min<size_t>(20, line.size()));
    for (auto& c : nm) if (c == '_') c = ' ';
    p.name = trimRight(nm);
    p.type = line.size() > 21 ? (int)leadingInt(line.substr(21, 1)) : 0;
    p.value = line.size() > 23 ? (int)leadingInt(line.substr(23, 1)) : 0;
    pool.push_back(p);
  }
  return pool;
}

std::map<int, std::array<std::string, 3>> loadDescriptions(const std::string& path) {
  std::map<int, std::array<std::string, 3>> out;
  auto ls = lines(path);
  if (!ls) return out;
  for (auto& line : *ls) {
    // ^#(\d+)\|(.*)$
    if (line.size() < 2 || line[0] != '#') continue;
    size_t i = 1;
    long n = 0;
    bool any = false;
    while (i < line.size() && line[i] >= '0' && line[i] <= '9') { n = n * 10 + (line[i++] - '0'); any = true; }
    if (!any || i >= line.size() || line[i] != '|') continue;
    std::string rest = line.substr(i + 1);
    std::vector<std::string> parts;
    std::string cur;
    for (char c : rest) {
      if (c == '|') { parts.push_back(cur); cur.clear(); }
      else cur += c;
    }
    std::array<std::string, 3> d;
    for (int k = 0; k < 3; k++) d[k] = k < (int)parts.size() ? parts[k] : "";
    out[(int)n] = d;
  }
  return out;
}

std::vector<Sign> loadSigns(const std::string& path) {
  std::vector<Sign> out;
  auto s = readFile(path);
  if (!s) return out;
  int n = u16(*s, 0);
  for (int i = 0; i < n; i++) {
    size_t o = 2 + 104 * i;
    if (o + 104 > s->size()) break;
    Sign sg;
    sg.index = i;
    sg.x = u16(*s, o);
    sg.y = u16(*s, o + 2);
    sg.lines = {cstr(*s, o + 4, 50), cstr(*s, o + 54, 50)};
    out.push_back(sg);
  }
  return out;
}

std::unique_ptr<Map> load(const std::string& dir, const std::string& name) {
  std::string base = dir + "/" + name;
  std::string s = mustRead(base + ".SCN");
  if (s.size() != 12001) throw std::runtime_error(fmt("unexpected .SCN size: %d", (int)s.size()));
  auto m = std::make_unique<Map>();
  m->name = name;

  for (auto& [k, off] : OPTIONS) *m->options.field(k) = u16(s, off);

  for (int i = 0; i < 8; i++) {
    int o = SIDE_RECS + SIDE_REC_STRIDE * i;
    Side& sd = m->sides[i];
    sd.index = i;
    sd.name = cstr(s, SIDE_NAMES + SIDE_STRIDE * i, SIDE_STRIDE);
    sd.colour = u16(s, SIDE_COLOURS + 2 * i);
    sd.edge = u16(s, SIDE_EDGES + 2 * i);
    sd.gold = u16(s, o + 2);
    sd.capX = u16(s, o + 6);
    sd.capY = u16(s, o + 8);
    sd.computer = u16(s, CONTROLLERS + 2 * i) != 0;
    sd.level = u16(s, LEVELS + 2 * i);
    sd.enhanced = u16(s, ENHANCED + 2 * i) != 0;
    sd.observe = u16(s, OBSERVE + 2 * i) != 0;
    sd.diploScore = u16(s, DIPLO_SCORE + 2 * i);
    sd.card = u16(s, CARDS + 2 * i);
  }

  int nCities = u16(s, CITIES_COUNT);
  m->cities.resize(nCities);
  m->cityAt.assign(MAP_W * MAP_H, nullptr);
  m->cityTile.assign(MAP_W * MAP_H, nullptr);
  for (int i = 0; i < nCities; i++) {
    int o = CITIES + CITY_STRIDE * i;
    City& c = m->cities[i];
    c.index = i;
    c.x = u16(s, o);
    c.y = u16(s, o + 2);
    c.name = cstr(s, o + 4, 16);
    c.income = u8(s, o + 42);
    for (int k = 0; k < 4; k++) {
      int t = u8(s, o + 22 + k);
      // the game removes Navy (type 5) from every city at start
      if (t != 255 && t != 5) c.produces.push_back(t);
    }
    c.defence = c.produces.size() >= 3 ? 2 : 1;
    m->cityAt[c.y * MAP_W + c.x] = &c;
  }
  // A city covers a 2x2 footprint; cityAt keys the top-left, cityTile every tile.
  for (auto& c : m->cities) {
    for (int dx = 0; dx <= 1; dx++) {
      for (int dy = 0; dy <= 1; dy++) {
        int k = (c.y + dy) * MAP_W + (c.x + dx);
        if (k >= 0 && k < MAP_W * MAP_H) m->cityTile[k] = &c;
      }
    }
  }
  // Ownership is derived: at scenario start a side owns only its capital.
  for (auto& sd : m->sides) {
    int k = sd.capY * MAP_W + sd.capX;
    City* c = k >= 0 && k < MAP_W * MAP_H ? m->cityAt[k] : nullptr;
    sd.inUse = c != nullptr;
    sd.capital = c;
    if (c) c->owner = &sd;
  }

  // fight order: 29 bytes per player, row 8 for neutral
  m->fightOrder.assign(FIGHT_ROWS, std::vector<int>(FIGHT_TYPES, 0));
  for (int row = 0; row < FIGHT_ROWS; row++)
    for (int t = 0; t < FIGHT_TYPES; t++) m->fightOrder[row][t] = u8(s, FIGHT_ORDER + row * FIGHT_TYPES + t);

  int nSites = u16(s, SITES_COUNT);
  for (int i = 0; i < nSites; i++) {
    int o = SITES + SITE_STRIDE * i;
    Site st;
    st.index = i;
    st.x = u16(s, o);
    st.y = u16(s, o + 2);
    st.name = cstr(s, o + 4, 20);
    st.type = u16(s, o + 24);
    m->sites.push_back(st);
  }

  for (int i = 0; i < N_ITEMS; i++) {
    int o = ITEMS + ITEM_STRIDE * i;
    std::string nm = cstr(s, o, 20);
    if (!nm.empty()) {
      Item it;
      it.index = i;
      it.name = nm;
      it.type = u8(s, o + 20);
      it.value = u8(s, o + 21);
      m->items.push_back(it);
    }
  }

  for (int i = 0; i < N_MONSTERS; i++) {
    std::string nm = cstr(s, MONSTERS + MONSTER_STRIDE * i, 12);
    // keyed by slot: a site's guardian byte indexes this directly
    if (!nm.empty()) m->monsters[i] = Monster{i, nm, u16(s, MONSTER_STRENGTH + 2 * i)};
  }

  // tile index -> terrain type id, the same table WARLORD2.EXE reads
  for (int i = 0; i < TERRAIN_COUNT; i++) m->terrainType[i] = u8(s, TERRAIN_TABLE + i);

  // terrain grid + road overlay; bit 15 of a map word marks a crossing
  std::string mp = mustRead(base + ".MAP");
  m->tiles.resize(MAP_W * MAP_H);
  m->crossing.resize(MAP_W * MAP_H);
  for (int i = 0; i < MAP_W * MAP_H; i++) {
    int w = u16(mp, i * 2);
    m->tiles[i] = w % 0x8000;
    m->crossing[i] = w >= 0x8000;
  }
  m->roads = mustRead(base + ".RD");
  if ((int)m->roads.size() < MAP_W * MAP_H) m->roads.resize(MAP_W * MAP_H, '\0');

  m->itemPool = loadItemPool(base + ".ITM");
  m->cityText = loadDescriptions(base + ".CTY");
  m->siteText = loadDescriptions(base + ".SPC");
  m->signs = loadSigns(base + ".SGN");
  m->combatCap = u16(s, COMBAT_CAP);
  return m;
}

int cityTileBase(const City& city) {
  if (city.razed) return 0xa0 + 2 * (city.razedBy == NONE ? 0 : city.razedBy);
  int o = city.ownerIndex;
  if (o == NONE) return 96;          // the original passes owner -1
  if (o < 6) return 0x62 + 2 * o;
  return 0x80 + 2 * (o - 6);
}

void setCityTiles(Map& map, const City& city) {
  int t = cityTileBase(city);
  int i = city.y * MAP_W + city.x;
  map.tiles[i] = t;
  map.tiles[i + 1] = t + 1;
  map.tiles[i + MAP_W] = t + 16;
  map.tiles[i + MAP_W + 1] = t + 17;
}

void refreshCityTiles(Map& map) {
  for (auto& c : map.cities) setCityTiles(map, c);
}

}  // namespace scn
}  // namespace w2
