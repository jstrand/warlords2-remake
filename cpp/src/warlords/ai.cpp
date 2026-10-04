#include "warlords/ai.hpp"

#include <algorithm>

#include "warlords/ai/cities.hpp"
#include "warlords/ai/core.hpp"
#include "warlords/ai/diplomacy.hpp"
#include "warlords/ai/groups.hpp"
#include "warlords/ai/heroes.hpp"
#include "warlords/ai/moves.hpp"
#include "warlords/aicard.hpp"
#include "warlords/hero.hpp"
#include "warlords/quest.hpp"

namespace w2::ai {

Hooks hooks;

namespace {
// The level's built-in settings (59bf:0d7b).
void levelDefaults(Game& g, AIData& d, int level) {
  Rng& r = g.rng;
  if (level == 0) {
    d.rebuildLimit = 30; d.cautious = 1;
    d.dieHuman = r.dice(1, 4, 0); d.dieWarlord = r.dice(1, 4, 0); d.dieLord = r.dice(1, 4, 0);
    d.maxGroups = 1; d.solidarity = 0;
    d.rebuildType = 3; d.rebuildTypeRich = 3;
    d.raze = 0; d.sack = 0; d.pillage = 0; d.perCity = 0; d.bonusHuman = 0;
    d.bonusWarlord = 0; d.bonusLord = 0; d.bonusKnight = 0; d.poor = 0; d.early = 0;
    d.humanShare = 80;
  } else if (level == 1) {
    d.rebuildLimit = 20; d.cautious = 0;
    d.dieHuman = r.dice(1, 4, 0); d.dieWarlord = r.dice(1, 4, 0); d.dieKnight = r.dice(1, 8, 0);
    d.bold = true;
    d.solidarity = 0; d.maxGroups = 2;
    d.rebuildType = 6; d.rebuildTypeRich = 18;
    d.raze = 5; d.sack = 10; d.pillage = 20; d.perCity = 1; d.bonusHuman = 5;
    d.bonusKnight = 0; d.bonusLord = 0; d.bonusWarlord = 0; d.poor = 0; d.early = 0;
    d.humanShare = 35;
  } else {
    d.rebuildLimit = 10; d.cautious = 0;
    d.dieHuman = r.dice(1, 10, 0); d.dieLord = r.dice(1, 8, 0); d.dieKnight = r.dice(1, 6, 0);
    d.maxGroups = 4; d.solidarity = 1;
    d.bold = true;
    d.rebuildType = 7; d.rebuildTypeRich = 0;
    d.raze = 5; d.sack = 10; d.pillage = 20; d.perCity = 5; d.bonusHuman = 50;
    d.bonusKnight = 0; d.bonusLord = 0; d.bonusWarlord = 0; d.poor = 50; d.early = 1;
    d.humanShare = 35;
  }
}

// Fill a computer side's AI data from its card (59bf:0d7b), dice and all.
void fromCard(Game& g, Side& side, AIData& d, const aicard::Card& card) {
  Rng& r = g.rng;
  d.dieHuman = r.dice(1, card.dieHuman, 0);
  d.dieLord = r.dice(1, card.dieLord, 0);
  d.dieKnight = r.dice(1, card.dieKnight, 0);
  d.dieWarlord = r.dice(1, card.dieWarlord, 0);
  d.maxGroups = std::max(0, std::min(core::MAX_GROUPS, card.groups));
  d.solidarity = card.solidarity;
  if (card.bold != 0) d.bold = true;
  d.cautious = card.cautious;
  d.rebuildType = card.rebuildType;
  d.rebuildTypeRich = card.rebuildTypeRich;
  d.rebuildLimit = card.rebuildLimit;
  d.raze = card.raze; d.sack = card.sack; d.pillage = card.pillage; d.perCity = card.perCity;
  d.bonusHuman = card.bonusHuman; d.bonusWarlord = card.bonusWarlord;
  d.bonusLord = card.bonusLord; d.bonusKnight = card.bonusKnight;
  d.poor = card.poor; d.early = card.early;
  d.humanShare = card.humanShare * 10 + r.dice(1, 10, 0);
  // the card's fight order replaces the side's
  g.map->fightOrder[side.index] = card.fightOrder;
}

// Split the cities no side starts with among the computer players, for the
// AI to think of as its own ground (623c:010b).
void shareOutCities(Game& g) {
  for (auto& c : g.map->cities) c.claim = c.ownerIndex != NONE ? c.ownerIndex : core::NEUTRAL;
  bool shares[8] = {false};
  int humans = 0, computers = 0;
  std::pair<int, int> from[8];
  for (int i = 0; i < 8; i++) {
    Side& s = g.map->sides[i];
    if (s.inUse) {
      if (s.computer) { shares[i] = true; computers++; }
      else humans++;
      from[i] = {s.capX, s.capY};
      City* c = core::cityAt(g, s.capX, s.capY);
      if (c) c->claim = i;
    }
  }
  if (computers == 0 && humans != 0) {
    int best = NONE, bestRoll = 0;
    for (int i = 7; i >= 0; i--) {
      if (g.map->sides[i].inUse) {
        shares[i] = true;
        int roll = g.rng.dice(1, 100, 0);
        if (best == NONE || bestRoll < roll) { best = i; bestRoll = roll; }
      }
    }
    if (best != NONE) shares[best] = false;
  }
  int turn = NONE;
  for (int i = 7; i >= 0; i--) if (shares[i]) { turn = i; break; }
  if (turn == NONE) return;
  for (;;) {
    auto [x, y] = from[turn];
    City* pick = nullptr;
    int bestD = NONE;
    for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
      City& c = g.map->cities[i];
      if (core::standing(c) && c.claim == core::NEUTRAL) {
        int d = core::dist(x, y, c.x, c.y);
        if (bestD == NONE || d < bestD) { pick = &c; bestD = d; }
      }
    }
    if (!pick) break;
    pick->claim = turn;
    Side& s = g.map->sides[turn];
    if (g.rng.dice(1, 10, -1) < 5) from[turn] = {s.capX, s.capY};
    else from[turn] = {pick->x, pick->y};
    do { turn = (turn + 1) % 8; } while (!shares[turn]);
  }
}

// ai_turn_setup (5db9:0386).
void turnSetup(Game& g, Side& side) {
  AIData& d = core::data(g, side);
  side.aiSolidarity = d.solidarity;
  bool quick = g.map->options.quickStart != 0 && g.map->options.hiddenMap != 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == side.index) {
      int r = core::role(d, c);
      if (r == core::EXPLORER || r == core::EXPLORER2) core::setRole(d, c, g.turn < 5 ? core::WEAK : core::STOP);
      if (quick) {
        if (g.turn == 1) core::setRole(d, c, core::EXPLORER);
        else if (g.turn < 3) core::setRole(d, c, core::EXPLORER2);
      }
      core::clearCflag(d, c, core::CF_CLEANED);
    }
  }
}
}  // namespace

AIData& initSide(Game& g, Side& side) {
  side.ai = std::make_shared<AIData>(core::newData());
  AIData& d = *side.ai;
  bool quick = g.map->options.quickStart != 0;
  for (auto& c : g.map->cities) {
    d.roles[c.index] = quick ? core::WEAK : 0;
    d.held[c.index] = 0;
    d.flags[c.index] = g.map->options.hiddenMap != 0 ? core::CF_UNSEEN : 0;
  }
  int level = 2;
  if (side.computer && (side.level == 0 || side.level == 1)) level = side.level;
  levelDefaults(g, d, level);
  if (side.inUse && side.computer) {
    auto card = aicard::load(g.dataDir, level, side.card);
    if (card) fromCard(g, side, d, *card);
  }
  return d;
}

void startGame(Game& g) {
  for (auto& s : g.map->sides) initSide(g, s);
  shareOutCities(g);
  for (auto& s : g.map->sides) {
    s.diploScore = g.rng.dice(1, 8, 0);
    if (g.greatest && !s.computer) s.diploScore += 400;
  }
}

void playTurn(Game& g, Side& side, const std::function<void(const char*)>& pause) {
  auto p = [&](const char* name) { if (pause) pause(name); };
  bool hidden = g.map->options.hiddenMap != 0;

  // the computer always hires an offered hero it can afford
  if (side.heroOffer && side.gold >= side.heroOffer->price) {
    hero::recruit(g, side, *side.heroOffer);
    side.heroOffer.reset();
  }
  for (Army* a : g.armies) if (a->owner == side.index) a->aiMoved = false;

  turnSetup(g, side);
  diplo::phase(g, side);                   p("diplomacy");
  heroes::phase(g, side);                  p("move hero");
  if (hidden) moves::search(g, side);
  p("move search");
  moves::heroParties(g, side);             p("move explore");
  groups::assault(g, side);                p("assault");
  moves::moveAll(g, side);                 p("move #1");
  moves::rescue(g, side);                  p("rescue");
  cities::evaluate(g, side, side.index);   p("evaluate");
  cities::clean(g, side);                  p("clean city");
  cities::neutral(g, side);                p("neutral");
  moves::moveAll(g, side);                 p("move #2");
  cities::quickAttack(g, side);
  if (hidden) cities::updateHide(g, side);
  groups::assaultXX(g, side);              p("assault XX");
  moves::specials(g, side);                p("specials");
  cities::rebuild(g, side);                p("rebuilding");
  moves::lastRescue(g, side);              p("last rescue");
  cities::production(g, side);             p("production");
  cities::vectoring(g, side);              p("vectoring");
}

void walked(Game& g, const std::vector<Army*>& stack, const move::WalkResult& r) {
  if (hooks.onWalk && r.steps > 0) hooks.onWalk(g, stack, r);
}

City* heroCity(Game& g, Side& side, City* dflt) {
  AIData& d = core::data(g, side);
  City* best = dflt;
  int bestScore = 0;
  for (int i = (int)g.map->cities.size() - 1; i >= 0; i--) {
    City& c = g.map->cities[i];
    if (c.ownerIndex == side.index) {
      int r = core::role(d, c);
      int score = 0;
      if (r == core::RALLY) score = g.rng.dice(1, 100, 100);
      else if (r == core::TAKING_NEUTRAL) score = g.rng.dice(1, 100, 50);
      else if (r == core::NEAR_NEUTRAL) score = g.rng.dice(1, 100, 0);
      if (bestScore < score) { best = &c; bestScore = score; }
    }
  }
  return best;
}

bool questCapture(Game& g, Side& side, City& c, const std::vector<Army*>& stack) {
  auto q = side.quest;
  if (g.map->options.quests == 0 || !q || q->city != &c) return false;
  if (q->type != quest::OCCUPY && q->type != quest::RAZE) return false;
  if (std::find(stack.begin(), stack.end(), q->hero) == stack.end()) return false;
  AIData& d = core::data(g, side);
  d.questsDone++;
  d.questCity = NONE;
  if (q->type == quest::RAZE) {
    core::Sel sel = core::select(g, stack);
    groups::raze(g, side, c, true, &sel);
    return true;
  }
  return false;
}

void recordBattle(Game& g, int defender, int attacker, int x, int y, int heroesLost, int armiesLost, bool allLost,
                  bool cityTile) {
  if (defender == NONE || attacker == NONE || defender == core::NEUTRAL) return;
  if (defender < 0 || defender > 7 || attacker < 0 || attacker > 7) return;
  AIData& d = core::data(g, defender);
  d.heroesKilled[attacker] += heroesLost;
  d.armiesKilled[attacker] += armiesLost;
  d.battles[attacker]++;
  if (allLost) d.lost[attacker]++;
  if (cityTile) {
    d.cityBattles[attacker]++;
    if (allLost) d.citiesLost[attacker]++;
  }
}

}  // namespace w2::ai
