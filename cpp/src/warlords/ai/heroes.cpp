#include "warlords/ai/heroes.hpp"

#include <vector>

#include "warlords/ai/cities.hpp"
#include "warlords/ai/core.hpp"
#include "warlords/ai/moves.hpp"
#include "warlords/quest.hpp"
#include "warlords/site.hpp"

namespace w2::ai::heroes {

using namespace core;

namespace {
bool blessed(const Army* h, const Site& s) { return s.templeIndex != NONE && h->blessings.count(s.templeIndex); }

// Is a site still worth the hero's while (6087:1637)?
bool worth(const Game& g, const Side& side, const Site& s, const Army* h) {
  int dd = dist(s.x, s.y, h->x, h->y);
  int reach = g.turn * 2 + (s.rich ? 15 : 3);
  return dd < reach && explored(g, side.index, s.x, s.y) && siteOpen(s);
}

// Is an item lying where a hero may take it (6087:1736)?
bool lying(const Game& g, const Side& side, const Item& it) {
  if (it.status != 1 || it.x == NONE || it.planted) return false;
  if (!explored(g, side.index, it.x, it.y)) return false;
  City* c = cityAt(g, it.x, it.y);
  return !c || !standing(*c) || c->ownerIndex == side.index;
}

struct Info {
  std::vector<Army*> heroes;
  std::vector<City*> city;
  std::vector<Site*> site;
  Site* quest = nullptr;
  int questD = 100;
  Site* target = nullptr;
  int siteD = 100;
  Item* item = nullptr;
  int itemD = 100;
  City* enemy = nullptr;
  int enemyD = 20;
};

// The side's heroes, six at most, with the city or site each stands on
// (6087:065d).
Info survey(Game& g, Side& side) {
  AIData& d = data(g, side);
  Info info;
  d.heroes = 0;
  for (Army* a : armies(g, side.index)) {
    if (isHero(a) && !a->transit && info.heroes.size() < 6) {
      info.heroes.push_back(a);
      City* c = cityAt(g, a->x, a->y);
      info.city.push_back(c && standing(*c) ? c : nullptr);
      info.site.push_back(siteAt(g, a->x, a->y));
      d.heroes++;
    }
  }
  return info;
}

// Temples and ruins in reach (6087:0fda).
void sites(Game& g, Side& side, size_t k, Info& info) {
  Army* h = info.heroes[k];
  int range = h->aiOrder == ORDER_CITY ? 5 : 25;
  int questD = 100, siteD = 100;
  Site* questS = nullptr;
  Site* best = nullptr;
  for (auto& s : g.map->sites) {
    if (worth(g, side, s, h)) {
      int dd = dist(s.x, s.y, h->x, h->y);
      bool ok = true;
      if (s.content == site::TEMPLE) {
        bool free = g.map->options.quests != 0 && !side.quest && dd < range && dd < questD;
        if (free) { questD = dd; questS = &s; }
        else if (blessed(h, s)) ok = false;
      } else {
        for (size_t j = 0; j < info.heroes.size(); j++) {
          Army* o = info.heroes[j];
          if (j != k && o->aiOrder == ORDER_SITE && o->aiDest == s.index) ok = false;
        }
      }
      if (ok && dd <= range && dd < siteD) { siteD = dd; best = &s; }
    }
  }
  info.quest = questS;
  info.questD = questD;
  info.target = best;
  info.siteD = siteD;
}

// Items lying about within 5 (sent at a city) or 15 (6087:11f8).
void items(Game& g, Side& side, size_t k, Info& info) {
  Army* h = info.heroes[k];
  int range = h->aiOrder == ORDER_CITY ? 5 : 15;
  Item* best = nullptr;
  int bestD = 100;
  for (int i = (int)g.map->items.size() - 1; i >= 0; i--) {
    Item& it = g.map->items[i];
    if (lying(g, side, it)) {
      bool taken = false;
      for (size_t j = 0; j < info.heroes.size(); j++) {
        Army* o = info.heroes[j];
        if (j != k && o->aiOrder == ORDER_ITEM && o->aiDest == it.index) taken = true;
      }
      if (!taken) {
        int dd = dist(it.x, it.y, h->x, h->y);
        if (dd <= range && dd < bestD) { best = &it; bestD = dd; }
      }
    }
  }
  info.item = best;
  info.itemD = bestD;
}

// An enemy city the hero's stack beats (6087:1348).
void enemy(Game& g, Side& side, size_t k, Info& info) {
  AIData& d = data(g, side);
  Army* h = info.heroes[k];
  info.enemy = nullptr;
  info.enemyD = 20;
  int dest = h->aiOrder == ORDER_CITY ? h->aiDest : NONE;
  auto list = collectOrdered(g, side.index, h->x, h->y, h->aiGroup, h->aiOrder, 12);
  if (list.empty()) return;
  Sel sel = select(g, list);
  int need = list.size() < 3 ? 95 : 75;
  int bestOdds = 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (standing(c) && !cflag(d, c, CF_UNSEEN) && c.ownerIndex != NONE && state(g, side.index, c.ownerIndex) == 2) {
      int dd = dist(c.x, c.y, h->x, h->y);
      if (dd <= info.enemyD || (dest != NONE && dest == c.index)) {
        int o = odds(g, sel, c.x, c.y) + (dest != NONE && dest == c.index ? 20 : 0);
        if (o >= need && (bestOdds < o || (o == bestOdds && dd < info.enemyD))) {
          info.enemy = &c;
          info.enemyD = dd;
          bestOdds = o;
        }
      }
    }
  }
}

// Pick (ai_choose_target, 6087:0efc): 1 the quest temple, 3 the site, 4 the
// enemy city, 2 the item, 0 nothing.
int choose(Game& g, Side& side, size_t k, Info& info) {
  AIData& d = data(g, side);
  int r = g.rng.dice(1, 20, 0);
  int pick = info.quest ? 1 : 0;
  int best = 0;
  if (info.quest) best = 100 - info.questD + r;
  r = g.rng.dice(1, 20, 0);
  int s = 100 - info.siteD + r;
  if (info.target && best < s) { pick = 3; best = s; }
  City* c = info.city[k];
  if (!c || get(d.garrison, c->index) > 4) {
    r = g.rng.dice(1, 20, 0);
    s = 100 - info.enemyD + r;
    if (info.enemy && best < s) { pick = 4; best = s; }
    r = g.rng.dice(1, 20, 0);
    if (info.item && best < 100 - info.itemD + r) pick = 2;
  }
  return pick;
}

// Go for the site (3) or the item (2) (6087:07ea). 1 when it got there.
int go(Game& g, Side& side, size_t k, int kind, Info& info) {
  AIData& d = data(g, side);
  Army* h = info.heroes[k];
  int tx, ty, index;
  if (kind == 3) {
    if (!info.target) return 0;
    tx = info.target->x; ty = info.target->y; index = info.target->index;
  } else {
    if (!info.item) return 0;
    tx = info.item->x; ty = info.item->y; index = info.item->index;
  }
  auto list = collectOrdered(g, side.index, h->x, h->y, h->aiGroup, h->aiOrder, 12);
  int n = (int)list.size();
  if (n == 0) return 0;
  City* c = info.city[k];
  int near = 100;
  if (c) {
    near = cities::nearestArmy(g, side, h->x, h->y);
    Army* flier = nullptr;
    for (int i = (int)list.size() - 1; i >= 0; i--) if (flies(g, list[i])) flier = list[i];
    if (!flier) {
      if (near >= 15) { list = {h}; n = 1; }
    } else {
      list = {h, flier};
      n = 2;
    }
  }
  if (!c || get(d.garrison, c->index) != n || n > 2 || near > 14) {
    std::optional<Sel> sel;
    if (kind == 3) {
      sel = order(g, list, ORDER_SITE, index, 0);
    } else {
      sel = order(g, list, ORDER_ROAM, 0, 0);
      if (sel) sel->leader->target = std::make_pair(tx, ty);
    }
    if (sel && moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second) == 4) return 1;
  }
  return 0;
}

// Go for the enemy city (6087:0b75).
int attack(Game& g, Side& side, size_t k, Info& info) {
  AIData& d = data(g, side);
  Army* h = info.heroes[k];
  City* target = info.enemy;
  if (!target) return 0;
  auto list = collectOrdered(g, side.index, h->x, h->y, h->aiGroup, h->aiOrder, 12);
  if (list.empty()) return 0;
  City* c = info.city[k];
  if (c && get(d.garrison, c->index) == (int)list.size()) {
    int near = cities::nearestArmy(g, side, h->x, h->y);
    if (near < (g.turn < 6 ? 10 : 15)) return 0;
  }
  auto sel = order(g, list, ORDER_CITY, target->index, 0);
  if (sel) moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
  return 0;
}

// The quest hero goes for the city it is to take or raze (6087:0cf3).
bool questGo(Game& g, Side& side, size_t k, Info& info) {
  AIData& d = data(g, side);
  Army* h = info.heroes[k];
  auto q = side.quest;
  if (!(q && (q->type == quest::OCCUPY || q->type == quest::RAZE))) return false;
  City* c = q->city;
  if (!c || cflag(d, *c, CF_UNSEEN)) return false;
  int need = (c->ownerIndex == NONE || g.turn < 8) ? 1 : 2;
  auto list = collect(g, side.index, h->x, h->y, 8);
  if ((int)list.size() < need) return false;
  if (!(h->aiOrder == ORDER_CITY && h->aiDest == c->index)) {
    Sel sel = select(g, list);
    if (odds(g, sel, c->x, c->y) < 75) return false;
  }
  auto sel = order(g, list, ORDER_CITY, c->index, 0);
  if (sel) moveTo(g, *sel, sel->leader->target->first, sel->leader->target->second);
  return true;
}
}  // namespace

void phase(Game& g, Side& side) {
  AIData& d = data(g, side);
  d.questCity = NONE;
  auto q = side.quest;
  if (g.map->options.quests != 0 && q && (q->type == quest::OCCUPY || q->type == quest::RAZE) && q->city) {
    d.questCity = q->city->index;
  }
  Info info = survey(g, side);
  for (int k = (int)info.heroes.size() - 1; k >= 0; k--) {
    Army* h = info.heroes[k];
    if (h && alive(g, h) && h->owner == side.index && isHero(h)) {
      if (info.city[k]) cities::garrison(g, side, *info.city[k], true);
      bool skip = g.map->options.quests != 0 && side.quest && side.quest->hero == h && questGo(g, side, k, info);
      if (!skip && alive(g, h)) {
        if (h->aiOrder == ORDER_SITE) {
          Site* s = h->aiDest >= 0 && h->aiDest < (int)g.map->sites.size() ? &g.map->sites[h->aiDest] : nullptr;
          if (!s || !worth(g, side, *s, h)) {
            for (Army* a : onTile(g, side.index, h->x, h->y)) {
              if (a->aiOrder == h->aiOrder) {
                clearOrder(a);
                a->target.reset();
              }
            }
          } else {
            info.target = s;
            go(g, side, k, 3, info);
          }
        }
        int step = 1, tries = 0;
        bool moved = false;
        while (step != 0 && tries < 2 && alive(g, h) && h->moves > 3) {
          tries++;
          sites(g, side, k, info);
          items(g, side, k, info);
          enemy(g, side, k, info);
          int pick = choose(g, side, k, info);
          if (pick == 1) {
            step = 0;
          } else if (pick == 2 || pick == 3) {
            moved = true;
            step = go(g, side, k, pick, info);
          } else if (pick == 4) {
            moved = true;
            step = attack(g, side, k, info);
          } else {
            if (moved && g.map->options.hiddenMap != 0 && g.turn < 10 && collect(g, side.index, h->x, h->y, 8).size() == 1) {
              moves::explore(g, side, h, nullptr);
            }
            step = 0;
          }
          if (!alive(g, h) || h->owner != side.index) step = 0;
        }
      }
    }
  }
}

}  // namespace w2::ai::heroes
