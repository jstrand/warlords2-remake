// Report > Quest (auto_ui_no_quest, 4976:0167; the text, 4976:0320).
//
// Popup 2 with dialog 16 -- Done (330). "Quest" in font 1, and the strategic
// map with no city shields. With no quest, one of four lines of group 20.
// With one, SCROLL.PCK and on it, black on yellow, "%s's Quest", a black rule
// and the quest's own lines (groups 21-27). The map shows where to go: a
// line from the hero to the target with a little shield in a box -- or, for
// a quest to slay a kind of army or a side's armies, a banner on every such
// stack (834b:158d). The hero's figure is drawn over it all.
#include <set>

#include "ui/dialogs.hpp"
#include "util/util.hpp"

namespace questui {

using w2::format;

namespace {
const kit::Rect R{80, 60, 480, 312};      // popup 2
const int MAP_X = 80, MAP_Y = 60;
const int DIALOG = 16, DONE = 330;
const char* const COMPASS[8] = {"north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"};

// 828e:0b51: the compass point from (x1, y1) to (x2, y2).
int direction(int x1, int y1, int x2, int y2) {
  if (x1 == x2) return y1 < y2 ? 4 : 0;
  if (y1 == y2) return x1 < x2 ? 2 : 6;
  if (x2 < x1 && y2 < y1) return 7;
  if (x2 < x1 && y1 < y2) return 5;
  if (x1 < x2 && y2 < y1) return 1;
  if (x1 < x2 && y1 < y2) return 3;
  return 0;
}

// Where an item is: its carrier, the ruin it lies in, or the ground.
std::pair<int, int> itemPlace(const w2::Game& g, const w2::Item* it) {
  if (it->status == 3) {
    for (w2::Army* a : g.armies)
      for (w2::Item* c : a->items)
        if (c == it && a->x != w2::NONE) return {a->x, a->y};
  } else if (it->status == 2) {
    for (auto& s : g.map->sites) if (s.item == it->index) return {s.x, s.y};
  }
  return {it->x, it->y};
}

struct QuestText {
  std::vector<std::pair<int, std::string>> lines;
  int tx = w2::NONE, ty = w2::NONE;
};

QuestText questText(const w2::Game& g, const w2::Side& side, const w2::Quest& q) {
  auto t = [](int grp, int i) { return kit::text(grp, i); };
  QuestText out;
  auto add = [&](int y, const std::string& s) { out.lines.emplace_back(y, s); };
  w2::Army* h = q.hero;
  auto fabled = [&](const w2::City* c) { return g.map->options.hiddenMap != 0 && !w2::game::seen(g, side.index, c->x, c->y); };
  using namespace w2::quest;
  if (q.type == SLAY_HERO && q.army) {
    w2::Army* foe = q.army;
    add(175, t(0x15, 0)); add(195, t(0x15, 1));
    const w2::Side* fs = g.map->side(foe->owner);
    add(215, format(t(0x15, 2), fs ? fs->name : ""));
    add(235, foe->name);
    add(265, t(0x15, 3));
    out.tx = foe->x; out.ty = foe->y;
    add(285, COMPASS[direction(h->x, h->y, out.tx, out.ty)]);
  } else if (q.type == RETRIEVE_ITEM && q.item) {
    add(175, t(0x16, 0)); add(195, t(0x16, 1)); add(215, q.item->name);
    add(245, t(0x16, 2));
    auto [x, y] = itemPlace(g, q.item);
    out.tx = x; out.ty = y;
    if (x != w2::NONE) add(265, COMPASS[direction(h->x, h->y, x, y)]);
  } else if (q.type == SLAY_TYPE && q.armyType) {
    add(175, t(0x17, 0)); add(195, t(0x17, 1)); add(215, t(0x17, 2));
    add(235, q.armyType->name);
  } else if (q.type == SLAUGHTER && q.side) {
    add(175, t(0x18, 0)); add(195, format(t(0x18, 1), q.required));
    add(215, q.side->name);
    add(245, t(0x18, 2)); add(265, format(t(0x18, 3), q.done));
  } else if ((q.type == OCCUPY || q.type == RAZE) && q.city) {
    int grp = q.type == OCCUPY ? 0x19 : 0x1a;
    const w2::City* c = q.city;
    add(175, t(grp, 0));
    add(195, format(t(grp, fabled(c) ? 2 : 1), c->name));
    add(215, t(grp, 3)); add(235, t(grp, 4)); add(265, t(grp, 5));
    out.tx = c->x; out.ty = c->y;
    add(285, COMPASS[direction(h->x, h->y, out.tx, out.ty)]);
  } else if (q.type == PILLAGE_GOLD) {
    add(175, t(0x1b, 0)); add(195, format(t(0x1b, 1), q.required));
    add(215, t(0x1b, 2)); add(235, t(0x1b, 3)); add(265, t(0x1b, 4));
    add(285, format(t(0x1b, 5), q.done));
  }
  return out;
}

struct QuestDialog : kit::Modal {
  screen::View v;
  std::shared_ptr<w2::Quest> q;
  std::string noQuest;
  void mapMarks(int tx, int ty) {
    using namespace w2::quest;
    gfx::setScissor(MAP_X, MAP_Y, 224, 312);
    if (q->type == SLAY_TYPE || q->type == SLAUGHTER) {
      std::set<int> done;
      for (w2::Army* a : G.g->armies) {
        bool match = (q->type == SLAY_TYPE && q->armyType && a->type == q->armyType->id) ||
                     (q->type == SLAUGHTER && q->side && a->owner == q->side->index);
        if (match && a->x != w2::NONE && a->owner != G.player->index && !done.count(a->x + a->y * 1000) &&
            w2::game::seen(*G.g, G.player->index, a->x, a->y)) {
          done.insert(a->x + a->y * 1000);
          gfx::setColor(1, 1, 1);
          int owner = a->owner == w2::NONE ? 8 : a->owner;
          gfx::draw(G.atransShields, gfx::newQuad(owner * 16, 164, 16, 10), MAP_X + std::max(0, a->x * 2 - 1),
                    MAP_Y + std::max(0, a->y * 2 - 1));
        }
      }
    }
    gfx::setScissor();
    if (tx != w2::NONE && q->type != SLAY_TYPE && q->type != SLAUGHTER) kit::mapTarget(MAP_X, MAP_Y, q->hero->x, q->hero->y, tx, ty);
  }
  void draw() override {
    kit::popup(R);
    front::drawStrategicMap(MAP_X, MAP_Y, nullptr, true);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), "Quest", 432, 62);                 // 4125:00b4
    if (!q) {
      kit::centred(kit::font(2), noQuest, 432, 134);
    } else {
      gfx::draw(G.scrollPic, 304, 95);
      const Font& f = kit::font(2).colours(0, 7);
      kit::centred(f, q->hero->name + "'s Quest", 432, 145);
      kit::setPal(0);
      gfx::rectangle(gfx::FILL, 360, 165, 160, 1);
      QuestText t = questText(*G.g, *G.player, *q);
      for (auto& l : t.lines) kit::centred(f, l.second, 432, l.first);
      mapMarks(t.tx, t.ty);
      front::drawHeroFigure(MAP_X, MAP_Y, q->hero->x, q->hero->y);
    }
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (c && c->id == DONE) kit::pop(this);
  }
  void keypressed(const std::string& key) override {
    if (key == "escape" || key == "return" || key == "kpenter") kit::pop(this);
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<QuestDialog>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->v.state[DONE] = w2::uidata::NORMAL;
  auto q = G.player->quest;
  if (q && q->hero && G.g->alive(q->hero) && q->hero->x != w2::NONE) d->q = q;
  d->noQuest = kit::text(0x14, G.g->rng.dice(1, 4, -1));
  kit::push(d);
}

}  // namespace questui
