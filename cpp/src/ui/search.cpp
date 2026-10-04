// Hero > Search: what a ruin, a temple or a sage says (site_search,
// 6536:0000), and the game's plain message box.
//
// A ruin (6536:01ab) opens popup 4 with "Searching" and SEARCH.PCK in a black
// frame; its story is told a line at a time from (128, 300), each waiting
// for a key or a click; then dialog 13: Done (292) and, when an item was
// found and left on the ground, Take (293).
//
// A temple (4976:0000) is popup 9 -- TEMPLE.PCK -- with its name and two
// lines of greeting, over dialog 15: Bless (328) and Quest (329).
//
// A sage (6536:0aa0) is announced in the ruin popup, then has a popup of its
// own -- popup 2, the strategic map, dialog 10 -- offering where the rich
// sites nearby lie (Items, 279), a gem (Money, 280) or a patch of the hidden
// map (Maps, 281); one of them, then Done (282).
//
// The message box (8065:1160) is popup 5 -- (144, 179) 352x64 -- closed by
// any key or click.
#include "ui/dialogs.hpp"
#include "platform/sound.hpp"
#include "util/util.hpp"
#include "warlords/cues.hpp"
#include "warlords/hero.hpp"
#include "warlords/quest.hpp"
#include "warlords/site.hpp"

namespace search {

using w2::format;

namespace {
const kit::Rect MSG{144, 179, 352, 64};  // popup 5

struct Message : kit::Modal {
  std::string line1, line2;
  bool one = false;
  kit::Done after;
  void close() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void draw() override {
    kit::popup(MSG);
    gfx::setColor(1, 1, 1);
    const Font& f = kit::font(2);
    if (one) {
      kit::centred(f, line1, 320, 201);
      return;
    }
    if (!line1.empty()) kit::centred(f, line1, 320, 190);
    if (!line2.empty()) kit::centred(f, line2, 320, 212);
  }
  void mousepressed(int, int, int) override { close(); }
  void keypressed(const std::string&) override { close(); }
};

const kit::Rect RUIN{120, 50, 400, 360}; // popup 4
const int RUIN_DIALOG = 13, DONE = 292, TAKE = 293;

std::vector<std::string> ruinLines(const w2::SearchResult& r) {
  std::vector<std::string> lines;
  std::string name = r.hero ? r.hero->name : "";
  const w2::Monster* monster = r.monster ? r.monster : r.guardian;
  if (r.kind != "allies") {
    if (monster) {
      lines.push_back(format(kit::text(0x33, 0), name, monster->name));
      lines.push_back(kit::text(0x33, r.kind == "killed" ? 1 : 2));
    } else {
      lines.push_back(kit::text(0x32, 0));
    }
  }
  if (r.kind == "item" && r.item) {
    lines.push_back(format(kit::text(0x34, 0), name, r.item->name));
  } else if (r.kind == "gold") {
    lines.push_back(format(kit::text(0x35, 0), name, r.gold));
  } else if (r.kind == "allies") {
    int n = (int)r.armies.size();
    std::string tname = r.type ? r.type->name : "";
    lines.push_back(n == 1 ? format(kit::text(0x36, 0), tname, name) : format(kit::text(0x36, 1), n, tname, name));
  }
  return lines;
}

// The ruin popup telling a story. With `after`, it has no buttons: the click
// after the last line closes it and runs `after`.
struct Ruin : kit::Modal {
  std::vector<std::string> lines;
  size_t shown = 1;
  screen::View v;
  bool guarded = false;
  bool canTake = false;
  w2::Army* hero = nullptr;
  kit::Done after;

  bool done() const { return shown >= lines.size(); }
  void advance() {
    // 6536:01ab: the orchestra comes in as the guardian's line is read on
    if (guarded && shown == 1) sound::effect("orch");
    shown++;
  }
  void close() {
    kit::pop(this);
    w2::quest::event(*G.g, *G.player, "item");
  }
  void draw() override {
    kit::popup(RUIN);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), "Searching", 320, 53);        // 4125:0c90
    gfx::draw(G.searchPic, 160, 92);
    kit::setPal(0);
    kit::outline(159, 91, 322, 202);
    const Font& f = kit::font(2);
    gfx::setColor(1, 1, 1);
    for (size_t i = 0; i < shown && i < lines.size(); i++) f.draw(lines[i], 128, 300 + 20 * (int)i);
    if (done() && !after) kit::drawControls(v, hidden);
  }
  void mousepressed(int x, int y, int) override {
    if (!done()) { advance(); return; }
    if (after) {
      auto a = after;
      kit::pop(this);
      a();
      return;
    }
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c) return;
    if (c->id == TAKE && hero) {
      for (auto& it : G.g->map->items) {
        if (it.status == 1 && it.x == hero->x && it.y == hero->y) w2::hero::takeItem(*G.g, hero, &it);
      }
      close();
    } else if (c->id == DONE) {
      close();
    }
  }
  void keypressed(const std::string& key) override {
    if (!done()) { advance(); return; }
    if (after) {
      auto a = after;
      kit::pop(this);
      a();
      return;
    }
    if (key == "return" || key == "kpenter" || key == "escape") close();
  }
};

void openRuin(std::vector<std::string> lines, bool guarded, bool canTake, w2::Army* hero, kit::Done after) {
  auto d = std::make_shared<Ruin>();
  d->lines = lines;
  d->guarded = guarded;
  d->canTake = canTake;
  d->hero = hero;
  d->after = after;
  d->v = kit::view(RUIN_DIALOG);
  d->view = &d->v;
  d->v.state[DONE] = w2::uidata::NORMAL;
  d->v.state[TAKE] = w2::uidata::NORMAL;
  if (!canTake) d->hidden.insert(TAKE);
  sound::effect("dramatic");
  kit::push(d);
}

const kit::Rect TEMPLE{160, 60, 320, 280}; // popup 9
const int TEMPLE_DIALOG = 15, BLESS = 328, QUEST = 329;

struct Temple : kit::Modal {
  std::shared_ptr<w2::SearchResult> r;
  std::vector<w2::Army*> stack;
  screen::View v;
  void draw() override {
    kit::popupFrame(TEMPLE);
    gfx::setColor(1, 1, 1);
    gfx::draw(G.templePic, TEMPLE.x, TEMPLE.y);
    const Font& f = kit::font(2).colours(7, 6);
    kit::centred(f, format(kit::text(0x13, 0), r->site->name), 320, 270);
    kit::centred(f, kit::text(0x13, 1), 320, 290);
    kit::centred(f, kit::text(0x13, 2), 320, 310);
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c) return;
    if (c->id == BLESS) {
      // temple_bless (6536:08a1): how many were blessed, and a parting line
      auto res = r;
      auto st = stack;
      kit::pop(this);
      int n = w2::site::bless(*G.g, *res->site, st);
      std::string line;
      if (n == 0) line = kit::text(0x37, 0);
      else if (n == 1) line = kit::text(0x38, 0);
      else line = format(kit::text(0x38, 1), n);
      message(line, kit::text(0x38, 2));
    } else if (c->id == QUEST) {
      auto res = r;
      kit::pop(this);
      w2::quest::assign(*G.g, *G.player, res->hero);
      front::openQuest();
    }
  }
};

void openTemple(std::shared_ptr<w2::SearchResult> r, const std::vector<w2::Army*>& stack) {
  sound::music(w2::cues::TEMPLE);                       // 4976:0000
  auto d = std::make_shared<Temple>();
  d->r = r;
  d->stack = stack;
  d->v = kit::view(TEMPLE_DIALOG);
  d->view = &d->v;
  d->v.state[BLESS] = w2::uidata::NORMAL;
  bool canQuest = G.g->map->options.quests != 0 && !G.player->quest && r->hero;
  d->v.state[QUEST] = canQuest ? w2::uidata::NORMAL : w2::uidata::DISABLED;
  kit::push(d);
}

const kit::Rect SAGE{80, 60, 480, 312};  // popup 2
const int SAGE_X = 80, SAGE_Y = 60;
const int SAGE_DIALOG = 10, ITEMS = 279, GEM = 280, MAP = 281, SAGE_DONE = 282;

struct Sage : kit::Modal {
  std::shared_ptr<w2::SearchResult> r;
  w2::Army* h = nullptr;
  screen::View v;
  std::vector<std::string> said;
  std::vector<w2::site::SageEntry> list;
  bool hiddenMap = false, pointing = false;
  w2::Site* target = nullptr;
  std::optional<std::array<int, 4>> patch;

  void offers(bool on) {
    auto& st = v.state;
    if (on) {
      st[ITEMS] = !list.empty() ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      st[GEM] = w2::uidata::NORMAL;
      st[MAP] = hiddenMap ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    } else {
      st[ITEMS] = st[GEM] = st[MAP] = w2::uidata::DISABLED;
    }
    hidden = {SAGE_DONE};
  }
  void finished() {
    v.state[SAGE_DONE] = w2::uidata::NORMAL;
    hidden = {ITEMS, GEM, MAP};
    pointing = false;
  }
  void draw() override {
    kit::popup(SAGE);
    front::drawStrategicMap(SAGE_X, SAGE_Y);
    if (!target) front::drawSiteMarkers(SAGE_X, SAGE_Y, r->site);
    if (target) {
      kit::mapTarget(SAGE_X, SAGE_Y, h->x, h->y, target->x, target->y);
      front::drawHeroFigure(SAGE_X, SAGE_Y, h->x, h->y);
    }
    if (patch) {
      kit::setPal(15);
      kit::outline(SAGE_X + 2 * (*patch)[0], SAGE_Y + 2 * (*patch)[1], 2 * (*patch)[2], 2 * (*patch)[3]);
    }
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), "A Sage!", 432, 62);           // 4125:0c7c
    const Font& f = kit::font(2);
    for (int i = 0; i <= 4; i++) kit::centred(f, kit::text(0x3f, i), 432, 120 + 20 * i);
    for (size_t i = 0; i < said.size(); i++) kit::centred(f, said[i], 432, 240 + 20 * (int)i);
    kit::drawControls(v, hidden);
  }
  void items() {
    offers(false);
    std::vector<std::string> names;
    for (auto& e : list) names.push_back(e.name);
    std::weak_ptr<Modal> self = weak_from_this();
    Sage* me = this;
    choose::open("Items", names, 0, [me, self](int i) {
      if (self.expired()) return;
      if (i == w2::NONE) return me->offers(true);
      const auto& e = me->list[i];
      if (e.kind == "gold") me->said.push_back(kit::text(0x3e, 2));
      else if (e.kind == "allies") me->said.push_back(kit::text(0x3e, 3));
      else me->said.push_back(format(kit::text(0x3e, 0), e.name));
      me->said.push_back(kit::text(0x3e, 1));
      if (w2::Site* s = w2::site::sageShow(*G.g, *G.player, e, me->h->x, me->h->y)) {
        me->said.push_back(format(kit::text(0x3e, 7), s->name));
        me->target = s;
      }
      front::stratDirty();
      me->finished();
    });
  }
  void mousepressed(int x, int y, int) override {
    if (pointing) {
      // the map is region 15: a click on it picks the tile (6536:0cd6)
      int tx = (x - SAGE_X) / 2, ty = (y - SAGE_Y) / 2;
      if (x >= SAGE_X && y >= SAGE_Y && x < SAGE_X + 224 && y < SAGE_Y + 312) {
        patch = w2::site::sageMap(*G.g, *G.player, tx, ty);
        front::stratDirty();
        finished();
      }
      return;
    }
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c) return;
    if (c->id == ITEMS) {
      items();
    } else if (c->id == GEM) {
      int n = w2::site::sageGem(*G.g, *G.player);
      said.push_back(kit::text(0x3b, 0));
      said.push_back(format(kit::text(0x3b, 1), n));
      finished();
    } else if (c->id == MAP) {
      offers(false);
      for (int i = 0; i <= 2; i++) said.push_back(kit::text(0x3c, i));
      pointing = true;
    } else if (c->id == SAGE_DONE) {
      kit::pop(this);
      front::stratDirty();
    }
  }
  void keypressed(const std::string& key) override {
    if (hidden.count(SAGE_DONE)) return;
    if (key == "escape" || key == "return" || key == "kpenter") {
      kit::pop(this);
      front::stratDirty();
    }
  }
};

void openSage(std::shared_ptr<w2::SearchResult> r) {
  auto d = std::make_shared<Sage>();
  d->r = r;
  d->h = r->hero;
  d->v = kit::view(SAGE_DIALOG);
  d->view = &d->v;
  d->list = w2::site::sageList(*G.g, *G.player, d->h->x, d->h->y);
  d->hiddenMap = G.g->map->options.hiddenMap != 0;
  d->offers(true);
  kit::push(d);
}
}  // namespace

void message(const std::string& line1, const std::string& line2, kit::Done after) {
  auto d = std::make_shared<Message>();
  d->line1 = line1;
  d->line2 = line2;
  d->after = after;
  kit::push(d);
}

void say(const std::string& line, kit::Done after) {
  auto d = std::make_shared<Message>();
  d->line1 = line;
  d->one = true;
  d->after = after;
  kit::push(d);
}

void open(const std::vector<w2::Army*>& stack) {
  if (stack.empty()) return;
  auto r = w2::game::searchHere(*G.g, stack, true);
  if (!r || r->kind == "no hero") return;
  if (r->kind == "temple") return openTemple(r, stack);
  if (r->kind == "sage") {
    sound::music(w2::cues::SAGE);
    openRuin({format(kit::text(0x3a, 0), r->hero ? r->hero->name : "")}, false, false, nullptr, [r]() { openSage(r); });
    return;
  }
  front::stratDirty();
  bool guarded = r->kind != "allies" && (r->monster || r->guardian);
  bool canTake = r->kind == "item" && r->item && r->item->status == 1;
  openRuin(ruinLines(*r), guarded, canTake, r->hero, nullptr);
}

}  // namespace search
