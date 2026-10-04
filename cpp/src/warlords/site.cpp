#include "warlords/site.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <map>

#include "warlords/armytype.hpp"
#include "warlords/combat.hpp"
#include "warlords/game.hpp"
#include "warlords/hero.hpp"
#include "warlords/history.hpp"
#include "warlords/move.hpp"
#include "warlords/quest.hpp"
#include "warlords/rules.hpp"
#include "warlords/scn.hpp"

namespace w2::site {

const char* const CONTENT_NAMES[6] = {"empty", "temple", "a magic item", "a sage", "gold", "allies"};

// The content tables, by band. 3 = sage, 4 = gold, 5 = allies.
static const std::vector<int>& content(const std::string& band) {
  static const std::vector<int> rich = {5, 5, 4}, far = {3, 4, 5, 3, 4}, near = {3, 4, 5};
  return band == "rich" ? rich : band == "near" ? near : far;
}
// Ally army types, by band (army type ids).
static const std::vector<int>& allyTypes(const std::string& band) {
  static const std::vector<int> rich = {25, 23, 27, 19},   // Dragons, Wizards, Devils, Archons
      far = {24, 20, 26},                                // Ghosts, Giant Worms, Demons
      near = {22, 20};                                   // Elementals, Giant Worms
  return band == "rich" ? rich : band == "near" ? near : far;
}

bool itemReserved(int type, int value) {
  return type == rules::ITEM_FLIGHT || type == rules::ITEM_DOUBLE_MOVE || type == rules::ITEM_STANDARD ||
         (type == rules::ITEM_COMMAND && value >= 2);
}

void fillItemPool(Game& g, int reserved) {
  if (!g.map->itemPool) return;
  const auto& pool = *g.map->itemPool;
  std::map<int, Item*> byIndex;
  std::map<int, bool> used;
  for (auto& it : g.map->items) byIndex[it.index] = &it;
  for (int idx = 8; idx <= 21; idx++) {
    auto f = byIndex.find(idx);
    if (f == byIndex.end()) continue;
    Item* item = f->second;
    bool want = idx < 8 + reserved;
    std::vector<int> choices;
    for (size_t i = 0; i < pool.size(); i++) {
      if (!used[(int)i] && itemReserved(pool[i]) == want) choices.push_back((int)i);
    }
    int k = g.rng.pickIndex(choices.size());
    if (k >= 0) {
      int pick = choices[k];
      used[pick] = true;
      item->name = pool[pick].name;
      item->type = pool[pick].type;
      item->value = pool[pick].value;
    }
  }
}

void markRich(Game& g) {
  std::vector<Site*> ruins;
  for (auto& s : g.map->sites) {
    s.rich = false;
    s.revealed = 0xff;
    if (s.type != TEMPLE) ruins.push_back(&s);
  }
  int want = (int)g.map->sites.size() * RICH_SHARE / 10;
  g.rng.shuffle(ruins);
  for (int i = 0; i < std::min(want, (int)ruins.size()); i++) {
    ruins[i]->rich = true;
    if (g.map->options.quests != 0) ruins[i]->revealed = 0;
  }
}

static std::string band(const Site& s, const std::vector<City*>& capitals) {
  if (s.rich) return "rich";
  for (City* c : capitals) {
    if (std::max(std::abs(s.x - c->x), std::abs(s.y - c->y)) < CAPITAL_RANGE) return "near";
  }
  return "far";
}

void setup(Game& g) {
  std::vector<City*> capitals;
  for (Side* s : g.sides) if (s->capital) capitals.push_back(s->capital);

  markRich(g);
  int temples = 0;
  for (auto& s : g.map->sites) {
    s.content = s.type == TEMPLE ? TEMPLE : EMPTY;
    if (s.content == TEMPLE) { s.templeIndex = temples; temples++; }
    s.item = NONE; s.guardian = NONE; s.allyType = NONE; s.searched = false;
    s.band = band(s, capitals);
  }

  int nSites = (int)g.map->sites.size();
  int reserved = nSites * 2 / 10;
  fillItemPool(g, reserved);

  int bandHi = reserved;
  int bandLo = std::min(g.rng.dice(2, 3, 1), bandHi);
  int last = std::min(22, nSites / 3 + g.rng.dice(1, 5, -3) + 8);

  std::vector<Site*> free;
  for (auto& s : g.map->sites) if (s.content == EMPTY) free.push_back(&s);
  g.rng.shuffle(free);

  std::map<int, Item*> byIndex;
  for (auto& it : g.map->items) {
    byIndex[it.index] = &it;
    it.status = 0;
  }

  for (int idx = 8; idx <= last - 1; idx++) {
    auto f = byIndex.find(idx);
    // the band between bandLo and bandHi is held back, probably for quests
    if (f != byIndex.end() && !(idx >= 8 + bandLo && idx < 8 + bandHi)) {
      Item* item = f->second;
      bool want = itemReserved(*item);
      for (size_t i = 0; i < free.size(); i++) {
        Site* s = free[i];
        if (s->rich == want) {
          s->content = ITEM;
          s->item = idx;
          item->status = 2;                        // hidden in a ruin
          free.erase(free.begin() + i);
          break;
        }
      }
    }
  }

  for (auto& s : g.map->sites) {
    if (s.content == EMPTY) s.content = g.rng.pick(content(s.band), 0);
    if (s.content == TEMPLE) {
      s.guardian = NONE;
    } else if (s.content == ALLIES) {
      s.allyType = g.rng.pick(allyTypes(s.band), 0);
      s.guardian = g.rng.dice(1, 9, 0);
    } else {
      s.guardian = g.rng.dice(1, 9, 0);
    }
  }
  g.map->siteAt.assign(g.map->width * g.map->height, nullptr);
  for (auto& s : g.map->sites) g.map->siteAt[s.y * g.map->width + s.x] = &s;
}

static Army* heroIn(const std::vector<Army*>& stack) {
  for (Army* a : stack) if (a->type == armytype::HERO) return a;
  return nullptr;
}

bool survivesGuardian(Game& g, const Army& h, const std::vector<Army*>& stack, int monsterStrength) {
  int margin = 90 + 5 * (h.strength + combat::battleItems(h) - monsterStrength) + 3 * (int)stack.size();
  return g.rng.dice(1, 100, 0) <= margin;
}

int guardianStrength(const Game& g, const Site& s) {
  if (s.guardian < 0 || s.guardian > 9) return 0;
  const auto& m = g.map->monsters[s.guardian];
  return m ? m->strength : 0;
}

int bless(Game& g, const Site& s, const std::vector<Army*>& stack) {
  if ((s.templeIndex == NONE ? 0 : s.templeIndex) >= BLESSING_TEMPLES) return 0;
  int bit = s.templeIndex;
  int blessed = 0;
  for (Army* a : stack) {
    if (!a->blessings.count(bit)) {
      a->blessings.insert(bit);
      a->strength = std::min(9, a->strength + 1);
      blessed++;
      if (a->type == armytype::HERO) hero::addExperience(g, a, 1);
    }
  }
  return blessed;
}

static const Monster* monsterOf(const Game& g, int i) {
  if (i < 0 || i > 9 || !g.map->monsters[i]) return nullptr;
  return &*g.map->monsters[i];
}

std::shared_ptr<SearchResult> search(Game& g, const std::vector<Army*>& stack, int x, int y, bool human) {
  if (g.map->siteAt.empty() || x < 0 || y < 0) return nullptr;
  Site* s = g.map->siteAt[y * g.map->width + x];
  if (!s || s->searched) return nullptr;
  Army* h = heroIn(stack);
  auto out = std::make_shared<SearchResult>();
  out->site = s;

  if (s->content == TEMPLE) {
    out->kind = "temple";
    if (human) { out->hero = h; return out; }
    out->blessed = bless(g, *s, stack);
    if (h && g.map->options.quests != 0) {
      Side* side = g.map->side(h->owner);
      if (side) out->assigned = quest::assign(g, *side, h);
    }
    return out;
  }

  if (!h) { out->kind = "no hero"; return out; }
  s->searched = true;
  out->hero = h;

  if (s->content == SAGE) {
    hero::addExperience(g, h, 3);
    history::deed(g, g.map->side(h->owner), history::FINDS, history::SAGE, 0, h->name);  // 6536:013c
    out->kind = "sage";
    return out;
  }

  hero::addExperience(g, h, 3);

  const Monster* beaten = nullptr;
  if (s->guardian != NONE && s->guardian > 0) {
    int str = guardianStrength(g, *s);
    beaten = monsterOf(g, s->guardian);
    if (!survivesGuardian(g, *h, stack, str)) {
      hero::dropItems(g, h, x, y);
      history::deed(g, g.map->side(h->owner), history::KILLED, history::SEARCHING, 0, h->name);  // 6536:02ef
      g.remove(h);
      out->kind = "killed";
      out->monster = monsterOf(g, s->guardian);
      return out;
    }
  }
  out->guardian = beaten;

  if (s->content == ITEM) {
    Item* found = nullptr;
    for (auto& it : g.map->items) if (it.index == s->item) found = &it;
    if (found && human) {
      found->status = 1; found->x = x; found->y = y;    // on the ground, to take
    } else if (found) {
      found->status = 3;                              // carried
      h->items.push_back(found);
    }
    Side* side = g.map->side(h->owner);
    if (found) history::deed(g, side, history::FINDS, found->index, 0, h->name);   // 6536:0571
    if (side) {
      quest::Event ev;
      ev.hero = h;
      out->quest = quest::event(g, *side, "item", ev);
    }
    out->kind = "item";
    out->item = found;
    return out;
  } else if (s->content == GOLD) {
    int gold = s->rich ? g.rng.dice(3, 1000, 1000) : g.rng.dice(3, 500, 500);
    Side* side = g.map->side(h->owner);
    if (side) side->gold += gold;
    out->kind = "gold";
    out->gold = gold;
    return out;
  } else if (s->content == ALLIES) {
    const ArmyType* type = g.types.byId(s->allyType);
    if (!type) type = g.types.byId(armytype::SCOUTS);
    int n = s->rich ? g.rng.dice(1, 2, 2) : g.rng.dice(1, 2, 0);
    for (int k = 0; k < n; k++) {
      int ax = x, ay = y;
      if ((int)game::armiesAt(g, ax, ay).size() >= rules::MAX_STACK) {
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && ny >= 0 && nx < g.map->width && ny < g.map->height &&
                (int)game::armiesAt(g, nx, ny).size() < rules::MAX_STACK &&
                move::COST[scn::terrainAt(*g.map, nx, ny)] != 0) {
              ax = nx; ay = ny;
            }
          }
        }
      }
      if ((int)game::armiesAt(g, ax, ay).size() < rules::MAX_STACK) {
        out->armies.push_back(g.add(hero::newAlly(type, ax, ay, h->owner, h->homeCity)));
      }
    }
    history::deed(g, g.map->side(h->owner), history::FINDS, history::ALLIES, 0, h->name);  // 6536:07a1
    out->kind = "allies";
    out->type = type;
    return out;
  }
  out->kind = "empty";
  return out;
}

static int sageDistance(const Site& s, int hx, int hy) {
  double dx = s.x - hx, dy = s.y - hy;
  return (int)std::floor(std::sqrt(dx * dx + dy * dy));   // 2012:1199
}

bool shownTo(const Site& s, const Side& side) { return ((s.revealed >> side.index) & 1) == 1; }

static bool unshown(const Side& side, const Site& s, int hx, int hy) {
  return s.rich && !s.searched && !shownTo(s, side) && sageDistance(s, hx, hy) < SAGE_RANGE;
}

std::vector<SageEntry> sageList(Game& g, const Side& side, int hx, int hy) {
  std::vector<SageEntry> list;
  bool gold = false, allies = false;
  for (auto& s : g.map->sites) {
    if (unshown(side, s, hx, hy)) {
      if (s.content == GOLD && !gold) {
        gold = true;
        list.push_back(SageEntry{"gold", "Gold", nullptr});
      } else if (s.content == ALLIES && !allies) {
        allies = true;
        list.push_back(SageEntry{"allies", "Allies", nullptr});
      } else if (s.content == ITEM) {
        for (auto& it : g.map->items) {
          if (it.index == s.item) list.push_back(SageEntry{"item", it.name, &it});
        }
      }
    }
  }
  return list;
}

Site* sageShow(Game& g, const Side& side, const SageEntry& entry, int hx, int hy) {
  Site* found = nullptr;
  int best = NONE;
  for (auto& s : g.map->sites) {
    if (entry.kind == "item") {
      if (s.content == ITEM && entry.item && s.item == entry.item->index) { found = &s; break; }
    } else if (unshown(side, s, hx, hy) && s.content == (entry.kind == "gold" ? GOLD : ALLIES)) {
      int d = sageDistance(s, hx, hy);
      if (best == NONE || d < best) { found = &s; best = d; }
    }
  }
  if (!found) return nullptr;
  if (!shownTo(*found, side)) found->revealed = found->revealed + (1 << side.index);
  game::reveal(g, side.index, found->x, found->y, false);
  return found;
}

int sageGem(Game& g, Side& side) {
  int n = g.rng.dice(3, 500, 500);
  side.gold += n;
  return n;
}

std::array<int, 4> sageMap(Game& g, const Side& side, int cx, int cy) {
  int x0 = std::max(0, cx - g.rng.dice(1, 5, 8));
  int y0 = std::max(0, cy - g.rng.dice(1, 5, 8));
  int w = g.rng.dice(1, 10, 15), h = g.rng.dice(1, 10, 15);
  if (x0 + w >= g.map->width) w = g.map->width - x0 - 1;
  if (y0 + h >= g.map->height) h = g.map->height - y0 - 1;
  for (int x = x0; x < x0 + w; x++)
    for (int y = y0; y < y0 + h; y++) game::reveal(g, side.index, x, y, false);
  return {x0, y0, w, h};
}

}  // namespace w2::site
