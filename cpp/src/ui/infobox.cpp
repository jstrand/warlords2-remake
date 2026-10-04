// What the right button shows off the map: a box that stays up while the
// button is held, and goes when it comes up (740d:11cf).
//
// Two boards, blitted through their masks:
//   POPUP.PCK (bitmap 29), 256x75: two lines -- the first in colour 7, the
//     second in white, centred on the box's middle less 8, at y + 11 and
//     y + 35 (740d:1201, 740d:1158).
//   POPUP3.PCK (bitmap 40), 240x128: an army (ui_army_info, 740d:0626) or an
//     army type (ui_army_type_info, 740d:032a): the name, the army on the
//     grey ring with a non-hero's medals round it, four numbers, and how the
//     type moves.
//
// A right-button press on a control looks it up in HELP\WARLORD2.HLP
// (54bd:0000): when the record's sub-id is 0 its title and description are
// the two lines; otherwise the sub-id says what the control stands for and
// the dialog it is on answers through its info().
#include <algorithm>
#include <map>

#include "front/pckimage.hpp"
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/armytype.hpp"
#include "warlords/bytes.hpp"

namespace infobox {

const Board LINES{29, "POPUP.PCK", 256, 75, 1};
const Board ARMY{40, "POPUP3.PCK", 240, 128, 10};
const Board POPUP2{36, "POPUP2.PCK", 256, 75, 1};

namespace {
std::map<int, gfx::ImageP> boards;

const int MOVE_FLY[2] = {184, 30}, MOVE_BOTH[2] = {216, 30}, MOVE_WOODS[2] = {248, 30}, MOVE_HILLS[2] = {152, 30};

struct Box : kit::Modal {
  Board which;
  int bx = 0, by = 0;
  std::function<void(int, int)> paint;
  void draw() override {
    auto b = board(which);
    gfx::setColor(1, 1, 1);
    if (b) gfx::draw(b, gfx::newQuad(0, 0, which.w, which.h), bx, by);
    gfx::setColor(1, 1, 1);
    paint(bx, by);
  }
  bool mousereleased(int, int, int) override {
    kit::pop(this);
    return true;
  }
  void mousepressed(int, int, int) override { kit::pop(this); }
  void keypressed(const std::string&) override { kit::pop(this); }
};

void show(const Board& which, int sx, int sy, std::function<void(int, int)> paint) {
  auto d = std::make_shared<Box>();
  d->which = which;
  auto [bx, by] = place(which, sx, sy);
  d->bx = bx;
  d->by = by;
  d->paint = paint;
  d->infobox = true;
  kit::push(d);
}

void armyBoard(int x, int y, const std::string& name, int typeId, int side, int medals) {
  const Font& f = kit::font(2);
  kit::centred(f, name, x + 112, y + 8);
  kit::army(typeId, side, x + 96, y + 31, 1);
  (void)medals;   // the remake keeps no medals yet
  const w2::ArmyType* t = G.g->types.byId(typeId);
  const int* src = nullptr;
  if (t && t->flies) src = MOVE_FLY;
  else if (t && t->woodsMove && t->hillsMove) src = MOVE_BOTH;
  else if (t && t->woodsMove) src = MOVE_WOODS;
  else if (t && t->hillsMove) src = MOVE_HILLS;
  if (src) {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.abits, gfx::newQuad(src[0], src[1], 32, 10), x + 96, y + 103);
  }
}

void numbers(int x, int y, const std::string& a, const std::string& b, const std::string& c, const std::string& d) {
  const Font& f = kit::font(2);
  gfx::setColor(1, 1, 1);
  kit::right(f, a, x + 104, y + 66);
  f.draw(b, x + 120, y + 66);
  kit::right(f, c, x + 104, y + 86);
  f.draw(d, x + 120, y + 86);
}

struct HelpRecord {
  int sub = 0;
  std::string title, text;
};
std::map<int, HelpRecord>& helpFile() {
  static std::map<int, HelpRecord> out;
  static bool loaded = false;
  if (loaded) return out;
  loaded = true;
  // HELP\WARLORD2.HLP (docs/formats/hlp.md): a u16 count, then 104-byte
  // records -- u16 id, u16 sub-id, title and description, 50 bytes each.
  // Only the first record with an id counts.
  if (auto s = w2::readFile(G.dataDir + "/HELP/WARLORD2.HLP")) {
    int n = w2::u16(*s, 0);
    for (int i = 0; i < n; i++) {
      size_t at = 2 + (size_t)i * 104;
      if (at + 104 > s->size()) break;
      int id = w2::u16(*s, at);
      if (!out.count(id)) out[id] = HelpRecord{w2::u16(*s, at + 2), w2::cstr(*s, at + 4, 50), w2::cstr(*s, at + 54, 50)};
    }
  }
  return out;
}
}  // namespace

gfx::ImageP board(const Board& which) {
  auto it = boards.find(which.bitmap);
  if (it != boards.end()) return it->second;
  gfx::ImageP img;
  try {
    img = pckimage::load(G.dataDir + "/PICS/" + which.file, G.palette, which.key);
  } catch (...) {
  }
  return boards[which.bitmap] = img;
}

std::pair<int, int> place(const Board& which, int sx, int sy) {
  const auto& L = G.layout;
  int hw = which.w / 2, hh = which.h / 2;
  int cx = (sx + 4) / 8 * 8;
  cx = std::max(hw, std::min(L.w - hw, cx));
  int cy = std::max(hh, std::min(L.h - 2 - hh, sy));
  return {cx - hw - L.dialog.x, cy - hh - L.dialog.y};
}

void lines(int sx, int sy, const std::string& title, const std::string& text) {
  show(LINES, sx, sy, [title, text](int x, int y) {
    const Font& f = kit::font(2);
    kit::centred(f.colours(7, 0), title, x + LINES.w / 2 - 8, y + 11);
    kit::centred(f, text, x + LINES.w / 2 - 8, y + 35);
  });
}

void army(int sx, int sy, const w2::Army* a) {
  const w2::ArmyType* t = G.g->types.byId(a->type);
  bool hero = a->hero();
  std::string name = hero ? a->name : (t ? t->name : "");
  w2::Army copy = *a;
  int side = G.player->index;
  show(ARMY, sx, sy, [name, copy, side](int x, int y) {
    armyBoard(x, y, name, copy.type, side, 0);
    numbers(x, y, w2::fmt("Strength: %d", copy.strength),     // 4125:10a8
            w2::fmt("Movement: %d", copy.maxMoves),           // 4125:10b5
            w2::fmt("Remain: %d", copy.moves),                // 4125:10c2
            w2::fmt("Upkeep: %d", copy.upkeep));              // 4125:10cd
  });
}

void armyType(int sx, int sy, int typeId, const w2::Slot& s, int side) {
  const w2::ArmyType* t = G.g->types.byId(typeId);
  std::string name = t ? t->name : "";
  int sd = side == w2::NONE ? G.player->index : side;
  w2::Slot slot = s;
  show(ARMY, sx, sy, [name, typeId, sd, slot](int x, int y) {
    armyBoard(x, y, name, typeId, sd, 0);
    numbers(x, y, w2::fmt("Strength: %d", slot.strength),     // 4125:107c
            w2::fmt("Movement: %d", slot.move),               // 4125:1089
            w2::fmt("Time: %d", slot.time),                   // 4125:1096
            w2::fmt("Cost: %d", slot.cost));                  // 4125:109f
  });
}

bool control(int sx, int sy, int id, kit::Modal* d) {
  auto& recs = helpFile();
  auto it = recs.find(id);
  if (it == recs.end()) return false;
  const HelpRecord& rec = it->second;
  if (rec.sub == 0) {
    lines(sx, sy, rec.title, rec.text);
    return true;
  }
  int sub = rec.sub;
  if (sub >= 33 && sub <= 36) {
    // the configurable buttons (545c:03f8): the menu item each one runs
    const auto& ui = G.screen->ui;
    auto item = ui.shortcuts.find(sub - 33);
    std::string name;
    if (item != ui.shortcuts.end()) {
      auto n = ui.shortcutNames.find(item->second);
      if (n != ui.shortcutNames.end()) name = n->second;
    }
    lines(sx, sy, "- User-Defined Button -", name);            // 4125:05e6
    return true;
  }
  if (d) return d->info(sub, sx, sy);
  return false;
}

const w2::uidata::Control* controlAt(const screen::View& v, int x, int y, const std::set<int>* hidden) {
  const w2::uidata::Control* found = nullptr;
  for (const auto& c : v.dialog.controls) {
    if (c.w > 0 && c.h > 0 && !(hidden && hidden->count(c.id)) && x >= c.x && x < c.x + c.w && y >= c.y && y < c.y + c.h) {
      found = &c;
    }
  }
  return found;
}

}  // namespace infobox
