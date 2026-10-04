// The city dialog: dialog 6 over popup 2, (80, 60) 480x312.
//
// One frame, four modes, switched by the row of buttons along the foot
// (193-196) -- 7204:0000 takes the mode as its argument:
//
//   0  Info        any city; what anyone can see of it
//   1  City        Rename, Raze, Build Prod
//   2  Production  what it builds
//   3  Vector      where what it builds goes
//
// Modes 1-3 are only for a city of the side's own (7204:03a9). The left half
// is the strategic map; the right half is auto_ui_city_info (7204:06de), one
// case per mode, every position out of its data segment (4125:0ea0 on).
#include <algorithm>
#include <cstdlib>

#include "platform/keys.hpp"
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/scn.hpp"

namespace city {

using w2::format;
using w2::NONE;

namespace {
const kit::Rect R{80, 60, 480, 312};     // popup 2
const kit::Rect MAP{80, 60, 224, 312};   // area screen 3, region 6
const int DIALOG = 6;
const int DONE = 192, DONE_PROD = 201, STOP = 202;
const int MODE_FIRST = 193;
const int SLOT_FIRST = 197;              // 197-200, Production only
const int RENAME = 203, BUILD = 204, RAZE = 205;
const int V_SEND = 210, V_MOVE = 211, V_ALL = 212;
const int V_SEND_LIT = 214, V_MOVE_LIT = 215, V_ALL_LIT = 216;

const int S_STATS = 0x74, S_CURRENT = 0x75;
const int S_RENAME = 0x77, S_RAZE = 0x78, S_BUILD = 0x79;
const int S_VECTOR = 0x9a, S_HELP1 = 0x9b, S_HELP2 = 0x9c;

// Rename and Raze keep their words in the executable (4125:1003, 4125:0a6c).
const char* RENAME_TITLE = "Rename City";
const char* RAZE_TITLE = "Raze City";

const int SHIELD_L[2] = {312, 102}, SHIELD_R[2] = {512, 102};
const int STAT_AT[3][2] = {{356, 106}, {356, 126}, {356, 150}};
const int TEXT_AT[3][2] = {{310, 259}, {310, 279}, {310, 299}};
const int INFO_SLOTS[4][2] = {{320, 190}, {376, 190}, {432, 190}, {488, 190}};
const int PROD_SLOTS[4][2] = {{312, 142}, {360, 142}, {408, 142}, {456, 142}};
const int CITY_TEXT[6][2] = {{376, 183}, {376, 203}, {376, 231}, {376, 251}, {376, 279}, {376, 299}};

bool vectorSeeAll = false;

int capitalOf(const w2::City* city) {
  for (auto& s : G.g->map->sides) if (s.capital == city) return s.index;
  return NONE;
}

// The small shield (8611:0bf7, size 4): BSHIELD.PCK's bottom row.
void smallShield(int side, int x, int y) {
  gfx::setColor(1, 1, 1);
  gfx::draw(G.shieldImg, gfx::newQuad(side * 32, 36, 32, 23), x, y);
}

const w2::Slot* building(const w2::City* city) {
  return city->producing != NONE && city->producing < (int)city->slots.size() ? &city->slots[city->producing] : nullptr;
}

// Income, defence and owner (7204:0727, :1769).
void stats(const w2::City* city) {
  const Font& f = kit::font(2);
  gfx::setColor(1, 1, 1);
  bool razed = city->razed;
  f.draw(format(kit::text(S_STATS, 0), razed ? 0 : city->income), STAT_AT[0][0], STAT_AT[0][1]);
  f.draw(format(kit::text(S_STATS, 1), razed ? 0 : city->defence), STAT_AT[1][0], STAT_AT[1][1]);
  std::string owner;
  if (razed) owner = kit::text(S_STATS, 2);
  else if (city->ownerIndex == NONE) owner = kit::text(S_STATS, 3);
  else owner = format(kit::text(S_STATS, 4), G.g->map->sides[city->ownerIndex].name);
  f.draw(owner, STAT_AT[2][0], STAT_AT[2][1]);
}

void shields(const w2::City* city) {
  int side = city->ownerIndex == NONE ? 8 : city->ownerIndex;
  kit::shield(side, SHIELD_L[0], SHIELD_L[1]);
  kit::shield(side, SHIELD_R[0], SHIELD_R[1]);
}

void tile(int mx, int my, int x, int y) {
  int t = w2::scn::tileAt(*G.g->map, mx, my);
  int sheet = t / 96, i = t % 96;
  gfx::setColor(1, 1, 1);
  gfx::draw(G.sheets[std::min(sheet, 1)], gfx::newQuad((i % 16) * 40, (i / 16) * 40, 40, 40), x, y);
}

struct CityDialog : kit::Modal {
  w2::City* city = nullptr;
  screen::View v;
  int mode = NONE, sub = 0;
  int chosen = NONE;

  bool owned() const { return city->ownerIndex == G.player->index && !w2::game::sideCities(*G.g, *G.player).empty(); }

  // Which controls this mode shows, and in what state (7204:03a9).
  void refresh() {
    auto& st = v.state;
    bool mine = owned();
    for (int i = 0; i <= 3; i++) {
      int id = MODE_FIRST + i;
      if (i == mode) st[id] = w2::uidata::ACTIVE;
      else if (i > 0 && !mine) st[id] = w2::uidata::DISABLED;
      else st[id] = w2::uidata::NORMAL;
    }
    std::set<int> shown = {193, 194, 195, 196};
    if (mode == PRODUCTION) {
      for (int i = 0; i <= 3; i++) { shown.insert(SLOT_FIRST + i); st[SLOT_FIRST + i] = w2::uidata::NORMAL; }
      shown.insert(DONE_PROD);
      st[DONE_PROD] = w2::uidata::NORMAL;
      shown.insert(STOP);
      st[STOP] = city->producing != NONE ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    } else {
      shown.insert(DONE);
      st[DONE] = w2::uidata::NORMAL;
    }
    if (mode == CITY) {
      for (int id : {RENAME, RAZE, BUILD}) { shown.insert(id); st[id] = w2::uidata::NORMAL; }
    }
    if (mode == VECTOR) {
      int all = vectorSeeAll ? V_ALL_LIT : V_ALL;
      shown.insert(all);
      st[all] = w2::uidata::NORMAL;
      int send = sub == 1 ? V_SEND_LIT : V_SEND;
      shown.insert(send);
      st[send] = (sub == 1 || city->producing != NONE) ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      int mv = sub == 2 ? V_MOVE_LIT : V_MOVE;
      shown.insert(mv);
      st[mv] = (sub == 2 || !w2::game::vectoredTo(*G.g, city).empty()) ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    }
    hidden.clear();
    for (auto& c : v.dialog.controls) if (!shown.count(c.id)) hidden.insert(c.id);
  }

  void setMode(int m) {
    if (m != INFO && city->ownerIndex != G.player->index) m = INFO;
    // 7204:0000: going into Production chooses what the city builds now
    if (m == PRODUCTION && mode != PRODUCTION) {
      const w2::Slot* b = building(city);
      chosen = b ? b->type : NONE;
    }
    mode = m;
    sub = 0;
    refresh();
  }

  void close() {
    kit::pop(this);
    tutorial::show("select");                       // 7204:0321
  }

  // Choose what the city builds: slot n of the list, from 0 (7087:00c8).
  void pick(int n) {
    if (n < (int)city->slots.size()) chosen = city->slots[n].type;
    if (chosen == NONE) return;
    for (size_t i = 0; i < city->slots.size(); i++)
      if (city->slots[i].type == chosen) w2::game::setProduction(*G.g, *city, (int)i);
    refresh();
  }

  void stop() {
    w2::game::setProduction(*G.g, *city, NONE);
    chosen = NONE;
    refresh();
  }

  void rename() {
    std::weak_ptr<Modal> self = weak_from_this();
    CityDialog* me = this;
    w2::City* c = city;
    input::Options o;
    o.title = RENAME_TITLE;
    o.lines = {"Type the new name for", "this city"};
    o.text = city->name;
    o.maxChars = 15;
    o.maxWidth = 128;                                 // 7204:2013
    o.ok = [self, me, c](const std::string& name) {
      w2::game::renameCity(*G.g, *c, name);
      if (!self.expired()) me->setMode(CITY);
    };
    o.cancelled = [self, me]() { if (!self.expired()) me->setMode(CITY); };
    input::open(o);
  }

  void raze() {
    std::weak_ptr<Modal> self = weak_from_this();
    CityDialog* me = this;
    w2::City* c = city;
    input::Options o;
    o.title = RAZE_TITLE;
    o.lines = {"Are you sure that you", "want to", "raze " + city->name + "?", "You won't be popular!"};
    o.confirm = true;
    o.ok = [self, me, c](const std::string&) {
      w2::game::raze(*G.g, *G.player, *c, {}, true);
      front::stratDirty();
      if (!self.expired()) me->setMode(INFO);
    };
    o.cancelled = [self, me]() { if (!self.expired()) me->setMode(CITY); };
    input::open(o);
  }

  void build() {
    std::weak_ptr<Modal> self = weak_from_this();
    CityDialog* me = this;
    buyprod::open(city, [self, me]() { if (!self.expired()) me->setMode(CITY); });
  }

  void drawInfo() {
    shields(city);
    stats(city);
    int owner = city->ownerIndex == NONE ? 8 : city->ownerIndex;
    if (!city->razed && (G.g->map->options.viewProduction == 0 || city->ownerIndex == G.player->index)) {
      for (int i = 0; i < 4; i++) {
        bool have = i < (int)city->slots.size();
        kit::army(have ? city->slots[i].type : NONE, owner, INFO_SLOTS[i][0], INFO_SLOTS[i][1], 1);
      }
    } else {
      kit::setPal(0);
      kit::outline(311, 169, 242, 88);
      kit::bevel(310, 168, 244, 90, 4, 2);
      gfx::setColor(1, 1, 1);
      if (G.cityBack) gfx::draw(G.cityBack, gfx::newQuad(0, 0, 240, 86), 312, 170);
      tile(city->x, city->y, 392, 176);
      tile(city->x, city->y + 1, 392, 216);
      tile(city->x + 1, city->y, 432, 176);
      tile(city->x + 1, city->y + 1, 432, 216);
    }
    int cap = capitalOf(city);
    if (cap != NONE) smallShield(cap, 408, 170);
    auto it = G.g->map->cityText.find(city->index);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    if (it != G.g->map->cityText.end())
      for (int i = 0; i < 3; i++) if (!it->second[i].empty()) f.draw(it->second[i], TEXT_AT[i][0], TEXT_AT[i][1]);
  }

  void drawCityMode() {
    shields(city);
    stats(city);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    int k = 0;
    for (int group : {S_RENAME, S_RAZE, S_BUILD}) {
      for (int i = 0; i <= 1; i++) {
        f.draw(kit::text(group, i), CITY_TEXT[k][0], CITY_TEXT[k][1]);
        k++;
      }
    }
  }

  void drawProduction() {
    const Font& f = kit::font(2);
    int owner = city->ownerIndex == NONE ? 8 : city->ownerIndex;
    int cap = capitalOf(city);
    if (cap != NONE) smallShield(cap, 312, 110);
    gfx::setColor(1, 1, 1);
    kit::right(f, kit::text(S_CURRENT, 0), 408, 110);
    const w2::Slot* b = building(city);
    kit::army(b ? b->type : NONE, owner, 416, 104, 1);
    std::string text = city->producing != NONE ? w2::fmt("%dt", city->countdown) : "-";
    gfx::setColor(1, 1, 1);
    if (city->vectorTo != NONE) {
      std::string where;
      if (city->vectorTo == w2::game::STANDARD) {
        where = kit::text(S_CURRENT, w2::game::standardAt(*G.g, *G.player).first != NONE ? 2 : 3);
      } else {
        w2::City* dest = G.g->map->city(city->vectorTo);
        where = dest ? dest->name : kit::text(S_CURRENT, 3);
      }
      f.draw(text + kit::text(S_CURRENT, 1), 456, 102);
      f.draw(where, 456, 122);
    } else {
      f.draw(text, 456, 110);
    }
    const w2::Slot* ch = nullptr;
    for (int i = 0; i < 4; i++) {
      bool have = i < (int)city->slots.size();
      bool mine = have && city->slots[i].type == chosen;
      if (mine) ch = &city->slots[i];
      kit::army(have ? city->slots[i].type : NONE, owner, PROD_SLOTS[i][0], PROD_SLOTS[i][1], mine ? G.player->index + 2 : 1);
    }
    if (ch) {
      gfx::setColor(1, 1, 1);
      gfx::draw(G.bigArmy, 320, 182);
      int x = 320 + 136;                                // 4125:060a
      f.draw(ch->name, x, 182);
      f.draw(w2::fmt("Time: %d", ch->time), x, 212);
      f.draw(w2::fmt("Cost: %d", ch->cost), x, 232);
      f.draw(w2::fmt("Strength: %d", ch->strength), x, 252);
      f.draw(w2::fmt("Move: %d", ch->move), x, 272);
    }
  }

  void drawVector() {
    const Font& f = kit::font(2);
    int owner = city->ownerIndex == NONE ? 8 : city->ownerIndex;
    gfx::setColor(1, 1, 1);
    kit::right(f, kit::text(S_VECTOR, 0), 360, 109);
    const w2::Slot* b = building(city);
    kit::army(b ? b->type : NONE, owner, 368, 103, 1);
    gfx::setColor(1, 1, 1);
    f.draw(city->producing != NONE ? format(kit::text(S_VECTOR, 1), city->countdown) : "-", 408, 109);
    w2::Army* out = nullptr;
    w2::Army* onRoad = nullptr;
    std::vector<w2::Army*> next, after;
    for (w2::Army* a : G.g->armies) {
      if (a->transit && a->owner == G.player->index) {
        if (a->homeCity == city->index && a->transit->dest == city->vectorTo) {
          if (a->transit->turns >= 2) out = a;
          else onRoad = a;
        }
        if (a->transit->dest == city->index && a->homeCity != city->index) {
          auto& row = a->transit->turns >= 2 ? after : next;
          if (row.size() < 4) row.push_back(a);
        }
      }
    }
    kit::army(out ? out->type : NONE, owner, 432, 103, 1);
    kit::army(onRoad ? onRoad->type : NONE, owner, 472, 103, 1);
    gfx::setColor(1, 1, 1);
    kit::right(f, kit::text(S_VECTOR, 2), 392, 155);
    kit::right(f, kit::text(S_VECTOR, 3), 392, 188);
    for (int i = 0; i < 4; i++) {
      int x = 400 + i * 40;
      kit::army(i < (int)next.size() ? next[i]->type : NONE, G.player->index, x, 149, 1);
      kit::army(i < (int)after.size() ? after[i]->type : NONE, G.player->index, x, 182, 1);
    }
    gfx::setColor(1, 1, 1);
    int a0 = sub == 1 ? 2 : 0;
    f.draw(kit::text(S_HELP1, a0), 368, 221);
    f.draw(kit::text(S_HELP1, a0 + 1), 368, 241);
    int b0 = sub == 2 ? 2 : 0;
    f.draw(kit::text(S_HELP2, b0), 368, 272);
    f.draw(kit::text(S_HELP2, b0 + 1), 368, 292);
  }

  void draw() override {
    kit::popup(R);
    if (mode == VECTOR) {
      int filter = NONE;
      if (sub == 1) filter = -1;
      else if (sub == 2) filter = (int)w2::game::vectoredTo(*G.g, city).size();
      front::drawVectorMap(MAP.x, MAP.y, city, filter, vectorSeeAll && sub == 0);
    } else {
      front::drawStrategicPanel(MAP.x, MAP.y, city);
    }
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), city->name, 432, 62);
    if (mode == INFO) drawInfo();
    else if (mode == CITY) drawCityMode();
    else if (mode == PRODUCTION) drawProduction();
    else drawVector();
    kit::drawControls(v, hidden);
  }

  // Move the dialog to another city, in the mode it is in (7204:0000).
  void switchTo(w2::City* target, int m) {
    int keep = m == NONE ? mode : m;
    kit::pop(this);
    open(target, keep);
  }

  // Send what this city builds to the side's city nearest (mx, my), or its
  // planted standard if that is nearer (828e:0651).
  void sendTo(int mx, int my) {
    if (city->producing == NONE) return;
    w2::City* target = w2::game::nearestCity(*G.g, mx, my, G.player);
    if (!target) return;
    auto [sx, sy] = w2::game::standardAt(*G.g, *G.player);
    bool toStandard = sx != NONE && std::max(std::abs(sx - mx), std::abs(sy - my)) <
                                        std::max(std::abs(target->x - mx), std::abs(target->y - my));
    if (toStandard) w2::game::vectorToStandard(*G.g, *city, *G.player);
    else w2::game::vector(*G.g, *city, target);
  }

  // A click on the map in Info, City or Production (7204:1afa).
  void pickOnMap(int mx, int my) {
    bool shift = keys::isDown({"lshift", "rshift"});
    if (mode == PRODUCTION && shift) {
      sendTo(mx, my);
      refresh();
      return;
    }
    w2::City* target;
    if (mode == INFO) target = w2::game::nearestCity(*G.g, mx, my, nullptr, G.player);
    else if (mode == PRODUCTION) target = w2::game::nearestCity(*G.g, mx, my, G.player);
    else target = w2::game::nearestCity(*G.g, mx, my, G.player, G.player);
    if (target && target != city) switchTo(target, NONE);
  }

  // A click on the map in Vector mode (7087:072e, 7087:028b).
  void mapClick(int mx, int my) {
    w2::City* target = w2::game::nearestCity(*G.g, mx, my, G.player);
    if (!target) return;
    bool shift = keys::isDown({"lshift", "rshift"});
    int was = sub;
    if (shift || was == 1) {
      sendTo(mx, my);
      if (shift) { sub = 0; refresh(); return; }
    } else if (was == 2) {
      auto incoming = w2::game::vectoredTo(*G.g, city);
      auto there = w2::game::vectoredTo(*G.g, target);
      bool ok = !incoming.empty() && (int)(incoming.size() + there.size()) <= w2::game::MAX_VECTORED_TO;
      if (std::find(incoming.begin(), incoming.end(), target) != incoming.end()) ok = false;
      if (ok) for (w2::City* c : incoming) c->vectorTo = target->index;
    }
    sub = 0;
    if (was != 1 && target != city) {
      switchTo(target, VECTOR);
      return;
    }
    refresh();
  }

  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (c) {
      int id = c->id;
      if (id == DONE || id == DONE_PROD) close();
      else if (id >= MODE_FIRST && id < MODE_FIRST + 4) setMode(id - MODE_FIRST);
      else if (id >= SLOT_FIRST && id < SLOT_FIRST + 4) pick(id - SLOT_FIRST);
      else if (id == STOP) stop();
      else if (id == RENAME) rename();
      else if (id == RAZE) raze();
      else if (id == BUILD) build();
      else if (id == V_SEND || id == V_SEND_LIT) { sub = sub == 1 ? 0 : 1; refresh(); }
      else if (id == V_MOVE || id == V_MOVE_LIT) { sub = sub == 2 ? 0 : 2; refresh(); }
      else if (id == V_ALL || id == V_ALL_LIT) { vectorSeeAll = !vectorSeeAll; refresh(); }
      return;
    }
    if (MAP.contains(x, y)) {
      int mx = (x - MAP.x) / 2, my = (y - MAP.y) / 2;
      if (mode == VECTOR) mapClick(mx, my);
      else pickOnMap(mx, my);
    }
  }

  // the right button on a production slot (sub-ids 1-4, 54bd:00df)
  bool info(int s, int sx, int sy) override {
    int n = s - 1;
    if (n < 0 || n >= (int)city->slots.size()) return false;
    infobox::armyType(sx, sy, city->slots[n].type, city->slots[n]);
    return true;
  }

  // The right button off the controls (1726:0009, screen 3).
  void rightpressed(int x, int y, int sx, int sy) override {
    if (MAP.contains(x, y)) {
      int first = mode == PRODUCTION ? 4 : mode == VECTOR ? 6 : 0;
      infobox::lines(sx, sy, kit::text(0x7a, first), kit::text(0x7a, first + 1));
      return;
    }
    if (mode == INFO && x >= 308 && x < 532 && y >= 180 && y < 230) {
      if (city->razed || (G.g->map->options.viewProduction != 0 && city->ownerIndex != G.player->index)) {
        infobox::lines(sx, sy, city->name, "A picture of the city!");          // 4125:0fe7
        return;
      }
      int n = std::min(3, (x - 308) / 56);
      if (n < (int)city->slots.size())
        infobox::armyType(sx, sy, city->slots[n].type, city->slots[n], city->ownerIndex == NONE ? 8 : city->ownerIndex);
      else infobox::lines(sx, sy, city->name, "Info about city production"); // 4125:0fcc
    }
  }

  void keypressed(const std::string& key) override {
    if (key == "escape" || key == "return" || key == "kpenter") close();
  }
};
}  // namespace

void open(w2::City* c, int mode) {
  auto d = std::make_shared<CityDialog>();
  d->city = c;
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  kit::push(d);
  d->setMode(mode == NONE ? (c->ownerIndex == G.player->index ? PRODUCTION : INFO) : mode);
  // the tutorial on production (7204:025d)
  std::vector<std::string> moments = {"prod"};
  if (w2::game::sideCities(*G.g, *G.player).size() >= 2) moments.push_back("prod2");
  tutorial::chain(moments);
}

}  // namespace city
