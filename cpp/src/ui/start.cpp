// The start screens: the menu the game opens on, choosing a scenario, and
// setting its sides and options up (7f77:0000, 7f77:0725, 7bab:0000).
//
// The start menu (7f77:0000, dialog 1): STARTUP0-3.PCK with New Scenario
// (100), Load Game (101), Random Map (102) and Begin (103), and the
// scenario's own PICS\SCENARIO.PCK under a bar with its name. The scenario
// starts as Erythea (4125:2a6e). There is no random map generator, and Random
// Map is greyed.
//
// New Scenario (7f77:058d, 0725): popup 19, NEWSCEN.PCK, dialog 29 -- the
// scenarios of SCENARIO.DAT in a black box, seven rows, and on the crystal
// ball the chosen one's name, description, cities, ruins and players.
//
// Begin (7bab:0000) sets the sides up on the main screen's own frame,
// dialog 3: a box a side with its face and its button -- Human, Knight,
// Lord, Warlord or Off (7bab:0634); Begin (141), Main Menu (142), I am the
// Greatest (143) / No! I really am Normal (144), the presets (145-147) and
// Edit Options (148); the difficulty rating (7bab:0bab).
//
// Edit Options (7bab:12a2): popup 4, dialog 4 -- the ten options of group 4
// two to a row; 159-168 change one, 169-171 are the presets, OK (172).
#include <map>

#include "front/pckimage.hpp"
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/bytes.hpp"
#include "warlords/scn.hpp"

namespace start {

using w2::format;

namespace {
// the ten options, in the order of group 4 and the table at 4125:23b4
const char* const OPTION_KEYS[10] = {"neutralCities", "diplomacy", "quests", "hiddenMap", "viewEnemies",
                                     "viewProduction", "intenseCombat", "quickStart", "militaryAdvisor", "randomTurns"};
const int PRESETS[3][6] = {                                   // 4125:2378
    {0, 0, 0, 0, 1, 0},
    {1, 1, 1, 0, 0, 0},
    {2, 1, 1, 1, 0, 1},
};

struct Scenario {
  std::string name, dir, text;
  int cities = 0, ruins = 0, players = 0;
};

// SCENARIO.DAT's records.
std::vector<Scenario> scenarios() {
  std::vector<Scenario> out;
  auto s = w2::readFile(G.dataDir + "/DATA/SCENARIO.DAT");
  if (!s) return out;
  for (size_t i = 0; i < s->size() / 84; i++) {
    size_t o = 84 * i;
    out.push_back(Scenario{w2::cstr(*s, o, 20), w2::cstr(*s, o + 20, 8), w2::cstr(*s, o + 28, 30), w2::u16(*s, o + 76),
                           w2::u16(*s, o + 78), w2::u16(*s, o + 80)});
  }
  return out;
}

std::map<std::string, gfx::ImageP> art;
gfx::ImageP image(const std::string& path, int key = pckimage::NOKEY) {
  auto it = art.find(path);
  if (it != art.end()) return it->second;
  gfx::ImageP img;
  try {
    img = pckimage::load(path, G.palette, key);
  } catch (...) {
  }
  return art[path] = img;
}

struct SideSetup {
  bool inUse = false;
  std::string name;
  int colour = 15, edge = 0;
  bool computer = false;
  int level = 0, card = 0;
};

struct Setup {
  Scenario sc;
  int options[10] = {0};
  SideSetup sides[8];
  bool greatest = false;
};

std::shared_ptr<Setup> newSetup(const Scenario& sc) {
  std::string dir = w2::upper(sc.dir);
  auto map = w2::scn::load(G.dataDir + "/" + dir, dir);
  auto st = std::make_shared<Setup>();
  st->sc = sc;
  for (int i = 0; i <= 9; i++) st->options[i] = *map->options.field(OPTION_KEYS[i]);
  for (auto& s : map->sides) {
    SideSetup& e = st->sides[s.index];
    e.inUse = s.inUse;
    e.name = s.name;
    e.colour = s.colour;
    e.edge = s.edge;
    e.computer = s.computer;
    e.level = s.computer ? s.level : 0;
    e.card = s.card;
  }
  return st;
}

bool presetMatches(const Setup& st, int p) {
  for (int i = 0; i <= 5; i++) if (st.options[i] != PRESETS[p][i]) return false;
  return true;
}

// 7bab:0bab: the options' weight and the computers' strength, a percent.
int rating(const Setup& st) {
  const int* o = st.options;
  int cx = std::min(20, o[0] * 4 + o[1] * 4 + o[2] * 3 + o[3] * 4 - o[4] + o[5]);
  int sum = 0, n = 0;
  for (auto& s : st.sides) {
    if (s.inUse && s.computer && s.level != 3) { sum += s.level + 1; n++; }
  }
  int v = n == 0 ? 80 : sum * 80 / (n * 3);
  if (v >= 78) v = 80;
  return v + cx;
}

struct Options : kit::Modal {
  std::shared_ptr<Setup> st;
  screen::View v;
  kit::Done after;
  void refresh() {
    auto& s = v.state;
    for (int i = 0; i <= 9; i++) s[159 + i] = w2::uidata::NORMAL;
    for (int p = 0; p <= 2; p++) s[169 + p] = presetMatches(*st, p) ? w2::uidata::ACTIVE : w2::uidata::NORMAL;
    s[172] = w2::uidata::NORMAL;
  }
  void draw() override {
    const kit::Rect R{120, 50, 400, 360};   // popup 4
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(8, 0), 320, 55);
    const Font& f = kit::font(2);
    const int ys[2] = {111, 250};
    for (int k = 0; k < 2; k++) {
      int y = ys[k];
      kit::setPal(15);
      gfx::rectangle(gfx::FILL, 160, y, 320, 1);
      kit::setPal(0);
      gfx::rectangle(gfx::FILL, 161, y + 1, 320, 1);
      std::string t = kit::text(9 + k, 0);
      int w = f.width(t);
      kit::setPal(3);
      gfx::rectangle(gfx::FILL, 320 - w / 2 - 4, y, w + 8, 2);
      gfx::setColor(1, 1, 1);
      kit::centred(f, t, 320, y - 7);
    }
    for (int i = 0; i <= 9; i++) {
      int x = i % 2 == 0 ? 128 : 320;
      int y = i < 6 ? 131 + 30 * (i / 2) : 270 + 30 * ((i - 6) / 2);
      gfx::setColor(1, 1, 1);
      f.draw(kit::text(4, i), x, y);
      int val = st->options[i];
      std::string word = i == 0 ? kit::text(5, val) : (val != 0 ? "On" : "Off");
      f.colours(7, 0).draw(word, x + 128, y);
    }
    kit::drawControls(v);
  }
  void finish() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    int id = c->id;
    if (id == 172) return finish();
    if (id >= 159 && id <= 168) {
      int i = id - 159;
      if (i == 0) st->options[0] = (st->options[0] + 1) % 3;
      else st->options[i] = st->options[i] != 0 ? 0 : 1;
    } else if (id >= 169 && id <= 171) {
      for (int k = 0; k <= 5; k++) st->options[k] = PRESETS[id - 169][k];
    }
    refresh();
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") finish();
  }
};

void openOptions(std::shared_ptr<Setup> st, kit::Done after) {
  auto d = std::make_shared<Options>();
  d->st = st;
  d->after = after;
  d->v = kit::view(4);
  d->view = &d->v;
  d->refresh();
  kit::push(d);
}

const int FACES[7][2] = {{0, 0}, {0, 40}, {0, 80}, {0, 120}, {0, 160}, {0, 200}, {440, 40}};
const int LEVEL_BUTTON[5] = {120, 200, 280, 360, 40};   // 4125:24b8: Knight .. Off, Human

kit::Rect rectFor(int i) { return kit::Rect{i < 4 ? 24 : 208, 40 + 90 * (i % 4), 160, 70}; }   // 4125:23c8

struct SetupScreen : kit::Modal {
  std::shared_ptr<Setup> st;
  screen::View v;
  std::function<void(std::shared_ptr<Setup>)> begin;
  kit::Done back;

  void refresh() {
    auto& s = v.state;
    for (int i = 0; i < 8; i++) s[125 + i] = w2::uidata::NORMAL;
    for (int p = 0; p <= 2; p++) s[145 + p] = presetMatches(*st, p) ? w2::uidata::ACTIVE : w2::uidata::NORMAL;
    s[141] = s[142] = s[148] = s[143] = s[144] = w2::uidata::NORMAL;
    hidden = {st->greatest ? 143 : 144, 157, 158};
    for (int i = 0; i < 8; i++) { hidden.insert(133 + i); hidden.insert(149 + i); }
  }
  void blit(int sx, int sy, int w, int h, int x, int y) {
    auto a = G.screen->artFor(31);              // SETUPBU.PCK
    if (!a) return;
    gfx::setColor(1, 1, 1);
    gfx::draw(a->image, gfx::newQuad(sx, sy, w, h), x, y);
  }
  void draw() override {
    screen::drawBackground(*G.screen, layout::compute(640, 480));
    gfx::setColor(1, 1, 1);
    gfx::draw(G.marble, gfx::newQuad(0, 0, 360, 360), 16, 30);
    gfx::draw(G.marble, gfx::newQuad(0, 0, 224, 312), 400, 30);
    gfx::draw(G.marble, gfx::newQuad(0, 60, 360, 66), 16, 403);
    gfx::draw(G.marble, gfx::newQuad(0, 0, 224, 114), 400, 355);
    const Font& f = kit::font(2);
    for (int i = 0; i < 8; i++) {
      const SideSetup& s = st->sides[i];
      kit::Rect R = rectFor(i);
      if (s.name.empty()) continue;
      int c = s.colour, e = s.edge;
      kit::setPal(c);
      kit::outline(R.x, R.y, R.w, R.h);
      gfx::rectangle(gfx::FILL, R.x - 1, R.y - 1, R.w + 2, 1);
      gfx::rectangle(gfx::FILL, R.x - 1, R.y - 1, 1, R.h + 2);
      gfx::rectangle(gfx::FILL, R.x + 1, R.y + R.h - 2, R.w - 2, 1);
      gfx::rectangle(gfx::FILL, R.x + R.w - 2, R.y + 1, 1, R.h - 2);
      kit::setPal(e);
      gfx::rectangle(gfx::FILL, R.x + 1, R.y + 1, R.w - 2, 1);
      gfx::rectangle(gfx::FILL, R.x + 1, R.y + 1, 1, R.h - 2);
      gfx::rectangle(gfx::FILL, R.x - 1, R.y + R.h, R.w + 2, 1);
      gfx::rectangle(gfx::FILL, R.x + R.w, R.y - 1, 1, R.h + 2);
      int nx = R.x + 24, ny = R.y - 8;
      int w = f.width(s.name);
      kit::setPal(3);
      gfx::rectangle(gfx::FILL, nx - 4, ny, w + 8, 20);
      gfx::setColor(1, 1, 1);
      f.colours(c, e).draw(s.name, nx, ny);
      // the face (7bab:0634): Off, a human (two faces, turn about), or the
      // computer's level; the button the same, Off and Human mapped in
      int face;
      if (!s.inUse) face = 6;
      else if (!s.computer) face = i % 2 == 1 ? 5 : 4;
      else face = std::clamp(s.level, 0, 3);
      blit(FACES[face][0], FACES[face][1], 40, 40, R.x + 16, R.y + 16);
      int btn = (face == 6 || face == 3) ? 3 : (face >= 4 ? 4 : face);
      blit(LEVEL_BUTTON[btn], 0, 80, 25, R.x + 72, R.y + 12);
    }
    gfx::setColor(1, 1, 1);
    kit::centred(f, kit::text(7, 0), 512, 48);
    kit::centred(f, kit::text(7, 1), 512, 206);
    kit::centred(f, format(kit::text(6, 0), rating(*st)), 196, 426);
    kit::drawControls(v, hidden);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c) return;
    int id = c->id;
    if (id >= 125 && id <= 132) {
      // 7bab:0a4e: Human, Knight, Lord, Warlord, Off, and round again
      SideSetup& s = st->sides[id - 125];
      if (s.inUse) {
        if (!s.computer) { s.computer = true; s.level = 0; }
        else if (s.level == 3) { s.computer = false; s.level = 0; }
        else s.level++;
        s.card = 0;                    // the level's Standard character
      }
    } else if (id == 141) {
      int playing = 0;
      for (auto& s : st->sides) if (s.inUse && !(s.computer && s.level == 3)) playing++;
      if (playing < 1) return;
      auto b = begin;
      auto s = st;
      kit::pop(this);
      b(s);
      return;
    } else if (id == 142) {
      auto b = back;
      kit::pop(this);
      b();
      return;
    } else if (id == 143) {
      st->greatest = true;
      for (auto& s : st->sides) {
        if (s.inUse) {
          if (!(s.computer && s.level == 2)) s.card = 0;
          s.computer = true;
          s.level = 2;
        }
      }
    } else if (id == 144) {
      st->greatest = false;
    } else if (id >= 145 && id <= 147) {
      for (int k = 0; k <= 5; k++) st->options[k] = PRESETS[id - 145][k];
    } else if (id == 148) {
      std::weak_ptr<Modal> self = weak_from_this();
      SetupScreen* me = this;
      openOptions(st, [self, me]() { if (!self.expired()) me->refresh(); });
      return;
    }
    refresh();
  }
};

void openSetup(std::shared_ptr<Setup> st, std::function<void(std::shared_ptr<Setup>)> begin, kit::Done back) {
  auto d = std::make_shared<SetupScreen>();
  d->st = st;
  d->begin = begin;
  d->back = back;
  d->v = kit::view(3);
  d->v.screen = true;     // a screen, not a dialog: Begin has no ring
  d->view = &d->v;
  d->menuBar = true;      // the menu bar stays live over it (7bab:0034)
  d->refresh();
  kit::push(d);
}

struct Chooser : kit::Modal {
  std::vector<Scenario> list;
  screen::View v;
  int topRow = 0, cur = 0;
  std::function<void(int)> after;
  void refresh() {
    auto& s = v.state;
    for (auto& c : v.dialog.controls) s[c.id] = w2::uidata::NORMAL;
    int up = topRow > 0 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    int down = topRow + 7 < (int)list.size() ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    s[470] = up; s[472] = up; s[471] = down; s[473] = down;
  }
  void draw() override {
    const kit::Rect R{80, 45, 480, 360};    // popup 19
    kit::popupFrame(R);
    auto pic = image(G.dataDir + "/PICS/NEWSCEN.PCK");
    gfx::setColor(1, 1, 1);
    if (pic) gfx::draw(pic, R.x, R.y);
    kit::setPal(0);
    gfx::rectangle(gfx::FILL, 112, 153, 136, 140);
    gfx::setColor(1, 1, 1);
    const Font& f = kit::font(2);
    for (int i = 0; i <= 6; i++) {
      if (topRow + i < (int)list.size()) f.colours(i == cur ? 15 : 5, 0).draw(list[topRow + i].name, 112, 153 + 20 * i);
    }
    if (topRow + cur < (int)list.size()) {
      const Scenario& e = list[topRow + cur];
      const Font& g5 = f.colours(5, 0);
      kit::centred(g5, e.name, 416, 101);
      kit::centred(g5, e.text, 416, 174);
      kit::right(g5, w2::fmt("%d", e.cities), 368, 234);
      g5.draw(w2::fmt("%d", e.ruins), 472, 234);
      kit::centred(g5, w2::fmt("%d", e.players), 416, 254);
    }
    std::set<int> h = {476, 477, 478, 479, 480, 481, 482};
    kit::drawControls(v, h);
  }
  void take(bool ok) {
    int pick = ok ? topRow + cur : w2::NONE;
    auto a = after;
    kit::pop(this);
    a(pick);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c || v.stateOf(c->id) == w2::uidata::DISABLED) return;
    int id = c->id;
    if (id == 474) return take(true);
    if (id == 475) return take(false);
    if (id >= 476 && id <= 482) {
      if (topRow + id - 476 < (int)list.size()) cur = id - 476;
    } else if (id == 470 || id == 472) {
      topRow = std::max(0, topRow - (id == 472 ? 7 : 1));
    } else if (id == 471 || id == 473) {
      topRow = std::min(std::max(0, (int)list.size() - 7), topRow + (id == 473 ? 7 : 1));
    }
    refresh();
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter") take(true);
    else if (key == "escape") take(false);
  }
};

void openChooser(const std::vector<Scenario>& list, int current, std::function<void(int)> after) {
  auto d = std::make_shared<Chooser>();
  d->list = list;
  d->cur = current;
  d->after = after;
  d->v = kit::view(29);
  d->view = &d->v;
  while (d->cur >= 7) { d->cur--; d->topRow++; }
  d->refresh();
  kit::push(d);
}

struct Menu : kit::Modal {
  std::vector<Scenario> list;
  screen::View v;
  int cur = 0;
  std::function<void(const std::string&, const w2::game::NewGameOptions&)> beginGame;
  std::function<void(std::unique_ptr<w2::Game>)> loaded;
  void refresh() {
    auto& s = v.state;
    s[100] = s[101] = s[103] = w2::uidata::NORMAL;
    s[102] = w2::uidata::DISABLED;
    hidden.clear();
    for (int id = 104; id <= 113; id++) hidden.insert(id);
  }
  void draw() override {
    gfx::setColor(1, 1, 1);
    for (int q = 0; q < 4; q++) {
      auto img = image(G.dataDir + "/PICS/STARTUP" + std::to_string(q) + ".PCK");
      if (img) gfx::draw(img, (q % 2) * 320, (q / 2) * 240);
    }
    if (cur < (int)list.size()) {
      const Scenario& sc = list[cur];
      std::string dir = w2::upper(sc.dir);
      if (auto pic = image(G.dataDir + "/" + dir + "/PICS/SCENARIO.PCK")) {
        gfx::setColor(1, 1, 1);
        gfx::draw(pic, gfx::newQuad(0, 0, 264, 225), 328, 200);
      }
      kit::setPal(3);
      gfx::rectangle(gfx::FILL, 336, 166, 248, 28);
      gfx::setColor(1, 1, 1);
      kit::centred(kit::font(2), sc.name, 460, 172);
    }
    kit::drawControls(v, hidden);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c || v.stateOf(c->id) == w2::uidata::DISABLED) return;
    int id = c->id;
    std::weak_ptr<Modal> self = weak_from_this();
    Menu* me = this;
    if (id == 100) {
      openChooser(list, cur, [self, me](int i) {
        if (!self.expired() && i != w2::NONE) me->cur = i;
      });
    } else if (id == 101) {
      auto l = loaded;
      savegame::load([self, l](std::unique_ptr<w2::Game> g) {
        if (auto s = self.lock()) kit::pop(s.get());
        l(std::move(g));
      });
    } else if (id == 103 && cur < (int)list.size()) {
      auto keep = shared_from_this();
      kit::pop(this);
      auto st = newSetup(list[cur]);
      auto bg = beginGame;
      openSetup(st,
                [bg](std::shared_ptr<Setup> s) {
                  w2::game::NewGameOptions o;
                  for (int i = 0; i <= 9; i++) o.options.emplace_back(OPTION_KEYS[i], s->options[i]);
                  for (int i = 0; i < 8; i++) {
                    const SideSetup& e = s->sides[i];
                    w2::game::SideSetup ss;
                    ss.computer = e.computer;
                    ss.level = e.level;
                    ss.card = e.card;
                    ss.off = e.computer && e.level == 3;
                    o.sides[i] = ss;
                  }
                  o.greatest = s->greatest;
                  bg(w2::upper(s->sc.dir), o);
                },
                [keep]() {
                  auto m = std::static_pointer_cast<Menu>(keep);
                  m->refresh();
                  kit::push(keep);
                });
    }
  }
};
}  // namespace

void open(std::function<void(const std::string& dir, const w2::game::NewGameOptions& opts)> begin,
          std::function<void(std::unique_ptr<w2::Game>)> loaded) {
  auto d = std::make_shared<Menu>();
  d->list = scenarios();
  for (size_t i = 0; i < d->list.size(); i++) if (d->list[i].dir == "Erythea") d->cur = (int)i;   // 4125:2a6e
  d->beginGame = begin;
  d->loaded = loaded;
  d->v = kit::view(1);
  d->v.screen = true;     // a screen, not a dialog: Begin has no ring
  d->view = &d->v;
  d->menuBar = true;
  d->refresh();
  kit::push(d);
}

}  // namespace start
